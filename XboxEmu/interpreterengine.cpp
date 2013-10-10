#include "interpreterengine.h"

#include "ppc/cpu.h"
#include "ppc/interpreter.h"
#include "ppc/disassembler.h"

#include "util/log.h"
#include "util/be/memory.h"

InterpreterEngine::InterpreterEngine()
{
}

bool InterpreterEngine::run(uint32_t address)
{
   ppc::Interpreter::State intState;
   ppc::Instruction ins;
   ppc::Interpreter::init();
   ppc::Disassembler::init();

   memset(&intState, 0, sizeof(ppc::Interpreter::State));

   intState.cia = address;
   intState.nia = address + 4;

   uint32_t stackSize = 0x00040000;
   uint8_t *stack = new uint8_t[stackSize];
   intState.reg.gpr[1] = (uint64_t)stack + stackSize;

   for (uint32_t i = 0; i < 16; ++i) {
      ppc::Disassembler::State disState;
      disState.cia = intState.cia;

      ins.value = be::Memory::read<uint32_t>(intState.cia);

      ppc::Disassembler::decode(&disState, ins);
      ppc::Interpreter::decode(&intState, ins);

      xDebug() << ppc::Disassembler::toString(&disState).c_str();

      intState.cia = intState.nia;
      intState.nia = intState.cia + 4;
   }

   return true;
}
