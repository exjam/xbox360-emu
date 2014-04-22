#include "system.h"

#include "powerpc/cpu.h"
#include "powerpc/interpreter/interpreter.h"
#include "powerpc/disassembler/disassembler.h"

#include "common/log.h"
#include "common/memory.h"

#include <fstream>
#include <Windows.h>

System::System()
{
}

bool System::load(const std::string &path)
{
   xex::Loader test;
   std::ifstream file;

   file.open(path, std::ifstream::in | std::ifstream::binary);

   if (!file.is_open()) {
      xDebug() << "Could not open " << path << " for reading";
      return false;
   }

   test.load(file, mBinary);
   file.close();

   if (!initKernel()) {
      xDebug() << "Could not resolve init kernel for " << path;
      return false;
   }

   return true;
}

xex::Binary *System::getXexBinary()
{
   return &mBinary;
}

void System::resumeThread(Thread *thread)
{
   ppc::Instruction ins;
   ppc::Interpreter::State *state;

   if (std::find(mThreads.begin(), mThreads.end(), thread) == mThreads.end()) {
      mThreads.push_back(thread);
   }
   
   if (!thread->stack) {
      thread->stack = new uint8_t[thread->stackSize];
   }

   if (!thread->state) {
      auto state = new ppc::Interpreter::State();
      auto kthread = new KThread();
      auto kprocess = new KProcess();
      auto pcr = new KPcr();

      memset(pcr, 0, sizeof(KPcr));
      memset(kthread, 0, sizeof(KThread));
      memset(kprocess, 0, sizeof(KProcess));
      memset(state, 0, sizeof(ppc::Interpreter::State));
      
      thread->state = state;
      thread->pcr = pcr;
      pcr->PrcbData.CurrentThread = kthread;
      kthread->ApcState.Process = kprocess;

      kthread->ThreadId = GetCurrentThreadId();

      // Set up TLS
      kprocess->SizeOfTlsSlots = mBinary.header.tlsInfo.dataSize * mBinary.header.tlsInfo.slotCount;
      memset(kprocess->TlsSlotBitmap, 0xff, 8 * 4);
      
      // TlsData works like stack, subtracts from ptr!
      pcr->TlsData = (new uint8_t[kprocess->SizeOfTlsSlots]) + kprocess->SizeOfTlsSlots;
      
      state->cia = thread->startAddress;
      state->nia = state->cia + 4;

      // Initial register values
      state->reg.gpr[1] = reinterpret_cast<uint64_t>(thread->stack) + thread->stackSize;
      state->reg.gpr[3] = thread->startParameter;
      state->reg.gpr[13] = reinterpret_cast<uint64_t>(thread->pcr);
   }

   state = reinterpret_cast<ppc::Interpreter::State *>(thread->state);

   FILE* fh;
   fopen_s(&fh, "out.txt", "w");

   analyse(0x82F088D0);

   for (unsigned i = 0; i < 99999; ++i) {
      ppc::Disassembler::State dis;

      dis.cia = state->cia;
      ins.value = Memory::read<uint32_t>(state->cia);

      ppc::Disassembler::decode(&dis, ins);

      xDebug()
         << Log::hex(static_cast<uint32_t>(state->cia))
         << " " << dis.result.disasm;

      ppc::Interpreter::decode(state, ins);


      if (state->nia != state->cia + 4) {
         fprintf(fh, "%08X branch to %08X\n", state->cia, state->nia);
         xDebug() << "BRANCH";
      }

      state->cia = state->nia;
      state->nia = state->cia + 4;
   }
}

bool System::start()
{
   Thread *thread = new Thread();

   memset(thread, 0, sizeof(Thread));

   thread->stackSize    = mBinary.header.defaultStackSize.size;
   thread->startAddress = mBinary.header.entryPoint.address;

   resumeThread(thread);

   return true;
}
