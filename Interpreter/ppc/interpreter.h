#ifndef PPC_INTERPRETER_H
#define PPC_INTERPRETER_H

#include "ppc/cpu.h"

namespace ppc
{
struct Instruction;

namespace Interpreter
{

struct State {
   uint64_t cia;
   uint64_t nia;
   Registers reg;
};

bool raise(State *state, ppc::Exceptions exception);

#include "emugen_instr_table.h"

} // namespace Interpreter

} // namespace ppc

#endif // PPC_INTERPRETER_H
