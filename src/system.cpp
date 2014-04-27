#include "system.h"

#include "powerpc/cpu.h"
#include "powerpc/interpreter/interpreter.h"
#include "powerpc/disassembler/disassembler.h"

#include "common/log.h"
#include "common/memory.h"

#include "kernel/xboxkrnl/thread.h"

#include "kernel/xboxkrnl/thread.h"
#include "kernel/xboxkrnl/process.h"

#include <fstream>

System::System()
{
}

xex::Binary *System::getBinary()
{
   return &mBinary;
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

   return true;
}

bool System::start()
{
   be_ptr32_t<Thread> thread;

   // Initialize kernel
   initKernel();

   // Create main thread
   ExCreateThread(reinterpret_cast<be_uint32_t*>(&thread),
                  mBinary.header.defaultStackSize.size,
                  nullptr,
                  nullptr,
                  reinterpret_cast<void*>(mBinary.header.entryPoint.address),
                  nullptr,
                  0);

   // Swap pointer back to little endian memory...
   thread->join();

   return true;
}

void System::resumeThread(Thread *thread)
{
   ppc::Instruction ins;
   auto state = reinterpret_cast<ppc::Interpreter::State*>(thread->getState());

   if (!state) {
      // Create a new interpreter state for this thread
      state = new ppc::Interpreter::State();
      thread->setState(state);

      // Zero it first pls..
      memset(state, 0, sizeof(ppc::Interpreter::State));

      // Set the entry point
      state->cia = thread->getStartAddress().getRawPointer();
      state->nia = state->cia + 4;

      // Initial register values
      state->reg.gpr[1] = thread->getStackBase().getRawPointer();
      state->reg.gpr[3] = thread->getStartParameter().getRawPointer();
      state->reg.gpr[13] = thread->getPcr().getRawPointer();
   }

   // XXX: Debug
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
         xDebug() << "BRANCH";
      }

      state->cia = state->nia;
      state->nia = state->cia + 4;
   }
}
