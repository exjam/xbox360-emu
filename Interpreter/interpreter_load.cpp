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
#define fpr(instr) state->reg.fpr[instr]

template<typename DstType, typename SrcType>
static inline DstType& reinterpret(SrcType& value)
{
   return *reinterpret_cast<DstType*>(&value);
}

enum LoadFlags {
   LoadDefault    = 0,
   LoadIndexed    = 1 << 0,
   LoadUpdate     = 1 << 1,
   LoadDS         = 1 << 2,
   LoadZeroExtend = 1 << 3,
   LoadSignExtend = 1 << 4,
   LoadFpu        = 1 << 5
};

/* Load x */
template<typename SrcType, int Flags>
bool lxx(State *state, Instruction instr)
{
   ppc::reg_t ea;

   if (Flags & LoadDS) {
      ea = bits::signExtend<16>(static_cast<uint64_t>(instr.ds) << 2);
   } else if (Flags & LoadIndexed) {
      ea = gpr(instr.rB);
   } else {
      ea = bits::signExtend<16>(instr.d);
   }

   if (Flags & LoadUpdate) {
      ea += gpr(instr.rA);
      gpr(instr.rA) = ea;
   } else {
      ea += gpr0(instr.rA);
   }

   if (Flags & LoadFpu) {
      fpr(instr.frD) = static_cast<ppc::freg_t>(be::Memory::read<SrcType>(ea));
   } else {
      if (Flags & LoadZeroExtend) {
         #pragma warning(suppress: 4244)
         gpr(instr.rD) = be::Memory::read<SrcType>(ea);
      } else if (Flags & LoadSignExtend) {
         #pragma warning(suppress: 4244)
         gpr(instr.rD) = bits::signExtend<sizeof(SrcType)* 8, ppc::reg_t>(be::Memory::read<SrcType>(ea));
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

UNIMPLEMENTED(ldarx);    /* Load Doubleword and Reserve Indexed */

/* Load Double with Update */
bool ldu(State *state, Instruction instr)
{
   return lxx<uint64_t, LoadDS | LoadUpdate>(state, instr);
}

/* Load Double with Update Indexed */
bool ldux(State *state, Instruction instr)
{
   return lxx<uint64_t, LoadDS | LoadUpdate | LoadIndexed>(state, instr);
}

/* Load Double Indexed */
bool ldx(State *state, Instruction instr)
{
   return lxx<uint64_t, LoadDS | LoadIndexed>(state, instr);
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

UNIMPLEMENTED(lhbrx);    /* Load Halfword Byte-Reverse indexed */

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

UNIMPLEMENTED(lmw);      /* Load Multiple Word */
UNIMPLEMENTED(lswi);     /* Load String Word Immediate */
UNIMPLEMENTED(lswx);     /* Load String Word Indexed */

/* Load Word Algebraic */
bool lwa(State *state, Instruction instr)
{
   return lxx<uint32_t, LoadSignExtend>(state, instr);
}

UNIMPLEMENTED(lwarx);    /* Load Word and Reserve Indexed */

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

UNIMPLEMENTED(lwbrx);    /* Load Word Byte-Reverse Indexed */

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
