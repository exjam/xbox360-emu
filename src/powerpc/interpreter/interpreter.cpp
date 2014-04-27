#include "interpreter.h"

#include "powerpc/instructions.h"
#include "powerpc/decoder.h"

namespace ppc 
{

namespace Interpreter
{

typedef bool (*fptr_t)(State *, Instruction);

#include "powerpc/emugen_table.h"

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
