#include "interpreterengine.h"

#include "ppc/cpu.h"
#include "ppc/interpreter.h"
#include "ppc/disassembler.h"

#include "util/log.h"
#include "util/be/memory.h"

InterpreterEngine::InterpreterEngine()
{
}

bool InterpreterEngine::run(xex::Binary &xex)
{
   ppc::Interpreter::State intState;
   ppc::Instruction ins;
   ppc::Interpreter::init();
   ppc::Disassembler::init();

   memset(&intState, 0, sizeof(ppc::Interpreter::State));

   intState.cia = xex.header.entryPoint.address;
   intState.nia = xex.header.entryPoint.address + 4;

   uint32_t stackSize = 0x400000;
   uint8_t *stack = new uint8_t[stackSize];
   intState.reg.gpr[1] = (uint64_t)stack + stackSize;

   for (uint32_t i = 0; i < 9999; ++i) {
      ppc::Disassembler::State disState;
      disState.cia = intState.cia;

      ins.value = be::Memory::read<uint32_t>(intState.cia);

      ppc::Disassembler::decode(&disState, ins);
      xDebug() << Log::hex(static_cast<uint32_t>(intState.cia)) << " " << disState.result.code << " " << disState.result.operands;

      ppc::Interpreter::decode(&intState, ins);
      
      intState.cia = intState.nia;
      intState.nia = intState.cia + 4;
   }

   return true;
}
