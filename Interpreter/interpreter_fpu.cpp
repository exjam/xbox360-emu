#include "ppc/interpreter.h"
#include "ppc/instructions.h"

#include "util/log.h"
#include "util/bits.h"
#include "util/be/memory.h"

#include <limits>
#include <float.h>

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

static inline bool isNegativeInfinity(double value)
{
   double positiveInfinity = std::numeric_limits<double>::infinity();
   uint64_t bitsC = *reinterpret_cast<uint64_t*>(&positiveInfinity);
   uint64_t bitsV = *reinterpret_cast<uint64_t*>(&value);
   bitsC |=  1ull << 63;
   return bitsC == bitsV;
}

static inline bool isPositiveInfinity(double value)
{
   double positiveInfinity = std::numeric_limits<double>::infinity();
   uint64_t bitsC = *reinterpret_cast<uint64_t*>(&positiveInfinity);
   uint64_t bitsV = *reinterpret_cast<uint64_t*>(&value);
   return bitsC == bitsV;
}

static inline bool isNaN(double value)
{
   return _isnan(value) != 0;
}

static inline bool isSignallingNaN(double value)
{
   double sNaN = std::numeric_limits<double>::signaling_NaN();
   uint64_t bitsC = *reinterpret_cast<uint64_t*>(&sNaN);
   uint64_t bitsV = *reinterpret_cast<uint64_t*>(&value);
   return bitsC == bitsV;
}

static inline bool isQuietNaN(double value)
{
   double qNaN = std::numeric_limits<double>::quiet_NaN();
   uint64_t bitsC = *reinterpret_cast<uint64_t*>(&qNaN);
   uint64_t bitsV = *reinterpret_cast<uint64_t*>(&value);
   return bitsC == bitsV;
}

static inline void updateFpscr(State *state)
{
   /* TODO: */
   //uint32_t vxvc : 1;      /* FP Exception: Invalid Operation for Compare */
   //uint32_t vximz : 1;     /* FP Exception: Invalid Operation for [Inf * Zero] */
   //uint32_t vxzdz : 1;     /* FP Exception: Invalid Operation for [Zero / Zero] */
   //uint32_t vxidi : 1;     /* FP Exception: Invalid Operation for [Inf / Inf] */
   //uint32_t vxisi : 1;     /* FP Exception: Invalid Operation for [Inf - Inf] */
   //uint32_t vxsnan : 1;    /* FP Exception: Invalid Operation for SNaN */
   //uint32_t fr : 1;        /* FP State: Fraction Rounded */

   /* UNUSED: _SW_INVALID, _SW_DENORMAL */
   
   /* _SW_UNEMULATED _SW_SQRTNEG  _SW_STACKOVERFLOW _SW_STACKUNDERFLOW*/

   auto flags = _statusfp();
   Fpscr fpscr = state->reg.fpscr;
   fpscr.fi = (flags & _SW_INEXACT) != 0;
   fpscr.xx |= fpscr.fi;
   fpscr.ux |= (flags & _SW_UNDERFLOW) != 0;
   fpscr.ox |= (flags & _SW_OVERFLOW) != 0;
   fpscr.zx |= (flags & _SW_ZERODIVIDE) != 0;
   fpscr.vx = fpscr.vxsnan | fpscr.vxisi | fpscr.vxidi | fpscr.vxzdz | fpscr.vximz | fpscr.vxvc | fpscr.vxsqrt | fpscr.vxsoft | fpscr.vxcvi;
   fpscr.fex = (fpscr.vx & fpscr.ve) | (fpscr.ox & fpscr.oe) | (fpscr.ux & fpscr.ue) | (fpscr.zx & fpscr.ze) | (fpscr.xx & fpscr.xe);
   fpscr.fx |= fpscr.vx | fpscr.ox | fpscr.ux | fpscr.zx | fpscr.xx;
   state->reg.fpscr = fpscr;
}

static inline void updateFprfCompare(State *state, double value)
{
   auto flags = 0;

   if (value == 0.0 || value == -0.0) {
      flags |= ppc::Fpscr::Equal;
   } else if (value < 0.0) {
      flags |= ppc::Fpscr::Negative;
   } else if (value > 0.0) {
      flags |= ppc::Fpscr::Positive;
   } else {
      flags |= ppc::Fpscr::Unordered;
   }

   state->reg.fpscr.fprf = flags;
}

