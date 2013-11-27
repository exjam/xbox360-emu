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

/* Store x */
enum StoreFlags {
   StoreDefault     = 0,
   StoreIndexed     = 1 << 0,
   StoreUpdate      = 1 << 1,
   StoreDS          = 1 << 2,
   StoreDirect      = 1 << 3,
   StoreReversed    = 1 << 4,
   StoreConditional = 1 << 5,
};

template<typename DstType, int Flags>
bool stxx(State *state, Instruction instr)
{
   ppc::reg_t ea;
   DstType value;

   if (Flags & StoreDS) {
      ea = bits::signExtend<16>(static_cast<uint64_t>(instr.ds) << 2);
   } else if (Flags & StoreIndexed) {
      ea = gpr(instr.rB);
   } else {
      ea = bits::signExtend<16, uint64_t>(instr.d);
   }

   if (Flags & StoreUpdate) {
      ea += gpr(instr.rA);
   } else {
      ea += gpr0(instr.rA);
   }

   if (Flags & StoreConditional) {
      state->reg.cr.cr0 = 0;

      if (state->reg.reserve && state->reg.reserveAddress == ea) {
         state->reg.cr.cr0 = ppc::Cr::Zero;
         state->reg.reserve = 0;
      }

      if (state->reg.xer.so) {
         state->reg.cr.cr0 |= ppc::Cr::SummaryOverflow;
      }
      
      if ((state->reg.cr.cr0 & ppc::Cr::Zero) == 0) {
         /* Reservation does not exist */
         return true;
      }
   }

   if (std::numeric_limits<DstType>::is_integer) {
      value = reinterpret<DstType>(gpr(instr.rS));
   } else {
      value = static_cast<DstType>(fpr(instr.frS));
   }

   if (Flags & StoreReversed) {
      value = bits::swap(value);
   }

   be::Memory::write<DstType>(ea, value);

   if (Flags & StoreUpdate) {
      gpr(instr.rA) = ea;
   }

   return true;
}

/* Store Byte */
bool stb(State *state, Instruction instr)
{
   return stxx<uint8_t, StoreDefault>(state, instr);
}

/* Store Byte with Update */
bool stbu(State *state, Instruction instr)
{
   return stxx<uint8_t, StoreUpdate>(state, instr);
}

/* Store Byte with Update Indexed */
bool stbux(State *state, Instruction instr)
{
   return stxx<uint8_t, StoreUpdate | StoreIndexed>(state, instr);
}

/* Store Byte Indexed */
bool stbx(State *state, Instruction instr)
{
   return stxx<uint8_t, StoreIndexed>(state, instr);
}

/* Store Doubleword */
bool std(State *state, Instruction instr)
{
   return stxx<uint64_t, StoreDS>(state, instr);
}

/* Store Doubleword Conditional Indexed */
bool stdcx(State *state, Instruction instr)
{
   return stxx<uint64_t, StoreConditional | StoreIndexed>(state, instr);
}

/* Store Doubleword with Update */
bool stdu(State *state, Instruction instr)
{
   return stxx<uint64_t, StoreDS | StoreUpdate>(state, instr);
}

/* Store Doubleword with Update Indexed */
bool stdux(State *state, Instruction instr)
{
   return stxx<uint64_t, StoreIndexed | StoreUpdate>(state, instr);
}

/* Store Doubleword Indexed */
bool stdx(State *state, Instruction instr)
{
   return stxx<uint64_t, StoreIndexed>(state, instr);
}

/* Store Floating-Point Double */
bool stfd(State *state, Instruction instr)
{
   return stxx<double, StoreDefault>(state, instr);
}

/* Store Floating-Point Double with Update */
bool stfdu(State *state, Instruction instr)
{
   return stxx<double, StoreUpdate>(state, instr);
}

/* Store Floating-Point Double with Update Indexed */
bool stfdux(State *state, Instruction instr)
{
   return stxx<double, StoreUpdate | StoreIndexed>(state, instr);
}

/* Store Floating-Point Double Indexed */
bool stfdx(State *state, Instruction instr)
{
   return stxx<double, StoreIndexed>(state, instr);
}

