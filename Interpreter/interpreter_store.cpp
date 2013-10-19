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

/* Store x */
enum StoreFlags {
   StoreDefault = 0,
   StoreIndexed = 1 << 0,
   StoreUpdate  = 1 << 1,
   StoreDS      = 1 << 2,
   StoreDirect  = 1 << 3,
};

template<typename DstType, int Flags>
bool stxx(State *state, Instruction instr)
{
   ppc::reg_t ea;

   if (Flags & StoreDS) {
      ea = bits::signExtend<16>(static_cast<uint64_t>(instr.ds) << 2);
   } else if (Flags & StoreIndexed) {
      ea = gpr(instr.rB);
   } else {
      ea = bits::signExtend<16>(instr.d);
   }

   if (Flags & StoreUpdate) {
      ea += gpr(instr.rA);
      gpr(instr.rA) = ea;
   } else {
      ea += gpr0(instr.rA);
   }

   if (std::numeric_limits<DstType>::is_integer) {
      be::Memory::write<DstType>(ea, reinterpret<DstType>(gpr(instr.rS)));
   } else {
      be::Memory::write<DstType>(ea, static_cast<DstType>(fpr(instr.frS)));
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

/* Store Double */
bool std(State *state, Instruction instr)
{
   return stxx<uint64_t, StoreDS>(state, instr);
}

UNIMPLEMENTED(stdcx);    /* Store Doubleword Conditional Indexed */

/* Store Double with Update */
bool stdu(State *state, Instruction instr)
{
   return stxx<uint64_t, StoreDS | StoreUpdate>(state, instr);
}

/* Store Double with Update Indexed */
bool stdux(State *state, Instruction instr)
{
   return stxx<uint64_t, StoreIndexed | StoreUpdate>(state, instr);
}

/* Store Double Indexed */
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
   /* TODO: stfiwx Maybe do a convert as we always use doubles in fpu */
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

UNIMPLEMENTED(sthbrx);   /* Store Halfword Byte-Reverse Indexed */

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

UNIMPLEMENTED(stmw);     /* Store Multiple Word */
UNIMPLEMENTED(stswi);    /* Store String Word Immediate */
UNIMPLEMENTED(stswx);    /* Store String Word Indexed */

/* Store Word */
bool stw(State *state, Instruction instr)
{
   return stxx<uint32_t, StoreDefault>(state, instr);
}

UNIMPLEMENTED(stwbrx);   /* Store Word Byte-Reverse Indexed */
UNIMPLEMENTED(stwcx);    /* Store Word Conditional Indexed */

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
