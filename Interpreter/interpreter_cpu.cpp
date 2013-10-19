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
#define crn(n) bits::field<uint32_t>(state->reg.cr.value, n * 4, n)

/* Move Condition Register Field */
bool mcrf(State *state, Instruction instr)
{
   crn(instr.crfD) = crn(instr.crfS);
   return true;
}

/* Move from Condition Register */
bool mfcr(State *state, Instruction instr)
{
   gpr(instr.rD) = state->reg.cr.value;
   return true;
}

/* Move from One Condition Register Field */
bool mfocrf(State *state, Instruction instr)
{
   auto crN = -1;

   for (int i = 0; i < 8 && crN == 0; ++i) {
      if (instr.crm & (1 << i)) {
         crN = i;
      }
   }

   if (crN > 0 && crN < 8) {
      auto mask = 0xf << (crN * 4);
      gpr(instr.rD) &= ~mask;
      gpr(instr.rD) |= state->reg.cr.value & mask;
   }

   return true;
}

/* Move from Machine State Register */
bool mfmsr(State *state, Instruction instr)
{
   gpr(instr.rD) = state->reg.msr;
   return true;
}

/* Move from Special-Purpose Register */
bool mfspr(State *state, Instruction instr)
{
   auto spr = (instr.spr >> 5) & 0x1f | (instr.spr & 0x1f);
   gpr(instr.rD) = state->reg.getSpr(spr);
   return true;
}

UNIMPLEMENTED(mfsr);     /* Move from Segment Register */
UNIMPLEMENTED(mfsrin);   /* Move from Segment Register Indirect */

/* Move from Time Base */
bool mftb(State *state, Instruction instr)
{
   auto tbr = (instr.tbr >> 5) & 0x1f | (instr.tbr & 0x1f);
   
   switch (tbr) {
   case 268:
      gpr(instr.rD) = state->reg.tbl;
      break;
   case 269:
      gpr(instr.rD) = state->reg.tbu;
      break;
   }

   return true;
}

/* Move to Condition Register Field */
bool mtcrf(State *state, Instruction instr)
{
   uint32_t mask = 0;

   for (int i = 0; i < 8; ++i) {
      if (instr.crm & (1 << i)) {
         mask |= 0xf << i;
      }
   }

   state->reg.cr.value &= ~mask;
   state->reg.cr.value |= gpr(instr.rD) & mask;
   return true;
}

/* Move to Machine State Register */
bool mtmsr(State *state, Instruction instr)
{
   auto s = gpr(instr.rS);
   state->reg.msr = s;

   if (instr.l15 == 0) {
      state->reg.msr |= ((s >> 14) & 1) << 5;
      state->reg.msr |= ((s >> 14) & 1) << 4;
   }

   return true;
}

/* Move to Machine State Register Doubleword */
bool mtmsrd(State *state, Instruction instr)
{
   auto s = gpr(instr.rS);

   if (instr.l15 == 0) {
      state->reg.msr = s;
      state->reg.msr |= ((s >> 14) & 1) << 5;
      state->reg.msr |= ((s >> 14) & 1) << 4;
   } else {
      state->reg.msr &= ~0xfffe;
      state->reg.msr |= s & 0xfffe;
   }

   return true;
}

/* Move to One Condition Register Field */
bool mtocrf(State *state, Instruction instr)
{
   auto crN = -1;

   for (int i = 0; i < 8 && crN == 0; ++i) {
      if (instr.crm & (1 << i)) {
         crN = i;
      }
   }

   if (crN > 0 && crN < 8) {
      auto mask = 0xf << (crN * 4);
      state->reg.cr.value &= ~mask;
      state->reg.cr.value |= gpr(instr.rD) & mask;
   }

   return true;
}

/* Move to Special-Purpose Register */
bool mtspr(State *state, Instruction instr)
{
   auto spr = (instr.spr >> 5) & 0x1f | (instr.spr & 0x1f);
   state->reg.setSpr(spr, gpr(instr.rS));
   return true;
}

UNIMPLEMENTED(mtsr);     /* Move to Segment Register */
UNIMPLEMENTED(mtsrin);   /* Move to Segment Register Indirect */

UNIMPLEMENTED(rfid);     /* Return from Interrupt Doubleword */

UNIMPLEMENTED(slbia);    /* SLB Invalidate All */
UNIMPLEMENTED(slbie);    /* SLB Invalidate Entry */
UNIMPLEMENTED(slbimfee); /* SLB Move From Entry ESID */
UNIMPLEMENTED(slbimfev); /* SLB Move From Entry VSID */
UNIMPLEMENTED(slbimte);  /* SLB Move to Entry */

UNIMPLEMENTED(sync);     /* Synchronize */

UNIMPLEMENTED(td);       /* Trap Doubleword */
UNIMPLEMENTED(tdi);      /* Trap Doubleword Immediate */
UNIMPLEMENTED(tw);       /* Trap Word */
UNIMPLEMENTED(twi);      /* Trap Word Immediate */

UNIMPLEMENTED(tlbia);    /* Translation Lookaside Buffer Invalidate All */
UNIMPLEMENTED(tlbie);    /* Translation Lookaside Buffer Invalidate Entry */
UNIMPLEMENTED(tlbiel);   /* Translation Lookaside Buffer Invalidate Entry Local */
UNIMPLEMENTED(tlbsync);  /* Translation Lookaside Buffer Synchronize */

} // namespace Interpreter

} // namespace ppc
