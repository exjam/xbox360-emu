#ifndef PPC_INTERPRETER_H
#define PPC_INTERPRETER_H

#include "cpu.h"

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

typedef bool (*fptr)(State *state, Instruction ins);

#define AddTable(x)
#define AddInstrAlias(x,y,z)
#define AddInstr(id, name) bool name (State *state, Instruction instr);
#include "table19.inc"
#include "table31.inc"
#include "tablePrimary.inc"
#undef AddTable
#undef AddInstr
#undef AddInstrAlias

} // namespace Interpreter

} // namespace ppc

#endif // PPC_INTERPRETER_H
