#include "ppc/interpreter.h"
#include "ppc/instructions.h"

#include "util/log.h"
#include "util/bits.h"
#include "util/be/memory.h"

#undef max
#include <limits>

namespace ppc 
{

namespace Interpreter
{

#define sprSplit(instr) (((instr.spr & 0x1f) << 5) | ((instr.spr >> 5) & 0x1f))
#define gpr(instr) state->reg.gpr[instr]
#define gpr0(id) ((id == 0) ? 0 : state->reg.gpr[id])

static inline void updateCr0(State *state, uint64_t uValue)
{
   state->reg.cr.cr0 = 0;

   if (uValue == 0) {
      state->reg.cr.zero = 1;
   } else if (bits::beBit<64>(uValue, 63)) {
      state->reg.cr.negative = 1;
   } else {
      state->reg.cr.positive = 1;
   }

   state->reg.cr.summaryOverflow = state->reg.xer.so;
}

static inline void updateXerOverflow(State *state, bool overflow)
{
   state->reg.xer.ov = overflow;
   
   if (overflow == 1) {
      state->reg.xer.so = 1;
   }
}

/* add, addc, adde */
bool add(State *state, Instruction instr)
{
   uint64_t a = gpr(instr.rA);
   uint64_t b = gpr(instr.rB);
   uint64_t d = a + b;

   if (instr.subop9_oe == ppc::op::adde) {
      d += state->reg.xer.ca;
   }

   gpr(instr.rD) = d;

   if (instr.oe) {
      updateXerOverflow(state, bits::leBit<64>((a ^ d) & (b ^ d), 63) != 0);
   }

   if (instr.rc) {
      updateCr0(state, gpr(instr.rD));
   }

   if (instr.subop9_oe == ppc::op::addc || instr.subop9_oe == ppc::op::adde) {
      state->reg.xer.ca = gpr(instr.rD) < gpr(instr.rA);
   }

   return true;
}

/* addi, addis */
bool addi(State *state, Instruction instr)
{
   uint64_t a = gpr0(instr.rA);
   int64_t s = bits::signExtend<16>(instr.simm);

   if (instr.opcode == ppc::op::addis) {
      s <<= 16;
   }

   gpr(instr.rD) = a + s;
   return true;
}

/* addic, addic. */
bool addic(State *state, Instruction instr)
{
   gpr(instr.rD) = gpr(instr.rA) + bits::signExtend<16>(instr.simm);

   if (instr.opcode == ppc::op::addicr) {
      updateCr0(state, gpr(instr.rD));
   }

   state->reg.xer.ca = gpr(instr.rD) < gpr(instr.rA);
   return true;
}

/* addme */
bool addme(State *state, Instruction instr)
{
   uint64_t a = gpr(instr.rA);
   uint64_t b = state->reg.xer.ca - 1;
   uint64_t d = a + b;
   gpr(instr.rD) = d;

   if (instr.oe) {
      updateXerOverflow(state, bits::leBit<64>((a ^ d) & (b ^ d), 63) != 0);
   }

   if (instr.rc) {
      updateCr0(state, gpr(instr.rD));
   }

   state->reg.xer.ca = gpr(instr.rD) < gpr(instr.rA);
   return true;
}

/* addze */
bool addze(State *state, Instruction instr)
{
   uint64_t a = gpr(instr.rA);
   uint64_t b = state->reg.xer.ca;
   uint64_t d = a + b;
   gpr(instr.rD) = d;

   if (instr.oe) {
      updateXerOverflow(state, bits::leBit<64>((a ^ d) & (b ^ d), 63) != 0);
   }

   if (instr.rc) {
      updateCr0(state, gpr(instr.rD));
   }

   state->reg.xer.ca = gpr(instr.rD) < gpr(instr.rA);
   return true;
}

/* and */
bool and(State *state, Instruction instr)
{
   gpr(instr.rA) = gpr(instr.rS) & gpr(instr.rB);

   if (instr.rc) {
      updateCr0(state, gpr(instr.rA));
   }

   return true;
}

/* andc */
bool andc(State *state, Instruction instr)
{
   gpr(instr.rA) = gpr(instr.rS) & ~gpr(instr.rB);

   if (instr.rc) {
      updateCr0(state, gpr(instr.rA));
   }

   return true;
}

/* andi, andis */
bool andi(State *state, Instruction instr)
{
   uint64_t uimm = instr.uimm;
   
   if (instr.opcode == ppc::op::andis) {
      uimm <<= 16;
   }

   gpr(instr.rA) = gpr(instr.rS) & uimm;
   updateCr0(state, gpr(instr.rA));
   return true;
}

/* b */
bool b(State *state, Instruction instr)
{
   if (instr.aa) {
      state->nia = bits::signExtend<26>(instr.li << 2);
   } else {
      state->nia = state->cia + bits::signExtend<26>(instr.li << 2);
   }

   if (instr.lk) {
      state->reg.lr = state->cia + 4;
   }

   return true;
}

/* bc */
bool bc(State *state, Instruction instr)
{
   uint64_t ctr_ok = 1;
   uint64_t cond_ok = 1;

   if (bits::beBit<5>(instr.bo, 2) == 0) {
      state->reg.ctr -= 1;
      ctr_ok = (state->reg.ctr != 0) ^ bits::beBit<5>(instr.bo, 3);
   }

   if (bits::beBit<5>(instr.bo, 0) == 0) {
      cond_ok = bits::beBit<32>(state->reg.cr.value, instr.bi) == bits::beBit<5>(instr.bo, 1);
   }

   if (ctr_ok && cond_ok) {
      if (instr.aa) {
         state->nia = bits::signExtend<16>(instr.bd << 2);
      } else {
         state->nia = state->cia + bits::signExtend<16>(instr.bd << 2);
      }

      if (instr.lk) {
         state->reg.lr = state->cia + 4;
      }
   }

   return true;
}

/* bcctr */
bool bcctr(State *state, Instruction instr)
{
   uint64_t cond_ok = 1;
   
   if (bits::beBit<5>(instr.bo, 0) == 0) {
      cond_ok = bits::beBit<32>(state->reg.cr.value, instr.bi) == bits::beBit<5>(instr.bo, 1);
   }

   if (cond_ok) {
      state->nia = state->reg.ctr << 2;

      if (instr.lk) {
         state->reg.lr = state->cia + 4;
      }
   }

   return true;
}

/* bclr */
bool bclr(State *state, Instruction instr)
{
   uint64_t ctr_ok = 1;
   uint64_t cond_ok = 1;

   if (bits::beBit<5>(instr.bo, 2) == 0) {
      state->reg.ctr -= 1;
      ctr_ok = (state->reg.ctr != 0) ^ bits::beBit<5>(instr.bo, 3);
   }

   if (bits::beBit<5>(instr.bo, 0) == 0) {
      cond_ok = bits::beBit<32>(state->reg.cr.value, instr.bi) == bits::beBit<5>(instr.bo, 1);
   }
   
   if (ctr_ok && cond_ok) {
      state->nia = state->reg.lr & 0xFFFFFFFC;

      if (instr.lk) {
         state->reg.lr = state->cia + 4;
      }
   }

   return true;
}

/* cmp */

/* cmpi */

/* cmpl */

/* cmpli */

/* cntlzdx */

/* cntlzwx */

/* crand */

/* crandc */

/* creqv */

/* std, stdu */
bool std(State *state, Instruction instr)
{
   uint64_t ea = gpr0(instr.rA) + bits::signExtend<16>(instr.ds << 2);
   be::Memory::write<uint64_t>(ea, gpr(instr.rS));

   if (instr.rc) {
      gpr(instr.rA) = ea;
   }

   return true;
}

/* stw, stwu */
bool stw(State *state, Instruction instr)
{
   uint64_t ea = gpr0(instr.rA) + bits::signExtend<16>(instr.d);
   uint32_t value = gpr(instr.rS) >> 32;
   be::Memory::write<uint32_t>(ea, value);

   if (instr.opcode == ppc::op::stwu) {
      gpr(instr.rA) = ea;
   }

   return true;
}

/* mfspr */
bool mfspr(State *state, Instruction instr)
{
   gpr(instr.rD) = state->reg.getSpr(sprSplit(instr));
   return true;
}

} // namespace Interpreter

} // namespace ppc
