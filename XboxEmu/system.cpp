#include "system.h"

#include <ppc/cpu.h>
#include <ppc/interpreter.h>
#include <ppc/disassembler.h>

#include <util/log.h>
#include <util/be/memory.h>
#include <fstream>

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

   ppc::Interpreter::init();
   ppc::Disassembler::init();

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
      ppc::Interpreter::State *state = new ppc::Interpreter::State();
      thread->state = state;
      
      auto thrState = new XThreadState();
      thread->xthread.state = thrState;
      thread->xthread.state->threadId = GetCurrentThreadId();


      auto thrTLS = new XThreadLocalStorage();
      thread->xthread.state->tls = thrTLS;
      thread->xthread.state->tls->tlsSize = 0x400;

      memset(thread->xthread.state->tls->tlsVarBitset, 0xff, 8 * sizeof(XDWORD));

      thread->xthread.tlsVars = new XBYTE[thread->xthread.state->tls->tlsSize];
      thread->xthread.tlsVars.value += thread->xthread.state->tls->tlsSize;
      
      memset(state, 0, sizeof(ppc::Interpreter::State));

      state->cia = thread->startAddress;
      state->nia = state->cia + 4;
      state->reg.gpr[1] = reinterpret_cast<uint64_t>(thread->stack) + thread->stackSize;
      state->reg.gpr[3] = thread->startParameter;
      state->reg.gpr[13] = reinterpret_cast<uint64_t>(&thread->xthread);
   }

   state = reinterpret_cast<ppc::Interpreter::State *>(thread->state);

   for (uint32_t i = 0; i < 99999; ++i) {
      ppc::Disassembler::State dis;

      dis.cia = state->cia;
      ins.value = be::Memory::read<uint32_t>(state->cia);

      ppc::Disassembler::decode(&dis, ins);

      xDebug()
         << Log::hex(static_cast<uint32_t>(state->cia))
         << " " << dis.result.code
         << " " << dis.result.operands;

      ppc::Interpreter::decode(state, ins);

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
