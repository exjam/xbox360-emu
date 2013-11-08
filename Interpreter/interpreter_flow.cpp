#include "ppc/interpreter.h"
#include "ppc/instructions.h"

#include "util/log.h"
#include "util/bits.h"
#include "util/be/memory.h"

#include "common.h"

#include <limits>

namespace ppc 
{

namespace Interpreter
{

/* Branch */
bool b(State *state, Instruction instr)
{
   if (instr.aa) {
      state->nia = bits::signExtend<26>(static_cast<uint64_t>(instr.li) << 2);
   } else {
	  state->nia = state->cia + bits::signExtend<26>(static_cast<uint64_t>(instr.li) << 2);
   }

   if (instr.lk) {
      state->reg.lr = state->cia + 4;
   }

   return true;
}

/* Branch Conditional x */
enum BO
{
   CtrValue    = 1 << 1,
   NoCheckCtr  = 1 << 2,
   CondValue   = 1 << 3,
   NoCheckCond = 1 << 4
};

enum BcxFlags
{
   BcxDefault    =      0,
   BcxNoCheckCtr = 1 << 0,
   BcxBranchCtr  = 1 << 1,
   BcxBranchLr   = 1 << 2,
};

template<int Flags>
bool bcx(State *state, Instruction instr)
{
   bool ctr_ok = true;
   bool cond_ok = true;

   if ((Flags & BcxNoCheckCtr) == 0 && (instr.bo & NoCheckCtr) == 0) {
      state->reg.ctr -= 1;
      ctr_ok = ((state->reg.ctr != 0) ^ (instr.bo & CtrValue)) != 0;
   }

   if ((instr.bo & NoCheckCond) == 0) {
      if (instr.bo & CondValue) {
         cond_ok = (state->reg.cr.value & (1 << instr.bi)) != 0;
      } else {
         cond_ok = (state->reg.cr.value & (1 << instr.bi)) == 0;
      }
   }

   if (ctr_ok && cond_ok) {
      if (Flags & BcxBranchCtr) {
         state->nia = state->reg.ctr << 2;
      } else if (Flags & BcxBranchLr) {
         state->nia = state->reg.lr & 0xFFFFFFFC;
      } else {
         if (instr.aa) {
            state->nia = bits::signExtend<16>(static_cast<uint64_t>(instr.bd) << 2);
         } else {
            state->nia = state->cia + bits::signExtend<16>(static_cast<uint64_t>(instr.bd) << 2);
         }
      }

      if (instr.lk) {
         state->reg.lr = state->cia + 4;
      }
   }

   return true;
}

/* Branch Conditional */
bool bc(State *state, Instruction instr)
{
   return bcx<BcxDefault>(state, instr);
}

/* Branch Conditional to Count Register */
bool bcctr(State *state, Instruction instr)
{
   return bcx<BcxBranchCtr | BcxNoCheckCtr>(state, instr);
}

/* Branch Conditional to Link Register */
bool bclr(State *state, Instruction instr)
{
   return bcx<BcxBranchLr>(state, instr);
}

/* Compare x */
enum CmpxFlags
{
   CmpxDefault    =      0,
   CmpxImmediate  = 1 << 0
};

template<typename Type, int Flags>
bool cmpx(State *state, Instruction instr)
{
   auto flags = 0;
   Type a, b;

   if (instr.l == 0) {
      if (std::numeric_limits<Type>::is_signed) {
         a = bits::signExtend<32>(gpr(instr.rA));
      } else {
         a = bits::zeroExtend<32>(gpr(instr.rA));
      }
   } else {
      a = static_cast<Type>(gpr(instr.rA));
   }

   if (Flags & CmpxImmediate) {
      if (std::numeric_limits<Type>::is_signed) {
         b = bits::signExtend<16>(instr.simm);
      } else {
         b = instr.uimm;
      }
   } else if (instr.l == 0) {
      if (std::numeric_limits<Type>::is_signed) {
         b = bits::signExtend<32>(gpr(instr.rB));
      } else {
         b = bits::zeroExtend<32>(gpr(instr.rB));
      }
   } else {
      b = static_cast<Type>(gpr(instr.rB));
   }

   if (a < b) {
      flags |= ppc::Cr::Negative;
   } else if (a > b) {
      flags |= ppc::Cr::Positive;
   } else {
      flags |= ppc::Cr::Zero;
   }

   flags |= state->reg.xer.so;
   crf(instr.crfD) = flags;

   return true;
}

/* Compare */
bool cmp(State *state, Instruction instr)
{
   return cmpx<ppc::sreg_t, CmpxDefault>(state, instr);
}

/* Compare Immediate */
bool cmpi(State *state, Instruction instr)
{
   return cmpx<ppc::sreg_t, CmpxImmediate>(state, instr);
}

/* Compare Logical */
bool cmpl(State *state, Instruction instr)
{
   return cmpx<ppc::ureg_t, CmpxDefault>(state, instr);
}

/* Compare Logical Immediate */
bool cmpli(State *state, Instruction instr)
{
   return cmpx<ppc::ureg_t, CmpxImmediate>(state, instr);
}

/* Condition Register AND */
bool crand(State *state, Instruction instr)
{
   auto a = crb(instr.crbA);
   auto b = crb(instr.crbB);
   crb(instr.crbD) = a & b;
   return true;
}

/* Condition Register AND with Complement */
bool crandc(State *state, Instruction instr)
{
   auto a = crb(instr.crbA);
   auto b = crb(instr.crbB);
   crb(instr.crbD) = a & ~b;
   return true;
}

/* Condition Register Equivalent */
bool creqv(State *state, Instruction instr)
{
   auto a = crb(instr.crbA);
   auto b = crb(instr.crbB);
   crb(instr.crbD) = ~(a ^ b);
   return true;
}

/* Condition Register NAND */
bool crnand(State *state, Instruction instr)
{
   auto a = crb(instr.crbA);
   auto b = crb(instr.crbB);
   crb(instr.crbD) = ~(a & b);
   return true;
}

/* Condition Register NOR */
bool crnor(State *state, Instruction instr)
{
   auto a = crb(instr.crbA);
   auto b = crb(instr.crbB);
   crb(instr.crbD) = ~(a | b);
   return true;
}

/* Condition Register OR */
bool cror(State *state, Instruction instr)
{
   auto a = crb(instr.crbA);
   auto b = crb(instr.crbB);
   crb(instr.crbD) = a | b;
   return true;
}

/* Condition Register OR with Complement */
bool crorc(State *state, Instruction instr)
{
   auto a = crb(instr.crbA);
   auto b = crb(instr.crbB);
   crb(instr.crbD) = a | ~b;
   return true;
}

/* Condition Register XOR */
bool crxor(State *state, Instruction instr)
{
   auto a = crb(instr.crbA);
   auto b = crb(instr.crbB);
   crb(instr.crbD) = a ^ b;
   return true;
}

} // namespace Interpreter

} // namespace ppc
