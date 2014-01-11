#include "ppc/interpreter.h"
#include "common.h"

#include <ppc/instructions.h>

#include <util/log.h>
#include <util/bits.h>
#include <util/memory.h>

#include <limits>

namespace ppc 
{

namespace Interpreter
{
   
/* Load x */
enum LoadFlags {
   LoadDefault    = 0,
   LoadIndexed    = 1 << 0,
   LoadUpdate     = 1 << 1,
   LoadDS         = 1 << 2,
   LoadZeroExtend = 1 << 3,
   LoadSignExtend = 1 << 4,
   LoadFpu        = 1 << 5,
   LoadReversed   = 1 << 6,
};

template<typename SrcType, int Flags>
bool lxx(State *state, Instruction instr)
{
   ppc::reg_t ea;
   SrcType value;

   if (Flags & LoadIndexed) {
      ea = gpr(instr.rB);
   } else if (Flags & LoadDS) {
      ea = bits::signExtend<16>(static_cast<uint64_t>(instr.ds) << 2);
   } else {
      ea = bits::signExtend<16, uint64_t>(instr.d);
   }

   if (Flags & LoadUpdate) {
      ea += gpr(instr.rA);
      gpr(instr.rA) = ea;
   } else {
      ea += gpr0(instr.rA);
   }

   #pragma warning(suppress: 4244)
   value = Memory::read<SrcType>(ea);

   if (Flags & LoadReversed) {
      value = bits::swap(value);
   }

   if (Flags & LoadFpu) {
      fpr(instr.frD) = static_cast<ppc::freg_t>(value);
   } else {
      if (Flags & LoadSignExtend) {
         gpr(instr.rD) = bits::signExtend<sizeof(SrcType)* 8, ppc::reg_t>(value);
      } else {
         gpr(instr.rD) = value;
      }
   }

   return true;
}

/* Load Byte Zero Extend */
bool lbz(State *state, Instruction instr)
{
   return lxx<uint8_t, LoadZeroExtend>(state, instr);
}

/* Load Byte Zero Extend with Update */
bool lbzu(State *state, Instruction instr)
{
   return lxx<uint8_t, LoadZeroExtend | LoadUpdate>(state, instr);
}

/* Load Byte Zero Extend with Update Indexed */
bool lbzux(State *state, Instruction instr)
{
   return lxx<uint8_t, LoadZeroExtend | LoadUpdate | LoadIndexed>(state, instr);
}

/* Load Byte Zero Extend Indexed */
bool lbzx(State *state, Instruction instr)
{
   return lxx<uint8_t, LoadZeroExtend | LoadIndexed>(state, instr);
}

/* Load Double */
bool ld(State *state, Instruction instr)
{
   return lxx<uint64_t, LoadDS>(state, instr);
}

/* Load Doubleword and Reserve Indexed */
bool ldarx(State *state, Instruction instr)
{
   auto ea = gpr0(instr.rA) + gpr(instr.rB);
   state->reg.reserve = 1;
   state->reg.reserveAddress = ea;

   if (ea & 0x7) {
      return raise(state, ppc::Exceptions::Alignment);
   }

   return lxx<uint64_t, LoadZeroExtend | LoadIndexed>(state, instr);
}

/* Load Double with Update */
bool ldu(State *state, Instruction instr)
{
   return lxx<uint64_t, LoadDS | LoadUpdate>(state, instr);
}

/* Load Double with Update Indexed */
bool ldux(State *state, Instruction instr)
{
   return lxx<uint64_t, LoadUpdate | LoadIndexed>(state, instr);
}

/* Load Double Indexed */
bool ldx(State *state, Instruction instr)
{
   return lxx<uint64_t, LoadIndexed>(state, instr);
}

/* Load Floating-Point Double */
bool lfd(State *state, Instruction instr)
{
   return lxx<double, LoadFpu>(state, instr);
}

/* Load Floating-Point Double with Update */
bool lfdu(State *state, Instruction instr)
{
   return lxx<double, LoadFpu | LoadUpdate>(state, instr);
}

/* Load Floating-Point Double with Update Indexed */
bool lfdux(State *state, Instruction instr)
{
   return lxx<double, LoadFpu | LoadUpdate | LoadIndexed>(state, instr);
}

/* Load Floating-Point Double Indexed */
bool lfdx(State *state, Instruction instr)
{
   return lxx<double, LoadFpu | LoadIndexed>(state, instr);
}

/* Load Floating-Point Single */
bool lfs(State *state, Instruction instr)
{
   return lxx<float, LoadFpu>(state, instr);
}

/* Load Floating-Point Single with Update */
bool lfsu(State *state, Instruction instr)
{
   return lxx<float, LoadFpu | LoadUpdate>(state, instr);
}

/* Load Floating-Point Single with Update Indexed */
bool lfsux(State *state, Instruction instr)
{
   return lxx<float, LoadFpu | LoadUpdate | LoadIndexed>(state, instr);
}

/* Load Floating-Point Single Indexed */
bool lfsx(State *state, Instruction instr)
{
   return lxx<float, LoadFpu | LoadIndexed>(state, instr);
}

/* Load Halfword Algebraic */
bool lha(State *state, Instruction instr)
{
   return lxx<uint16_t, LoadSignExtend>(state, instr);
}

/* Load Halfword Algebraic with Update */
bool lhau(State *state, Instruction instr)
{
   return lxx<uint16_t, LoadSignExtend | LoadUpdate>(state, instr);
}

/* Load Halfword Algebraic with Update Indexed */
bool lhaux(State *state, Instruction instr)
{
   return lxx<uint16_t, LoadSignExtend | LoadUpdate | LoadIndexed>(state, instr);
}

/* Load Halfword Algebraic Indexed */
bool lhax(State *state, Instruction instr)
{
   return lxx<uint16_t, LoadSignExtend | LoadIndexed>(state, instr);
}

/* Load Halfword Byte-Reverse indexed */
bool lhbrx(State *state, Instruction instr)
{
   return lxx<uint16_t, LoadZeroExtend | LoadReversed | LoadIndexed>(state, instr);
}

/* Load Halfword Zero Extend */
bool lhz(State *state, Instruction instr)
{
   return lxx<uint16_t, LoadZeroExtend>(state, instr);
}

/* Load Halfword Zero Extend with Update */
bool lhzu(State *state, Instruction instr)
{
   return lxx<uint16_t, LoadZeroExtend | LoadUpdate>(state, instr);
}

/* Load Halfword Zero Extend with Update Indexed */
bool lhzux(State *state, Instruction instr)
{
   return lxx<uint16_t, LoadZeroExtend | LoadUpdate | LoadIndexed>(state, instr);
}

/* Load Halfword Zero Extend Indexed */
bool lhzx(State *state, Instruction instr)
{
   return lxx<uint16_t, LoadZeroExtend | LoadIndexed>(state, instr);
}

/* Load Multiple Word */
bool lmw(State *state, Instruction instr)
{
   auto ea = gpr0(instr.rA) + bits::signExtend<16, uint64_t>(instr.d);

   if (ea & 0x3) {
      raise(state, ppc::Exceptions::Alignment);
      return true;
   }

   for (auto i = instr.rD; i <= 31; ++i) {
      gpr(i) = Memory::read<uint32_t>(ea);
      ea += 4;
   }

   return true;
}

/* Load String Word x */
bool lsw(State *state, Instruction instr, uint64_t ea, uint64_t n)
{
   auto r = instr.rD - 1;
   auto i = 32;

   while (n > 0) {
      if (i == 32) {
         r = (r + 1) % 32;
         gpr(r) = 0;
      }

      gpr(r) |= Memory::read<uint8_t>(ea) << (24 - i);
      i += 8;

      if (i == 64) {
         i = 32;
      }

      ea++;
      n--;
   }

   return true;
}

/* Load String Word Immediate */
bool lswi(State *state, Instruction instr)
{
   return lsw(state,
              instr,
              gpr0(instr.rA),
              instr.nb == 0 ? 32 : instr.nb);
}

/* Load String Word Indexed */
bool lswx(State *state, Instruction instr)
{
   return lsw(state,
              instr,
              gpr0(instr.rA) + gpr(instr.rB),
              state->reg.xer.byteCount);
}

/* Load Word Algebraic */
bool lwa(State *state, Instruction instr)
{
   return lxx<uint32_t, LoadSignExtend>(state, instr);
}

/* Load Word and Reserve Indexed */
bool lwarx(State *state, Instruction instr)
{
   auto ea = gpr0(instr.rA) + gpr(instr.rB);
   state->reg.reserve = 1;
   state->reg.reserveAddress = ea;

   return lxx<uint32_t, LoadZeroExtend | LoadIndexed>(state, instr);
}

/* Load Word Algebraic with Update */
bool lwau(State *state, Instruction instr)
{
   return lxx<uint32_t, LoadSignExtend | LoadUpdate>(state, instr);
}

/* Load Word Algebraic with Update Indexed */
bool lwaux(State *state, Instruction instr)
{
   return lxx<uint32_t, LoadSignExtend | LoadUpdate | LoadIndexed>(state, instr);
}

/* Load Word Algebraic Indexed */
bool lwax(State *state, Instruction instr)
{
   return lxx<uint32_t, LoadSignExtend | LoadIndexed>(state, instr);
}

/* Load Word Byte-Reverse Indexed */
bool lwbrx(State *state, Instruction instr)
{
   return lxx<uint32_t, LoadZeroExtend | LoadReversed | LoadIndexed>(state, instr);
}

/* Load Word Zero Extend */
bool lwz(State *state, Instruction instr)
{
   return lxx<uint32_t, LoadZeroExtend>(state, instr);
}

/* Load Word Zero Extend with Update */
bool lwzu(State *state, Instruction instr)
{
   return lxx<uint32_t, LoadZeroExtend | LoadUpdate>(state, instr);
}

/* Load Word Zero Extend with Update Indexed */
bool lwzux(State *state, Instruction instr)
{
   return lxx<uint32_t, LoadZeroExtend | LoadUpdate | LoadIndexed>(state, instr);
}

/* Load Word Zero Extend Indexed */
bool lwzx(State *state, Instruction instr)
{
   return lxx<uint32_t, LoadZeroExtend | LoadIndexed>(state, instr);
}

} // namespace Interpreter

} // namespace ppc
