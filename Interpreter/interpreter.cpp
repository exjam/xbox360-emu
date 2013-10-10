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

#define sprSplit(instr) (((instr.spr & 0x1f) << 5) | ((instr.spr >> 5) & 0x1f))
#define gpr(instr) state->reg.gpr[instr]
#define gpr0(id) ((id == 0) ? 0 : state->reg.gpr[id])
#define fpr(instr) state->reg.fpr[instr]

static inline void updateCr0(State *state, uint64_t value)
{
   auto flags = 0;

   if (value == 0) {
      flags |= ppc::Cr::Zero;
   } else if (bits::leBit<64>(value, 63)) {
      flags |= ppc::Cr::Negative;
   } else {
      flags |= ppc::Cr::Positive;
   }

   state->reg.cr.cr0 = flags;
}

static inline void updateCr1(State *state, double value)
{
   state->reg.cr.cr1 = state->reg.fpscr.cr1;
}

static inline void updateXerOverflow(State *state, bool overflow)
{
   state->reg.xer.ov = overflow;
   
   if (overflow) {
      state->reg.xer.so = 1;
   }
}

static inline void setCrN(State *state, uint32_t crN, uint32_t flags)
{
   crN = (7 - crN) * 4;
   state->reg.cr.value &= ~(0xF << crN);
   state->reg.cr.value |= (flags << crN);
}

static inline uint32_t getCrN(State *state, uint32_t crN) 
{
   crN = (7 - crN) * 4;
   return (state->reg.cr.value >> crN) & 0xF;
}

