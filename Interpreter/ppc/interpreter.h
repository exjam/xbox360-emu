#ifndef PPC_INTERPRETER_H
#define PPC_INTERPRETER_H

#include "ppc/cpu.h"

namespace ppc
{

namespace Interpreter
{

struct State {
   uint64_t cia;
   uint64_t nia;
   Registers reg;
};

bool decode(State *state, Instruction instr);

bool raise(State *state, ppc::Exceptions exception);

#include <emugen_stubs.h>

} // namespace Interpreter

} // namespace ppc

#endif // PPC_INTERPRETER_H