/* Store Floating-Point as Integer Word Single */
bool stfiwx(State *state, Instruction instr)
{
   return stxx<float, StoreIndexed | StoreDirect>(state, instr);
}

/* Store Floating-Point Single */
bool stfs(State *state, Instruction instr)
{
   return stxx<float, StoreDefault>(state, instr);
}

/* Store Floating-Point Single with Update */
bool stfsu(State *state, Instruction instr)
{
   return stxx<float, StoreUpdate>(state, instr);
}

/* Store Floating-Point Single with Update Indexed */
bool stfsux(State *state, Instruction instr)
{
   return stxx<float, StoreUpdate | StoreIndexed>(state, instr);
}

/* Store Floating-Point Single Indexed */
bool stfsx(State *state, Instruction instr)
{
   return stxx<float, StoreIndexed>(state, instr);
}

/* Store Halfword */
bool sth(State *state, Instruction instr)
{
   return stxx<uint16_t, StoreDefault>(state, instr);
}

/* Store Halfword Byte-Reverse Indexed */
bool sthbrx(State *state, Instruction instr)
{
   return stxx<uint16_t, StoreReversed | StoreIndexed>(state, instr);
}

/* Store Halfword with Update */
bool sthu(State *state, Instruction instr)
{
   return stxx<uint16_t, StoreUpdate>(state, instr);
}

/* Store Halfword with Update Indexed */
bool sthux(State *state, Instruction instr)
{
   return stxx<uint16_t, StoreUpdate | StoreIndexed>(state, instr);
}

/* Store Halfword Indexed */
bool sthx(State *state, Instruction instr)
{
   return stxx<uint16_t, StoreIndexed>(state, instr);
}

/* Store Multiple Word */
bool stmw(State *state, Instruction instr)
{
   auto ea = gpr0(instr.rA) + bits::signExtend<16, uint64_t>(instr.d);
   auto r = instr.rS;

   while (r <= 31) {
      be::Memory::write(ea, gprw(r));
      r  += 1;
      ea += 4;
   }

   return true;
}

/* Store String Word x */
bool stsw(State *state, Instruction instr, uint64_t ea, uint64_t n)
{
   auto r = instr.rS - 1;
   auto i = 32;

   while (n > 0) {
      if (i == 32) {
         r = (r + 1) % 32;
      }

      be::Memory::write<uint8_t>(ea, (gpr(r) >> (24 - i)) & 0xFF);

      i += 8;

      if (i == 64) {
         i = 32;
      }

      ea++;
      n--;
   }

   return true;
}

/* Store String Word Immediate */
bool stswi(State *state, Instruction instr)
{
   return stsw(state,
               instr,
               gpr0(instr.rA),
               instr.nb == 0 ? 32 : instr.nb);
}

/* Store String Word Indexed */
bool stswx(State *state, Instruction instr)
{
   return stsw(state,
               instr,
               gpr0(instr.rA) + gpr(instr.rB),
               state->reg.xer.byteCount);
}

/* Store Word */
bool stw(State *state, Instruction instr)
{
   return stxx<uint32_t, StoreDefault>(state, instr);
}

/* Store Word Byte-Reverse Indexed */
bool stwbrx(State *state, Instruction instr)
{
   return stxx<uint32_t, StoreReversed | StoreIndexed>(state, instr);
}

/* Store Word Conditional Indexed */
bool stwcx(State *state, Instruction instr)
{
   return stxx<uint32_t, StoreConditional | StoreIndexed>(state, instr);
}

/* Store Word with Update */
bool stwu(State *state, Instruction instr)
{
   return stxx<uint32_t, StoreUpdate>(state, instr);
}

/* Store Word with Update Indexed */
bool stwux(State *state, Instruction instr)
{
   return stxx<uint32_t, StoreUpdate | StoreIndexed>(state, instr);
}

/* Store Word Indexed */
bool stwx(State *state, Instruction instr)
{
   return stxx<uint32_t, StoreIndexed>(state, instr);
}

} // namespace Interpreter

} // namespace ppc