static inline void updateFprf(State *state, double value)
{
   auto fpcls = _fpclass(value);
   auto flags = 0;

   if (fpcls & _FPCLASS_QNAN) {
      /* 1  0  0  0  1   Quiet NaN */
      flags |= ppc::Fpscr::ClassDescriptor;
      flags |= ppc::Fpscr::Unordered;
   } else if (fpcls & _FPCLASS_NINF) {
      /* 0  1  0  0  1  -Infinity */
      flags |= ppc::Fpscr::Negative;
      flags |= ppc::Fpscr::Unordered;
   } else if (fpcls & _FPCLASS_NN) {
      /* 0  1  0  0  0  -Normalised Number */
      flags |= ppc::Fpscr::Negative;
   } else if (fpcls & _FPCLASS_ND) {
      /* 1  1  0  0  0  -Denormalised Number */
      flags |= ppc::Fpscr::ClassDescriptor;
      flags |= ppc::Fpscr::Negative;
   } else if (fpcls & _FPCLASS_NZ) {
      /* 1  0  0  1  0  -Zero */
      flags |= ppc::Fpscr::ClassDescriptor;
      flags |= ppc::Fpscr::Equal;
   } else if (fpcls & _FPCLASS_PZ) {
      /* 0  0  0  1  0  +Zero */
      flags |= ppc::Fpscr::Equal;
   } else if (fpcls & _FPCLASS_PD) {
      /* 1  0  1  0  0  +Denormalized Number */
      flags |= ppc::Fpscr::ClassDescriptor;
      flags |= ppc::Fpscr::Positive;
   } else if (fpcls & _FPCLASS_PN) {
      /* 0  0  1  0  0  +Normalized Number */
      flags |= ppc::Fpscr::Positive;
   } else if (fpcls & _FPCLASS_PINF) {
      /* 0  0  1  0  1  +Infinity */
      flags |= ppc::Fpscr::Positive;
      flags |= ppc::Fpscr::Unordered;
   }

   state->reg.fpscr.fprf = flags;
}

static inline void updateCr1(State *state, double value)
{
   state->reg.cr.cr1 = state->reg.fpscr.cr1;
}

/* fabs */
bool fabs(State *state, Instruction instr)
{
   /* NOTE: Does not alter fpscr */
   fpr(instr.frD) = ::fabs(fpr(instr.frB));

   if (instr.rc) {
      updateCr1(state, fpr(instr.frD));
   }

   return true;
}

/* fadd */
bool fadd(State *state, Instruction instr)
{
   double a = fpr(instr.frA);
   double b = fpr(instr.frB);

   state->reg.fpscr.vxsnan |= isSignallingNaN(a) || isSignallingNaN(b);
   state->reg.fpscr.vxisi |= isPositiveInfinity(a) && isNegativeInfinity(b);

   fpr(instr.frD) = a + b;
   updateFpscr(state);

   if (instr.rc) {
      updateCr1(state, fpr(instr.frD));
   }

   return true;
}

/* fadds */
UNIMPLEMENTED(fadds);

/* fcfid */
bool fcfid(State *state, Instruction instr)
{
   uint64_t value = *reinterpret_cast<uint64_t*>(&fpr(instr.frB));

   fpr(instr.frD) = static_cast<double>(value);
   updateFpscr(state);

   if (instr.rc) {
      updateCr1(state, fpr(instr.frD));
   }

   return true;
}

/* fcmpo, fcmpu */
bool fcmp(State *state, Instruction instr)
{
   double a = fpr(instr.frA);
   double b = fpr(instr.frB);
   auto flags = 0;

   if (isNaN(a) || isNaN(b)) {
      flags = ppc::Fpscr::Unordered;
   } else if (a < b) {
      flags = ppc::Fpscr::Negative;
   } else if (a > b) {
      flags = ppc::Fpscr::Positive;
   } else {
      flags = ppc::Fpscr::Equal;
   }

   state->reg.fpscr.fprf = flags;
   setCrN(state, instr.crfd, flags);
   
   if (isSignallingNaN(a) || isSignallingNaN(b)) {
      state->reg.fpscr.vxsnan = 1;

      if (instr.subop10 == ppc::op::fcmpo && !state->reg.fpscr.ve) {
         state->reg.fpscr.vxvc = 1;
      }

      state->reg.fpscr.fx = 1;
   } else if (instr.subop10 == ppc::op::fcmpo && (isQuietNaN(a) || isQuietNaN(b))) {
      state->reg.fpscr.vxvc = 1;
      state->reg.fpscr.fx = 1;
   }
   
   return true;
}

/* fcmpo */
bool fcmpo(State *state, Instruction instr) {
   return fcmp(state, instr);
}

/* fcmpu */
bool fcmpu(State *state, Instruction instr) {
   return fcmp(state, instr);
}

UNIMPLEMENTED(fctid);
UNIMPLEMENTED(fctidz);
UNIMPLEMENTED(fctiw);
UNIMPLEMENTED(fctiwz);
UNIMPLEMENTED(fdiv);
UNIMPLEMENTED(fdivs);
UNIMPLEMENTED(fmadd);
UNIMPLEMENTED(fmadds);
UNIMPLEMENTED(fmr);
UNIMPLEMENTED(fmsub);
UNIMPLEMENTED(fmsubs);
UNIMPLEMENTED(fmul);
UNIMPLEMENTED(fmuls);
UNIMPLEMENTED(fnabs);
UNIMPLEMENTED(fneg);
UNIMPLEMENTED(fnmadd);
UNIMPLEMENTED(fnmadds);
UNIMPLEMENTED(fnmsub);
UNIMPLEMENTED(fnmsubs);
UNIMPLEMENTED(fres);
UNIMPLEMENTED(frsp);
UNIMPLEMENTED(frsqrte);
UNIMPLEMENTED(fsel);
UNIMPLEMENTED(fsqrt);
UNIMPLEMENTED(fsqrts);
UNIMPLEMENTED(fsub);
UNIMPLEMENTED(fsubs);
UNIMPLEMENTED(icbi);
UNIMPLEMENTED(isync);

} // namespace Interpreter

} // namespace ppc