/* add, addc, adde */
bool add(State *state, Instruction instr)
{
   auto a = gpr(instr.rA);
   auto b = gpr(instr.rB);
   auto d = a + b;

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

/* addc */
bool addc(State *state, Instruction instr)
{
   return add(state, instr);
}

/* adde */
bool adde(State *state, Instruction instr)
{
   return add(state, instr);
}

/* addi, addis */
bool addi(State *state, Instruction instr)
{
   auto a = gpr0(instr.rA);
   auto s = bits::signExtend<16>(instr.simm);

   if (instr.opcode == ppc::op::addis) {
      s <<= 16;
   }

   gpr(instr.rD) = a + s;
   return true;
}

/* addis */
bool addis(State *state, Instruction instr)
{
   return addi(state, instr);
}

/* addic, addic. */
bool addic(State *state, Instruction instr)
{
   gpr(instr.rD) = gpr(instr.rA) + bits::signExtend<16>(instr.simm);

   if (instr.opcode == ppc::op::addico) {
      updateCr0(state, gpr(instr.rD));
   }

   state->reg.xer.ca = gpr(instr.rD) < gpr(instr.rA);
   return true;
}

/* addic. */
bool addico(State *state, Instruction instr)
{
   return addic(state, instr);
}

/* addme */
bool addme(State *state, Instruction instr)
{
   auto a = gpr(instr.rA);
   auto b = state->reg.xer.ca - 1;
   auto d = a + b;

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
   auto a = gpr(instr.rA);
   auto b = state->reg.xer.ca;
   auto d = a + b;

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

/* andi., andis. */
bool andio(State *state, Instruction instr)
{
   ppc::reg_t uimm = instr.uimm;
   
   if (instr.opcode == ppc::op::andiso) {
      uimm <<= 16;
   }

   gpr(instr.rA) = gpr(instr.rS) & uimm;
   updateCr0(state, gpr(instr.rA));
   return true;
}

/* andis. */
bool andiso(State *state, Instruction instr)
{
   return andio(state, instr);
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
   bool ctr_ok = true;
   bool cond_ok = true;

   if (bits::beBit<5>(instr.bo, 2) == 0) {
      state->reg.ctr -= 1;
      ctr_ok = ((state->reg.ctr != 0) ^ bits::beBit<5>(instr.bo, 3)) != 0;
   }

   if (bits::beBit<5>(instr.bo, 0) == 0) {
      cond_ok = (bits::beBit<32>(state->reg.cr.value, instr.bi) == bits::beBit<5>(instr.bo, 1));
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
   bool cond_ok = true;
   
   if (bits::beBit<5>(instr.bo, 0) == 0) {
      cond_ok = (bits::beBit<32>(state->reg.cr.value, instr.bi) == bits::beBit<5>(instr.bo, 1));
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
   bool ctr_ok = true;
   bool cond_ok = true;

   if (bits::beBit<5>(instr.bo, 2) == 0) {
      state->reg.ctr -= 1;
      ctr_ok = ((state->reg.ctr != 0) ^ bits::beBit<5>(instr.bo, 3)) != 0;
   }

   if (bits::beBit<5>(instr.bo, 0) == 0) {
      cond_ok = (bits::beBit<32>(state->reg.cr.value, instr.bi) == bits::beBit<5>(instr.bo, 1));
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
bool cmp(State *state, Instruction instr)
{
   ppc::sreg_t a = gpr(instr.rA);
   ppc::sreg_t b = gpr(instr.rB);
   auto flags = 0;

   if (instr.l == 0) {
      a = bits::signExtend<32>(a);
      b = bits::signExtend<32>(a);
   }

   if (a < b) {
      flags |= ppc::Cr::Negative;
   } else if (a > b) {
      flags |= ppc::Cr::Positive;
   } else {
      flags |= ppc::Cr::Zero;
   }

   flags |= state->reg.xer.so;
   setCrN(state, instr.crfd, flags);

   return true;
}

/* cmpi */
bool cmpi(State *state, Instruction instr)
{
   ppc::sreg_t a = gpr(instr.rA);
   ppc::sreg_t b = bits::signExtend<16>(instr.simm);
   auto flags = 0;

   if (instr.l == 0) {
      a = bits::signExtend<32>(a);
   }

   if (a < b) {
      flags |= ppc::Cr::Negative;
   } else if (a > b) {
      flags |= ppc::Cr::Positive;
   } else {
      flags |= ppc::Cr::Zero;
   }

   flags |= state->reg.xer.so;
   setCrN(state, instr.crfd, flags);

   return true;
}

/* cmpl */
bool cmpl(State *state, Instruction instr)
{
   ppc::ureg_t a = gpr(instr.rA);
   ppc::ureg_t b = gpr(instr.rB);
   auto flags = 0;

   if (instr.l == 0) {
      a = bits::zeroExtend<32>(a);
      b = bits::zeroExtend<32>(a);
   }

   if (a < b) {
      flags |= ppc::Cr::Negative;
   } else if (a > b) {
      flags |= ppc::Cr::Positive;
   } else {
      flags |= ppc::Cr::Zero;
   }

   flags |= state->reg.xer.so;
   setCrN(state, instr.crfd, flags);

   return true;
}

/* cmpli */
bool cmpli(State *state, Instruction instr)
{
   ppc::ureg_t a = gpr(instr.rA);
   ppc::ureg_t b = instr.uimm;
   auto flags = 0;

   if (instr.l == 0) {
      a = bits::zeroExtend<32>(a);
   }

   if (a < b) {
      flags |= ppc::Cr::Negative;
   } else if (a > b) {
      flags |= ppc::Cr::Positive;
   } else {
      flags |= ppc::Cr::Zero;
   }

   flags |= state->reg.xer.so;
   setCrN(state, instr.crfd, flags);

   return true;
}

/* cntlzd, cntlzd. */
bool cntlzd(State *state, Instruction instr)
{
   auto a = gpr(instr.rS);
   gpr(instr.rA) = bits::beScanForward(a);

   if (instr.rc) {
      updateCr0(state, gpr(instr.rA));
   }

   return true;
}

/* cntlzw, cntlzw. */
bool cntlzw(State *state, Instruction instr)
{
   uint32_t a = gpr(instr.rS) >> 32;
   gpr(instr.rA) = bits::beScanForward(a);

   if (instr.rc) {
      updateCr0(state, gpr(instr.rA));
   }

   return true;
}

/* crand */
bool crand(State *state, Instruction instr)
{
   auto a = getCrN(state, instr.crbA);
   auto b = getCrN(state, instr.crbB);
   setCrN(state, instr.crbD, a & b);
   return true;
}

/* crandc */
bool crandc(State *state, Instruction instr)
{
   auto a = getCrN(state, instr.crbA);
   auto b = getCrN(state, instr.crbB);
   setCrN(state, instr.crbD, a & ~b);
   return true;
}

/* creqv */
bool creqv(State *state, Instruction instr)
{
   auto a = getCrN(state, instr.crbA);
   auto b = getCrN(state, instr.crbB);
   setCrN(state, instr.crbD, ~(a ^ b));
   return true;
}

/* crnand */
bool crnand(State *state, Instruction instr)
{
   auto a = getCrN(state, instr.crbA);
   auto b = getCrN(state, instr.crbB);
   setCrN(state, instr.crbD, ~(a & b));
   return true;
}

/* crnor */
bool crnor(State *state, Instruction instr)
{
   auto a = getCrN(state, instr.crbA);
   auto b = getCrN(state, instr.crbB);
   setCrN(state, instr.crbD, ~(a | b));
   return true;
}

/* cror */
bool cror(State *state, Instruction instr)
{
   auto a = getCrN(state, instr.crbA);
   auto b = getCrN(state, instr.crbB);
   setCrN(state, instr.crbD, a | b);
   return true;
}

/* crorc */
bool crorc(State *state, Instruction instr)
{
   auto a = getCrN(state, instr.crbA);
   auto b = getCrN(state, instr.crbB);
   setCrN(state, instr.crbD, a | ~b);
   return true;
}

/* crxor */
bool crxor(State *state, Instruction instr)
{
   auto a = getCrN(state, instr.crbA);
   auto b = getCrN(state, instr.crbB);
   setCrN(state, instr.crbD, a ^ b);
   return true;
}

/* dcbf */
bool dcbf(State *state, Instruction instr)
{
   /* TODO: dcbf */
   return false;
}

/* dcbst */
bool dcbst(State *state, Instruction instr)
{
   /* TODO: dcbst */
   return false;
}

/* dcbt */
bool dcbt(State *state, Instruction instr)
{
   /* TODO: dcbt == _mm_prefetch */
   return false;
}

/* dcbtst */
bool dcbtst(State *state, Instruction instr)
{
   /* TODO: dcbtst */
   return false;
}

/* dcbz */
bool dcbz(State *state, Instruction instr)
{
   auto ea = gpr0(instr.rA) + gpr(instr.rB);
   auto n = 32; /* TODO: dcbz block size */
   be::Memory::zero(ea & ~(n -1), n);
   return false;
}

/* divd, divd., divdo, divdo. */
bool divd(State *state, Instruction instr)
{
   auto a = gpr(instr.rA);
   auto b = gpr(instr.rB);

   if (b != 0) {
      auto d = a / b;
      gpr(instr.rD) = d;
   }

   if (instr.oe) {
      updateXerOverflow(state, (b == 0) || ((a == (1ull << 63)) && (b == -1)));
   }

   if (instr.rc) {
      updateCr0(state, gpr(instr.rD));
   }

   return true;
}

/* divdu, divdu., divduo, divduo. */
bool divdu(State *state, Instruction instr)
{
   auto a = gpr(instr.rA);
   auto b = gpr(instr.rB);

   if (b != 0) {
      auto d = a / b;
      gpr(instr.rD) = d;
   }

   if (instr.oe) {
      updateXerOverflow(state, (b == 0));
   }

   if (instr.rc) {
      updateCr0(state, gpr(instr.rD));
   }

   return true;
}

/* divw, divw., divwo, divwo. */
bool divw(State *state, Instruction instr)
{
   uint32_t a = static_cast<uint32_t>(gpr(instr.rA));
   uint32_t b = static_cast<uint32_t>(gpr(instr.rB));

   if (b != 0) {
      auto d = a / b;
      gpr(instr.rD) = d;
   }

   if (instr.oe) {
      updateXerOverflow(state, (b == 0) || ((a == (1 << 31)) && (b == -1)));
   }

   if (instr.rc) {
      updateCr0(state, gpr(instr.rD));
   }

   return true;
}

/* divwu, divwu., divwuo, divwuo. */
bool divwu(State *state, Instruction instr)
{
   uint32_t a = static_cast<uint32_t>(gpr(instr.rA));
   uint32_t b = static_cast<uint32_t>(gpr(instr.rB));

   if (b != 0) {
      auto d = a / b;
      gpr(instr.rD) = d;
   }

   if (instr.oe) {
      updateXerOverflow(state, (b == 0));
   }

   if (instr.rc) {
      updateCr0(state, gpr(instr.rD));
   }

   return true;
}

/* eciwx */
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

/* ecowx */
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

/* eieio */
bool eieio(State *state, Instruction instr)
{
   /* TODO: _mm_sfence(); */
   return false;
}

/* eqv */
bool eqv(State *state, Instruction instr)
{
   gpr(instr.rA) = ~(gpr(instr.rS) ^ gpr(instr.rB));

   if (instr.rc) {
      updateCr0(state, gpr(instr.rA));
   }

   return true;
}

/* extsb */
bool extsb(State *state, Instruction instr)
{
   gpr(instr.rA) = bits::signExtend<8>(gpr(instr.rS));

   if (instr.rc) {
      updateCr0(state, gpr(instr.rA));
   }

   return true;
}

/* extsh */
bool extsh(State *state, Instruction instr)
{
   gpr(instr.rA) = bits::signExtend<16>(gpr(instr.rS));

   if (instr.rc) {
      updateCr0(state, gpr(instr.rA));
   }

   return true;
}

/* extsw */
bool extsw(State *state, Instruction instr)
{
   gpr(instr.rA) = bits::signExtend<32>(gpr(instr.rS));

   if (instr.rc) {
      updateCr0(state, gpr(instr.rA));
   }

   return true;
}

/* stb */
bool stb(State *state, Instruction instr)
{
   auto ea = gpr0(instr.rA) + bits::signExtend<16>(instr.d);
   be::Memory::write<uint8_t>(ea, static_cast<uint8_t>(gpr(instr.rS)));
   return true;
}

/* stbu */
bool stbu(State *state, Instruction instr)
{
   auto ea = gpr0(instr.rA) + bits::signExtend<16>(instr.d);
   be::Memory::write<uint8_t>(ea, static_cast<uint8_t>(gpr(instr.rS)));
   gpr(instr.rA) = ea;
   return true;
}

/* std */
bool std(State *state, Instruction instr)
{
   auto ea = gpr0(instr.rA) + bits::signExtend<16>(instr.ds << 2);
   be::Memory::write<uint64_t>(ea, gpr(instr.rS));
   return true;
}

/* stdu */
bool stdu(State *state, Instruction instr)
{
   auto ea = gpr0(instr.rA) + bits::signExtend<16>(instr.ds << 2);
   be::Memory::write<uint64_t>(ea, gpr(instr.rS));
   gpr(instr.rA) = ea;
   return true;
}

/* sth */
bool sth(State *state, Instruction instr)
{
   auto ea = gpr0(instr.rA) + bits::signExtend<16>(instr.d);
   be::Memory::write<uint16_t>(ea, static_cast<uint16_t>(gpr(instr.rS)));
   return true;
}

/* sthu */
bool sthu(State *state, Instruction instr)
{
   auto ea = gpr0(instr.rA) + bits::signExtend<16>(instr.d);
   be::Memory::write<uint16_t>(ea, static_cast<uint16_t>(gpr(instr.rS)));
   gpr(instr.rA) = ea;
   return true;
}

/* stw */
bool stw(State *state, Instruction instr)
{
   auto ea = gpr0(instr.rA) + bits::signExtend<16>(instr.d);
   be::Memory::write<uint32_t>(ea, static_cast<uint32_t>(gpr(instr.rS)));
   return true;
}

/* stwu */
bool stwu(State *state, Instruction instr)
{
   auto ea = gpr0(instr.rA) + bits::signExtend<16>(instr.d);
   be::Memory::write<uint32_t>(ea, static_cast<uint32_t>(gpr(instr.rS)));
   gpr(instr.rA) = ea;
   return true;
}

/* mfspr */
bool mfspr(State *state, Instruction instr)
{
   gpr(instr.rD) = state->reg.getSpr(sprSplit(instr));
   return true;
}

#include "emugen_instr_table.cpp"

} // namespace Interpreter

} // namespace ppc
