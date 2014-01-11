#include "ppc/interpreter.h"

#include <ppc/instructions.h>
#include <ppc/decoder.h>

namespace ppc 
{

namespace Interpreter
{

typedef bool (*fptr_t)(State *, Instruction);

#include <emugen_table.h>

bool decode(State *state, Instruction instr)
{
   InstructionID id = ppc::decode(instr);

   if (id == InstructionID::Unknown) {
      return false;
   } else {
      return _emugenTable[static_cast<unsigned>(id)](state, instr);
   }
}

} // namespace Interpreter

} // namespace ppc
