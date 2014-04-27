#include "translator.h"

namespace ppc
{

namespace Translator
{

bool setConditionRegister0(State *state, exjit::Value *value)
{
}

/* Add */
bool add(State *state, Instruction instr)
{
   auto lhs = state->gpr(instr.rA);
   auto rhs = state->gpr(instr.rB);

   auto res = state->builder.createAdd(lhs, rhs);

   state->setGpr(instr.rD, res);

   if (instr.rc) {
      setConditionRegister0(state, res);
   }

   return true;
}

} // namespace Interpreter

} // namespace ppc
