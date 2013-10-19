#include "ppc/interpreter.h"
#include "ppc/instructions.h"
#include "ppc/fpu.h"

#include "util/log.h"
#include "util/bits.h"
#include "util/be/memory.h"

#include <limits>
#include <float.h>
#include <cmath>

namespace ppc 
{

namespace Interpreter
{
   
#define UNIMPLEMENTED(insName) \
   bool insName (State *state, Instruction) { \
      xDebug() << "Unimplemented interpreter instruction "#insName ; \
      return false; \
   }

#define fpr(id) state->reg.fpr[id]
#define crn(n) bits::field<uint32_t>(state->reg.cr.value, n * 4, n)
#define fpscr(n) bits::field<uint32_t>(state->reg.fpscr.value, n * 4, n)

template<typename DstType, typename SrcType>
static inline DstType& reinterpret(SrcType& value)
{
   return *reinterpret_cast<DstType*>(&value);
}

static inline void updateVx(State *state)
{
   Fpscr fpscr = state->reg.fpscr;
   fpscr.vx = fpscr.vxsnan | fpscr.vxisi | fpscr.vxidi | fpscr.vxzdz | fpscr.vximz | fpscr.vxvc | fpscr.vxsqrt | fpscr.vxsoft | fpscr.vxcvi;
   state->reg.fpscr = fpscr;
}

static inline void updateFexVx(State *state)
{
   Fpscr fpscr = state->reg.fpscr;
   fpscr.vx = fpscr.vxsnan | fpscr.vxisi | fpscr.vxidi | fpscr.vxzdz | fpscr.vximz | fpscr.vxvc | fpscr.vxsqrt | fpscr.vxsoft | fpscr.vxcvi;
   fpscr.fex = (fpscr.vx & fpscr.ve) | (fpscr.ox & fpscr.oe) | (fpscr.ux & fpscr.ue) | (fpscr.zx & fpscr.ze) | (fpscr.xx & fpscr.xe);
   state->reg.fpscr = fpscr;
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
   fpscr.vxsqrt |= (flags & _SW_SQRTNEG) != 0;
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

static inline void updateCr1(State *state)
{
   state->reg.cr.cr1 = state->reg.fpscr.cr1;
}

/* Floating Absolute Value */
bool fabs(State *state, Instruction instr)
{
   fpr(instr.frD) = std::fabs(fpr(instr.frB));

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

/* Floating Add x */
template<typename Type>
bool faddx(State *state, Instruction instr)
{
   auto a = fpr(instr.frA);
   auto b = fpr(instr.frB);

   state->reg.fpscr.vxsnan |= fpu::isSignalingNaN(a) || fpu::isSignalingNaN(b);
   state->reg.fpscr.vxisi  |= fpu::isInfinity(a) && fpu::isInfinity(b);

   auto r = a + b;
   updateFpscr(state);
   updateFprf(state, r);
   fpr(instr.frD) = r;

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

/* Floating Add */
bool fadd(State *state, Instruction instr)
{
   return faddx<double>(state, instr);
}

/* Floating Add Single */
bool fadds(State *state, Instruction instr)
{
   return faddx<float>(state, instr);
}

/* Floating Convert from Integer Doubleword  */
bool fcfid(State *state, Instruction instr)
{
   fpu::Double dbl = fpr(instr.frB);

   fpr(instr.frD) = static_cast<double>(dbl.uv);
   updateFpscr(state);
   updateFprf(state, fpr(instr.frD));

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

/* Floating Compare x */
enum Order {
   Unordered = 0,
   Ordered = 1
};

template<int Order>
bool fcmp(State *state, Instruction instr)
{
   auto a = fpr(instr.frA);
   auto b = fpr(instr.frB);
   auto flags = 0;

   if (fpu::isNaN(a) || fpu::isNaN(b)) {
      flags = ppc::Fpscr::Unordered;
   } else if (a < b) {
      flags = ppc::Fpscr::Negative;
   } else if (a > b) {
      flags = ppc::Fpscr::Positive;
   } else {
      flags = ppc::Fpscr::Equal;
   }

   state->reg.fpscr.fprf = flags;
   crn(instr.crfD) = flags;
   
   if (fpu::isSignalingNaN(a) || fpu::isSignalingNaN(b)) {
      state->reg.fpscr.vxsnan = 1;

      if (Order == Ordered && !state->reg.fpscr.ve) {
         state->reg.fpscr.vxvc = 1;
      }

      state->reg.fpscr.fx = 1;
   } else if (Order == Ordered && (fpu::isQuietNaN(a) || fpu::isQuietNaN(b))) {
      state->reg.fpscr.vxvc = 1;
      state->reg.fpscr.fx = 1;
   }
   
   return true;
}

/* Floating Compare */
bool fcmpo(State *state, Instruction instr)
{
   return fcmp<Ordered>(state, instr);
}

/* Floating Compare Unordered */
bool fcmpu(State *state, Instruction instr)
{
   return fcmp<Unordered>(state, instr);
}

/* Floating Convert to Integer x */
enum FctiRounding {
   RoundDefault = 0,
   RoundToZero = 1
};

template<typename DstType, int Rounding>
bool fctixx(State *state, Instruction instr)
{
   DstType value;

   if (fpr(instr.frB) > std::numeric_limits<DstType>::max()) {
      value = std::numeric_limits<DstType>::max();
   } else if (fpr(instr.frB) < std::numeric_limits<DstType>::min()) {
      value = std::numeric_limits<DstType>::min();
   } else if (Rounding == RoundToZero) {
      value = static_cast<DstType>(std::trunc(fpr(instr.frB)));
   } else {
      /* TODO: use rounding mode in FPSCR[RN] */
      value = static_cast<DstType>(fpr(instr.frB));
   }

   updateFpscr(state);

   /* TODO: is this always done? */
   if (static_cast<double>(value) > instr.frB) {
      state->reg.fpscr.fr = 1;
   }

   reinterpret<DstType>(fpr(instr.frB)) = value;

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

/* Floating Convert to Integer Doubleword */
bool fctid(State *state, Instruction instr)
{
   return fctixx<int64_t, RoundDefault>(state, instr);
}

/* Floating Convert to Integer Doubleword with Round to Zero */
bool fctidz(State *state, Instruction instr)
{
   return fctixx<int64_t, RoundToZero>(state, instr);
}

/* Floating Convert to Integer Word */
bool fctiw(State *state, Instruction instr)
{
   return fctixx<int32_t, RoundDefault>(state, instr);
}

/* Floating Convert to Integer Word with Round to Zero */
bool fctiwz(State *state, Instruction instr)
{
   return fctixx<int32_t, RoundToZero>(state, instr);
}

/* Floating Divide x */
template<typename Type>
bool fdivx(State *state, Instruction instr)
{
   auto a = fpr(instr.frA);
   auto b = fpr(instr.frB);

   state->reg.fpscr.vxzdz  |= fpu::isZero(a) && fpu::isZero(b);
   state->reg.fpscr.vxsnan |= fpu::isSignalingNaN(a) || fpu::isSignalingNaN(b);
   state->reg.fpscr.vxidi  |= fpu::isInfinity(a) && fpu::isInfinity(b);

   auto r = a / b;
   updateFpscr(state);
   updateFprf(state, r);
   fpr(instr.frD) = r;

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

/* Floating Divide */
bool fdiv(State *state, Instruction instr)
{
   return fdivx<double>(state, instr);
}

/* Floating Divide Single */
bool fdivs(State *state, Instruction instr)
{
   return fdivx<float>(state, instr);
}

/* Floating Multiply-Add x */
template<typename Type>
bool fmaddx(State *state, Instruction instr)
{
   auto a = fpr(instr.frA);
   auto b = fpr(instr.frB);
   auto c = fpr(instr.frC);

   state->reg.fpscr.vximz  |= fpu::isInfinity(a) && fpu::isZero(c);
   state->reg.fpscr.vxsnan |= fpu::isSignalingNaN(a) || fpu::isSignalingNaN(b) || fpu::isSignalingNaN(c);
   state->reg.fpscr.vxisi  |= (fpu::isInfinity(a) || fpu::isInfinity(c)) && fpu::isInfinity(b);

   auto r = (a * c) + b;
   updateFpscr(state);
   updateFprf(state, r);
   fpr(instr.frD) = r;

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

/* Floating Multiply-Add */
bool fmadd(State *state, Instruction instr)
{
   return fmaddx<double>(state, instr);
}

/* Floating Multiply-Add Single */
bool fmadds(State *state, Instruction instr)
{
   return fmaddx<float>(state, instr);
}

/* Floating Move Register */
bool fmr(State *state, Instruction instr)
{
   fpr(instr.frD) = fpr(instr.frB);

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

/* Floating Multiply-Subtract x */
template<typename Type>
bool fmsubx(State *state, Instruction instr)
{
   auto a = fpr(instr.frA);
   auto b = fpr(instr.frB);
   auto c = fpr(instr.frC);

   state->reg.fpscr.vximz  |= fpu::isInfinity(a) && fpu::isZero(c);
   state->reg.fpscr.vxsnan |= fpu::isSignalingNaN(a) || fpu::isSignalingNaN(b) || fpu::isSignalingNaN(c);
   state->reg.fpscr.vxisi  |= (fpu::isInfinity(a) || fpu::isInfinity(c)) && fpu::isInfinity(b);

   auto r = (a * c) - b;
   updateFpscr(state);
   updateFprf(state, r);
   fpr(instr.frD) = r;

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

/* Floating Multiply-Subtract */
bool fmsub(State *state, Instruction instr)
{
   return fmsubx<double>(state, instr);
}

/* Floating Multiply-Subtract Single */
bool fmsubs(State *state, Instruction instr)
{
   return fmsubx<float>(state, instr);
}

/* Floating Multiply x */
template<typename Type>
bool fmulx(State *state, Instruction instr)
{
   auto a = fpr(instr.frA);
   auto c = fpr(instr.frC);

   state->reg.fpscr.vximz  |= fpu::isInfinity(a) && fpu::isZero(c);
   state->reg.fpscr.vxsnan |= fpu::isSignalingNaN(a) || fpu::isSignalingNaN(c);

   auto r = a * c;
   updateFpscr(state);
   updateFprf(state, r);
   fpr(instr.frD) = r;

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

/* Floating Multiply */
bool fmul(State *state, Instruction instr)
{
   return fmulx<double>(state, instr);
}

/* Floating Multiply Single */
bool fmuls(State *state, Instruction instr)
{
   return fmulx<float>(state, instr);
}

/* Floating Negative Absolute Value */
bool fnabs(State *state, Instruction instr)
{
   fpr(instr.frD) = fpu::makeNegative(std::fabs(fpr(instr.frB)));

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

/* Floating Negate */
bool fneg(State *state, Instruction instr)
{
   fpr(instr.frD) = fpu::makeNegative(fpr(instr.frB));

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

/* Floating Negative Multiply-Add x */
template<typename Type>
bool fnmaddx(State *state, Instruction instr)
{
   auto a = fpr(instr.frA);
   auto b = fpr(instr.frB);
   auto c = fpr(instr.frC);

   state->reg.fpscr.vximz  |= fpu::isInfinity(a) && fpu::isZero(c);
   state->reg.fpscr.vxsnan |= fpu::isSignalingNaN(a) || fpu::isSignalingNaN(b) || fpu::isSignalingNaN(c);
   state->reg.fpscr.vxisi  |= (fpu::isInfinity(a) || fpu::isInfinity(c)) && fpu::isInfinity(b);

   auto r = -((a * c) + b);
   updateFpscr(state);
   updateFprf(state, r);
   fpr(instr.frD) = r;

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}


/* Floating Negative Multiply-Add */
bool fnmadd(State *state, Instruction instr)
{
   return fnmaddx<double>(state, instr);
}

/* Floating Negative Multiply-Add Single */
bool fnmadds(State *state, Instruction instr)
{
   return fnmaddx<float>(state, instr);
}

/* Floating Negative Multiply-Subtract x */
template<typename Type>
bool fnmsubx(State *state, Instruction instr)
{
   auto a = fpr(instr.frA);
   auto b = fpr(instr.frB);
   auto c = fpr(instr.frC);

   state->reg.fpscr.vximz  |= fpu::isInfinity(a) && fpu::isZero(c);
   state->reg.fpscr.vxsnan |= fpu::isSignalingNaN(a) || fpu::isSignalingNaN(b) || fpu::isSignalingNaN(c);
   state->reg.fpscr.vxisi  |= (fpu::isInfinity(a) || fpu::isInfinity(c)) && fpu::isInfinity(b);

   auto r = -((a * c) - b);
   updateFpscr(state);
   updateFprf(state, r);
   fpr(instr.frD) = r;

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

/* Floating Negative Multiply-Subtract */
bool fnmsub(State *state, Instruction instr)
{
   return fnmsubx<double>(state, instr);
}

/* Floating Negative Multiply-Subtract Single */
bool fnmsubs(State *state, Instruction instr)
{
   return fnmsubx<float>(state, instr);
}

/* Floating Reciprocal Estimate Single */
bool fres(State *state, Instruction instr)
{
   auto b = fpr(instr.frB);

   state->reg.fpscr.vxsnan |= fpu::isSignalingNaN(b);

   auto r = 1.0f / b;
   updateFpscr(state);
   updateFprf(state, r);
   fpr(instr.frD) = r;

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

/* Floating Round to Single */
bool frsp(State *state, Instruction instr)
{
   auto b = fpr(instr.frB);

   state->reg.fpscr.vxsnan |= fpu::isSignalingNaN(b);

   /* TODO: use rounding mode in FPSCR[RN] */
   auto r = static_cast<double>(static_cast<float>(b));
   updateFpscr(state);
   updateFprf(state, r);
   fpr(instr.frD) = r;

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

/* Floating Reciprocal Square Root Estimate */
bool frsqrte(State *state, Instruction instr)
{
   double b = fpr(instr.frB);
   double r;

   state->reg.fpscr.vxsnan |= fpu::isSignalingNaN(b);
   state->reg.fpscr.vxsqrt |= fpu::isNegativeInfinity(b) | fpu::isSignalingNaN(b) | fpu::isNegative(b);

   if (fpu::isNegativeZero(b)) {
      r = fpu::negativeInfinity();
   } if (fpu::isPositiveZero(b)) {
      r = fpu::positiveInfinity();
   } else if (fpu::isPositiveInfinity(b)) {
      r = fpu::positiveZero();
   } else if (fpu::isNegativeInfinity(b) || fpu::isSignalingNaN(b) || fpu::isQuietNaN(b) || fpu::isNegative(b)) {
      r = fpu::quietNaN();
   } else {
      /* TODO: frsqrte as estimate, not exact */
      r = 1.0 / std::sqrt(b);
   }

   updateFpscr(state);
   updateFprf(state, r);
   fpr(instr.frD) = r;

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

/* Floating Select */
bool fsel(State *state, Instruction instr)
{
   fpr(instr.frD) = fpr(instr.frA) > 0.0 ? fpr(instr.frC) : fpr(instr.frB);

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

/* Floating Square Root x */
template<typename Type>
bool fsqrtx(State *state, Instruction instr)
{
   double b = fpr(instr.frB);
   double r;

   state->reg.fpscr.vxsnan |= fpu::isSignalingNaN(b);
   state->reg.fpscr.vxsqrt |= fpu::isNegativeInfinity(b) | fpu::isSignalingNaN(b) | fpu::isNegative(b);

   if (fpu::isNegativeZero(b)) {
      r = fpu::negativeZero();
   } else if (fpu::isPositiveInfinity(b)) {
      r = fpu::positiveInfinity();
   } else if (fpu::isNegativeInfinity(b) || fpu::isSignalingNaN(b) || fpu::isQuietNaN(b) || fpu::isNegative(b)) {
      r = fpu::quietNaN();
   } else {
      r = std::sqrt(b);
   }

   updateFpscr(state);
   updateFprf(state, fpr(instr.frD));
   fpr(instr.frD) = r;

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

/* Floating Square Root */
bool fsqrt(State *state, Instruction instr)
{
   return fsqrtx<double>(state, instr);
}

/* Floating Square Root Single */
bool fsqrts(State *state, Instruction instr)
{
   return fsqrtx<float>(state, instr);
}

/* Floating Subtracet x */
template<typename Type>
bool fsubx(State *state, Instruction instr)
{
   auto a = fpr(instr.frA);
   auto b = fpr(instr.frB);

   state->reg.fpscr.vxsnan |= fpu::isSignalingNaN(a) || fpu::isSignalingNaN(b);
   state->reg.fpscr.vxisi  |= fpu::isInfinity(a) && fpu::isInfinity(b);

   auto r = a - b;
   updateFpscr(state);
   updateFprf(state, fpr(instr.frD));
   fpr(instr.frD) = r;

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

/* Floating Subtracet */
bool fsub(State *state, Instruction instr)
{
   return fsubx<double>(state, instr);
}

/* Floating Subtracet Single */
bool fsubs(State *state, Instruction instr)
{
   return fsubx<float>(state, instr);
}

/* Move to Condition Register from FPSCR */
bool mcrfs(State *state, Instruction instr)
{
   crn(instr.crfD) = fpscr(instr.crfS);

   switch (instr.crfS) {
   case 0:
      state->reg.fpscr.fx = 0;
      state->reg.fpscr.ox = 0;
      break;
   case 1:
      state->reg.fpscr.ux = 0;
      state->reg.fpscr.zx = 0;
      state->reg.fpscr.xx = 0;
      state->reg.fpscr.vxsnan = 0;
      break;
   case 2:
      state->reg.fpscr.vxisi = 0;
      state->reg.fpscr.vxidi = 0;
      state->reg.fpscr.vxzdz = 0;
      state->reg.fpscr.vximz = 0;
      break;
   case 3:
      state->reg.fpscr.vxvc = 0;
      break;
   case 5:
      state->reg.fpscr.vxsoft = 0;
      state->reg.fpscr.vxsqrt = 0;
      state->reg.fpscr.vxcvi = 0;
      break;
   }

   return true;
}

/* Move from FPSCR */
bool mffs(State *state, Instruction instr)
{
   reinterpret<uint64_t>(fpr(instr.frB)) = state->reg.fpscr.value;

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

/* Move to FPSCR Bit 0 */
bool mtfsb0(State *state, Instruction instr)
{
   state->reg.fpscr.value = bits::clear(state->reg.fpscr.value, instr.crbD);

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

/* Move to FPSCR Bit 1 */
bool mtfsb1(State *state, Instruction instr)
{
   state->reg.fpscr.value = bits::set(state->reg.fpscr.value, instr.crbD);

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

/* Move to FPSCR Fields */
bool mtfsf(State *state, Instruction instr)
{
   uint32_t mask = 0;

   for (int i = 0; i < 8; ++i) {
      if (instr.fm & (1 << i)) {
         if (i == 0) {
            mask |= 0x9;
         } else {
            mask |= 0xf << i;
         }
      }
   }

   state->reg.fpscr.value &= ~mask;
   state->reg.fpscr.value |= reinterpret<uint32_t>(fpr(instr.frB)) & mask;

   if (instr.fm & 1) {
      updateFexVx(state);
   } else {
      updateVx(state);
   }

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

/* Move to FPSCR Field Immediate */
bool mtfsfi(State *state, Instruction instr)
{

   if (instr.crfD == 0) {
      fpscr(instr.crfD) = instr.imm & 0x9;
   } else {
      fpscr(instr.crfD) = instr.imm;
   }

   updateFexVx(state);

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

} // namespace Interpreter

} // namespace ppc
