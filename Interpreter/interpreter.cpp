#include "ppc/interpreter.h"
#include "ppc/instructions.h"

#include "util/log.h"
#include "util/bits.h"
#include "util/be/memory.h"

#include <limits>

namespace ppc 
{

namespace Interpreter
{

#define UNIMPLEMENTED(insName) \
   bool insName (State *state, Instruction) { \
      xDebug() << "Unimplemented interpreter instruction "#insName ; \
      return false; \
   }

#define gpr(instr) state->reg.gpr[instr]
#define gpr0(id) ((id == 0) ? 0 : state->reg.gpr[id])

UNIMPLEMENTED(dcbf);     /* Data Cache Block Flush */
UNIMPLEMENTED(dcbst);    /* Data Cache Block Store */
UNIMPLEMENTED(dcbt);     /* Data Cache Block Touch */
UNIMPLEMENTED(dcbtst);   /* Data Cache Block Touch for Store */

/* Data Cache Block Zero */
bool dcbz(State *state, Instruction instr)
{
   auto ea = gpr0(instr.rA) + gpr(instr.rB);
   auto n = 32; /* TODO: dcbz block size */
   be::Memory::zero(ea & ~(n -1), n);
   return false;
}

/* External Control In Word Indexed */
bool eciwx(State *state, Instruction instr)
{
   auto ea = gpr0(instr.rA) + gpr(instr.rB);

   /* TODO: Load from physical address ea of device EAR[RID] */

   if ((state->reg.ear & 0x80000000) == 0) {
      /* TODO: Raise ppc::Exceptions::DSI */
   }

   if (ea & 0x3) {
      /* TODO: Raise ppc::Exceptions::Alignment */
   }

   return false;
}

/* External Control Out Word Indexed */
bool ecowx(State *state, Instruction instr)
{
   auto ea = gpr0(instr.rA) + gpr(instr.rB);

   /* TODO: Store to physical address ea of device EAR[RID] */

   if ((state->reg.ear & 0x80000000) == 0) {
      /* TODO: Raise ppc::Exceptions::DSI */
   }

   if (ea & 0x3) {
      /* TODO: Raise ppc::Exceptions::Alignment */
   }

   return false;
}

/* Enforice In-Order Execution of I/O */
bool eieio(State *state, Instruction instr)
{
   _mm_sfence();
   return true;
}

UNIMPLEMENTED(icbi);     /* Instruction Cache Block Invalidate */
UNIMPLEMENTED(isync);    /* Instruction Synchronize */

#include "emugen_instr_table.cpp"

} // namespace Interpreter

} // namespace ppc
