#include "ppc/interpreter.h"
#include "ppc/instructions.h"

#include "util/log.h"
#include "util/bits.h"
#include "util/be/memory.h"

#include <limits>
#include <float.h>
#include <cmath>

/* Why doesnt VS2012 have this function??? */
double trunc(double value)
{
   return (value < 0) ? ceil(value) : floor(value);
}

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
#define gpr(id) state->reg.gpr[id]
#define gpr0(id) ((id == 0) ? 0 : state->reg.gpr[id])
#define fpr(id) state->reg.fpr[id]

template<typename T>
static inline T &getFpr(State *state, int id) { ASSERT(0); return T(); }

template<>
static inline double &getFpr(State *state, int id)
{
   return state->reg.fpr[id];
}

template<>
static inline float &getFpr(State *state, int id)
{
   return *reinterpret_cast<float*>(&state->reg.fpr[id]);
}

template<typename T>
static inline void setFpr(State *state, int id, T value) { ASSERT(0); return T(); }

template<>
static inline void setFpr(State *state, int id, double value)
{
   state->reg.fpr[id] = value;
}

template<>
static inline void setFpr(State *state, int id, float value)
{
   *reinterpret_cast<float*>(&state->reg.fpr[id]) = value;
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

static inline bool isNegativeInfinity(float value)
{
   float negativeInfinity = std::numeric_limits<float>::infinity();
   uint32_t bitsC = *reinterpret_cast<uint32_t*>(&negativeInfinity);
   uint32_t bitsV = *reinterpret_cast<uint32_t*>(&value);
   bitsC |=  1u << 31;
   return bitsC == bitsV;
}

static inline bool isNegativeInfinity(double value)
{
   double negativeInfinity = std::numeric_limits<double>::infinity();
   uint64_t bitsC = *reinterpret_cast<uint64_t*>(&negativeInfinity);
   uint64_t bitsV = *reinterpret_cast<uint64_t*>(&value);
   bitsC |=  1ull << 63;
   return bitsC == bitsV;
}

static inline bool isPositiveInfinity(float value)
{
   float positiveInfinity = std::numeric_limits<float>::infinity();
   uint32_t bitsC = *reinterpret_cast<uint32_t*>(&positiveInfinity);
   uint32_t bitsV = *reinterpret_cast<uint32_t*>(&value);
   return bitsC == bitsV;
}

static inline bool isPositiveInfinity(double value)
{
   double positiveInfinity = std::numeric_limits<double>::infinity();
   uint64_t bitsC = *reinterpret_cast<uint64_t*>(&positiveInfinity);
   uint64_t bitsV = *reinterpret_cast<uint64_t*>(&value);
   return bitsC == bitsV;
}

template<typename T>
static inline T getNegativeInfinity() { ASSERT(0); return T(); }

template<>
static inline double getNegativeInfinity()
{
   double negativeInfinity = std::numeric_limits<double>::infinity();
   *reinterpret_cast<uint64_t*>(&negativeInfinity) |= 1ull << 63;
   return negativeInfinity;
}

template<typename T>
static inline T getPositiveInfinity()
{
   return std::numeric_limits<T>::infinity();
}

template<typename T>
static inline T getNegativeZero() { ASSERT(0); return T(); }

template<>
static inline double getNegativeZero()
{
   double negativeZero = 0.0;
   *reinterpret_cast<uint64_t*>(&negativeZero) |= 1ull << 63;
   return negativeZero;
}

template<>
static inline float getNegativeZero()
{
   float negativeZero = 0.0f;
   *reinterpret_cast<uint32_t*>(&negativeZero) |= 1 << 31;
   return negativeZero;
}

template<typename T>
static inline T getQuietNaN()
{
   return std::numeric_limits<T>::quiet_NaN();
}

template<typename T>
static inline T getSignalingNaN()
{
   return std::numeric_limits<T>::signaling_NaN();
}

static inline bool isZero(float value)
{
   return std::abs(value) == 0.0f;
}

static inline bool isZero(double value)
{
   return std::abs(value) == 0.0;
}

static inline bool isNegative(float value)
{
   uint32_t bitsV = *reinterpret_cast<uint32_t*>(&value);
   return (bitsV >> 31) != 0;
}

static inline bool isNegative(double value)
{
   uint64_t bitsV = *reinterpret_cast<uint64_t*>(&value);
   return (bitsV >> 63) != 0;
}

static inline bool isNegativeZero(float value)
{
   return isZero(value) && isNegative(value);
}

static inline bool isNegativeZero(double value)
{
   return isZero(value) && isNegative(value);
}

static inline bool isPositiveZero(float value)
{
   return isZero(value) && !isNegative(value);
}

static inline bool isPositiveZero(double value)
{
   return isZero(value) && !isNegative(value);
}

static inline bool isInfinity(float value)
{
   float infinity = std::numeric_limits<float>::infinity();
   uint32_t bitsC = *reinterpret_cast<uint32_t*>(&infinity);
   uint32_t bitsV = *reinterpret_cast<uint32_t*>(&value);
   bitsC |=  1u << 31;
   bitsV |=  1u << 31;
   return bitsC == bitsV;
}

static inline bool isInfinity(double value)
{
   double infinity = std::numeric_limits<double>::infinity();
   uint64_t bitsC = *reinterpret_cast<uint64_t*>(&infinity);
   uint64_t bitsV = *reinterpret_cast<uint64_t*>(&value);
   bitsC |=  1ull << 63;
   bitsV |=  1ull << 63;
   return bitsC == bitsV;
}

static inline bool isSignallingNaN(float value)
{
   float sNaN = std::numeric_limits<float>::signaling_NaN();
   uint32_t bitsC = *reinterpret_cast<uint32_t*>(&sNaN);
   uint32_t bitsV = *reinterpret_cast<uint32_t*>(&value);
   return bitsC == bitsV;
}

static inline bool isSignallingNaN(double value)
{
   double sNaN = std::numeric_limits<double>::signaling_NaN();
   uint64_t bitsC = *reinterpret_cast<uint64_t*>(&sNaN);
   uint64_t bitsV = *reinterpret_cast<uint64_t*>(&value);
   return bitsC == bitsV;
}

static inline bool isQuietNaN(float value)
{
   float qNaN = std::numeric_limits<float>::quiet_NaN();
   uint32_t bitsC = *reinterpret_cast<uint32_t*>(&qNaN);
   uint32_t bitsV = *reinterpret_cast<uint32_t*>(&value);
   return bitsC == bitsV;
}

static inline bool isQuietNaN(double value)
{
   double qNaN = std::numeric_limits<double>::quiet_NaN();
   uint64_t bitsC = *reinterpret_cast<uint64_t*>(&qNaN);
   uint64_t bitsV = *reinterpret_cast<uint64_t*>(&value);
   return bitsC == bitsV;
}

static inline bool isNaN(float value)
{
   return isSignallingNaN(value) || isQuietNaN(value);
}

static inline bool isNaN(double value)
{
   return _isnan(value) != 0;
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

/* fabs */
bool fabs(State *state, Instruction instr)
{
   fpr(instr.frD) = std::fabs(fpr(instr.frB));

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

/* fadd, fadd., fadds, fadds. */
template<typename Type>
bool faddx(State *state, Instruction instr)
{
   Type a = getFpr<Type>(state, instr.frA);
   Type b = getFpr<Type>(state, instr.frB);
   Type r;

   state->reg.fpscr.vxsnan |= isSignallingNaN(a) || isSignallingNaN(b);
   state->reg.fpscr.vxisi  |= isInfinity(a) && isInfinity(b);

   r = a + b;
   updateFpscr(state);
   updateFprf(state, fpr(instr.frD));
   setFpr<Type>(state, instr.frD, r);

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

/* fadd, fadd. */
bool fadd(State *state, Instruction instr)
{
   return faddx<double>(state, instr);
}

/* fadds, fadds. */
bool fadds(State *state, Instruction instr)
{
   return faddx<float>(state, instr);
}

/* fcfid */
bool fcfid(State *state, Instruction instr)
{
   uint64_t value = *reinterpret_cast<uint64_t*>(&fpr(instr.frB));

   fpr(instr.frD) = static_cast<double>(value);
   updateFpscr(state);
   updateFprf(state, fpr(instr.frD));

   if (instr.rc) {
      updateCr1(state);
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
bool fcmpo(State *state, Instruction instr)
{
   return fcmp(state, instr);
}

/* fcmpu */
bool fcmpu(State *state, Instruction instr)
{
   return fcmp(state, instr);
}

/* fctid, fctid., fctidz, fctidz. */
bool fctidx(State *state, Instruction instr)
{
   uint64_t value;

   if (fpr(instr.frB) > static_cast<double>((1ull << 63) - 1ull)) {
      value = (1ull << 63) - 1;
   } else if (fpr(instr.frB) < static_cast<double>(-(1ll << 63))) {
      value = 1ull << 63;
   } else if (instr.subop10 == ppc::op::fctid) {
      /* TODO: fctid use rounding mode in FPSCR[RN] */
      value = static_cast<uint64_t>(fpr(instr.frB));
   } else if (instr.subop10 == ppc::op::fctidz) {
      value = static_cast<uint64_t>(trunc(fpr(instr.frB)));
   }

   updateFpscr(state);

   if (static_cast<double>(value) > instr.frB) {
      state->reg.fpscr.fr = 1;
   }

   *reinterpret_cast<uint64_t*>(&fpr(instr.frD)) = value;

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

/* fctid, fctid. */
bool fctid(State *state, Instruction instr)
{
   return fctidx(state, instr);
}

/* fctidz, fctidz. */
bool fctidz(State *state, Instruction instr)
{
   return fctidx(state, instr);
}

/* fctiw, fctiw., fctiwz, fctiwz. */
bool fctiwx(State *state, Instruction instr)
{
   uint32_t value;

   if (fpr(instr.frB) > static_cast<double>((1u << 31) - 1u)) {
      value = (1u << 31) - 1;
   } else if (fpr(instr.frB) < static_cast<double>(-(1 << 31))) {
      value = 1u << 31;
   } else if (instr.subop10 == ppc::op::fctid) {
      /* TODO: fctid use rounding mode in FPSCR[RN] */
      value = static_cast<uint32_t>(fpr(instr.frB));
   } else if (instr.subop10 == ppc::op::fctidz) {
      value = static_cast<uint32_t>(trunc(fpr(instr.frB)));
   }

   updateFpscr(state);

   if (static_cast<double>(value) > instr.frB) {
      state->reg.fpscr.fr = 1;
   }

   *reinterpret_cast<uint32_t*>(&fpr(instr.frD)) = value;

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

/* fctiw, fctiw. */
bool fctiw(State *state, Instruction instr)
{
   return fctiwx(state, instr);
}

/* fctiwz, fctiwz. */
bool fctiwz(State *state, Instruction instr)
{
   return fctiwx(state, instr);
}

/* fdiv, fdiv., fdivs, fdivs. */
template<typename Type>
bool fdivx(State *state, Instruction instr)
{
   Type a = getFpr<Type>(state, instr.frA);
   Type b = getFpr<Type>(state, instr.frB);
   Type r;

   state->reg.fpscr.vxzdz  |= isZero(a) && isZero(b);
   state->reg.fpscr.vxsnan |= isSignallingNaN(a) || isSignallingNaN(b);
   state->reg.fpscr.vxidi  |= isInfinity(a) && isInfinity(b);

   r = a / b;
   updateFpscr(state);
   updateFprf(state, r);
   setFpr<Type>(state, instr.frD, r);

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

/* fdiv, fdiv. */
bool fdiv(State *state, Instruction instr)
{
   return fdivx<double>(state, instr);
}

/* fdivs, fdivs. */
bool fdivs(State *state, Instruction instr)
{
   return fdivx<float>(state, instr);
}

/* fmadd, fmadd., fmadds, fmadds. */
template<typename Type>
bool fmaddx(State *state, Instruction instr)
{
   Type a = getFpr<Type>(state, instr.frA);
   Type b = getFpr<Type>(state, instr.frB);
   Type c = getFpr<Type>(state, instr.frC);
   Type r;

   state->reg.fpscr.vximz  |= isInfinity(a) && isZero(c);
   state->reg.fpscr.vxsnan |= isSignallingNaN(a) || isSignallingNaN(b) || isSignallingNaN(c);
   state->reg.fpscr.vxisi  |= (isInfinity(a) || isInfinity(c)) && isInfinity(b);

   r = (a * c) + b;
   updateFpscr(state);
   updateFprf(state, r);
   setFpr<Type>(state, instr.frD, r);

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

/* fmadd, fmadd. */
bool fmadd(State *state, Instruction instr)
{
   return fmaddx<double>(state, instr);
}

/* fmadds, fmadds. */
bool fmadds(State *state, Instruction instr)
{
   return fmaddx<float>(state, instr);
}

/* fmr, fmr. */
bool fmr(State *state, Instruction instr)
{
   fpr(instr.frD) = fpr(instr.frB);

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

/* fmsub, fmsub., fmsubs, fmsubs. */
template<typename Type>
bool fmsubx(State *state, Instruction instr)
{
   Type a = getFpr<Type>(state, instr.frA);
   Type b = getFpr<Type>(state, instr.frB);
   Type c = getFpr<Type>(state, instr.frC);
   Type r;

   state->reg.fpscr.vximz  |= isInfinity(a) && isZero(c);
   state->reg.fpscr.vxsnan |= isSignallingNaN(a) || isSignallingNaN(b) || isSignallingNaN(c);
   state->reg.fpscr.vxisi  |= (isInfinity(a) || isInfinity(c)) && isInfinity(b);

   r = (a * c) - b;
   updateFpscr(state);
   updateFprf(state, r);
   setFpr<Type>(state, instr.frD, r);

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

/* fmsub, fmsub. */
bool fmsub(State *state, Instruction instr)
{
   return fmsubx<double>(state, instr);
}

/* fmsubs, fmsubs. */
bool fmsubs(State *state, Instruction instr)
{
   return fmsubx<float>(state, instr);
}

/* fmul, fmul., fmuls, fmuls. */
template<typename Type>
bool fmulx(State *state, Instruction instr)
{
   Type a = getFpr<Type>(state, instr.frA);
   Type c = getFpr<Type>(state, instr.frC);
   Type r;

   state->reg.fpscr.vximz  |= isInfinity(a) && isZero(c);
   state->reg.fpscr.vxsnan |= isSignallingNaN(a) || isSignallingNaN(c);

   r = a * c;
   updateFpscr(state);
   updateFprf(state, r);
   setFpr<Type>(state, instr.frD, r);

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

/* fmul, fmul. */
bool fmul(State *state, Instruction instr)
{
   return fmulx<double>(state, instr);
}

/* fmuls, fmuls. */
bool fmuls(State *state, Instruction instr)
{
   return fmulx<float>(state, instr);
}

/* fnabs, fnabs. */
bool fnabs(State *state, Instruction instr)
{
   fpr(instr.frD) = -std::fabs(fpr(instr.frB));

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

/* fneg, fneg. */
bool fneg(State *state, Instruction instr)
{
   fpr(instr.frD) = fpr(instr.frB);
   *reinterpret_cast<uint64_t*>(&fpr(instr.frD)) ^= 1ull << 63;

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

/* fnmadd, fnmadd., fnmadds, fnmadds. */
template<typename Type>
bool fnmaddx(State *state, Instruction instr)
{
   Type a = getFpr<Type>(state, instr.frA);
   Type b = getFpr<Type>(state, instr.frB);
   Type c = getFpr<Type>(state, instr.frC);
   Type r;

   state->reg.fpscr.vximz  |= isInfinity(a) && isZero(c);
   state->reg.fpscr.vxsnan |= isSignallingNaN(a) || isSignallingNaN(b) || isSignallingNaN(c);
   state->reg.fpscr.vxisi  |= (isInfinity(a) || isInfinity(c)) && isInfinity(b);

   r = -((a * c) + b);
   updateFpscr(state);
   updateFprf(state, r);
   setFpr<Type>(state, instr.frD, r);

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}


/* fnmadd, fnmadd. */
bool fnmadd(State *state, Instruction instr)
{
   return fnmaddx<double>(state, instr);
}

/* fnmadds, fnmadds. */
bool fnmadds(State *state, Instruction instr)
{
   return fnmaddx<float>(state, instr);
}

/* fnmsub, fnmsub., fnmsubs, fnmsubs. */
template<typename Type>
bool fnmsubx(State *state, Instruction instr)
{
   Type a = getFpr<Type>(state, instr.frA);
   Type b = getFpr<Type>(state, instr.frB);
   Type c = getFpr<Type>(state, instr.frC);

   state->reg.fpscr.vximz  |= isInfinity(a) && isZero(c);
   state->reg.fpscr.vxsnan |= isSignallingNaN(a) || isSignallingNaN(b) || isSignallingNaN(c);
   state->reg.fpscr.vxisi  |= (isInfinity(a) || isInfinity(c)) && isInfinity(b);

   Type r = -((a * c) - b);
   updateFpscr(state);
   updateFprf(state, r);
   setFpr<Type>(state, instr.frD, r);

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

/* fnmsub, fnmsub. */
bool fnmsub(State *state, Instruction instr)
{
   return fnmsubx<double>(state, instr);
}

/* fnmsubs, fnmsubs. */
bool fnmsubs(State *state, Instruction instr)
{
   return fnmsubx<float>(state, instr);
}

/* fres, fres. */
bool fres(State *state, Instruction instr)
{
   state->reg.fpscr.vxsnan |= isSignallingNaN(fpr(instr.frB));

   float b = getFpr<float>(state, instr.frB);
   float r = 1.0f / b;

   updateFpscr(state);
   updateFprf(state, r);
   setFpr<float>(state, instr.frD, r);

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

/* frsp, frsp. */
bool frsp(State *state, Instruction instr)
{
   state->reg.fpscr.vxsnan |= isSignallingNaN(fpr(instr.frB));

   /* TODO: use rounding mode in FPSCR[RN] */
   float r = static_cast<float>(fpr(instr.frB));

   updateFpscr(state);
   updateFprf(state, r);
   setFpr<float>(state, instr.frD, r);

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

/* frsqrte, frsqrte. */
bool frsqrte(State *state, Instruction instr)
{
   double b = fpr(instr.frB);
   double r;

   state->reg.fpscr.vxsnan |= isSignallingNaN(b);
   state->reg.fpscr.vxsqrt |= isNegativeInfinity(b) | isSignallingNaN(b) | isNegative(b);

   if (isNegativeZero(b)) {
      r = getNegativeInfinity<double>();
   } if (isPositiveZero(b)) {
      r = getPositiveInfinity<double>();
   } else if (isPositiveInfinity(b)) {
      r = 0.0;
   } else if (isNegativeInfinity(b) || isSignallingNaN(b) || isQuietNaN(b) || isNegative(b)) {
      r = getQuietNaN<double>();
   } else {
      /* TODO: frsqrte as estimate, not exact */
      r = 1.0 / std::sqrt(b);
   }

   updateFpscr(state);
   updateFprf(state, r);
   setFpr<double>(state, instr.frD, r);

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

/* fsel, fsel. */
bool fsel(State *state, Instruction instr)
{
   fpr(instr.frD) = fpr(instr.frA) > 0.0 ? fpr(instr.frC) : fpr(instr.frB);

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

/* fsqrt, fsqrt. */
template<typename Type>
bool fsqrtx(State *state, Instruction instr)
{
   Type b = getFpr<Type>(state, instr.frB);
   Type r;

   state->reg.fpscr.vxsnan |= isSignallingNaN(b);
   state->reg.fpscr.vxsqrt |= isNegativeInfinity(b) | isSignallingNaN(b) | isNegative(b);

   if (isNegativeZero(b)) {
      r = getNegativeZero<Type>();
   } else if (isPositiveInfinity(b)) {
      r = getPositiveInfinity<Type>();
   } else if (isNegativeInfinity(b) || isSignallingNaN(b) || isQuietNaN(b) || isNegative(b)) {
      r = getQuietNaN<Type>();
   } else {
      r = std::sqrt(b);
   }

   updateFpscr(state);
   updateFprf(state, fpr(instr.frD));
   setFpr<Type>(state, instr.frD, r);

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

/* fsqrt, fsqrt. */
bool fsqrt(State *state, Instruction instr)
{
   return fsqrtx<double>(state, instr);
}

/* fsqrts, fsqrts. */
bool fsqrts(State *state, Instruction instr)
{
   return fsqrtx<float>(state, instr);
}

/* fsub, fsub., fsubs, fsubs. */
template<typename Type>
bool fsubx(State *state, Instruction instr)
{
   Type a = getFpr<Type>(state, instr.frA);
   Type b = getFpr<Type>(state, instr.frB);
   Type r;

   state->reg.fpscr.vxsnan |= isSignallingNaN(a) || isSignallingNaN(b);
   state->reg.fpscr.vxisi  |= isInfinity(a) && isInfinity(b);

   r = a - b;
   updateFpscr(state);
   updateFprf(state, fpr(instr.frD));
   setFpr<Type>(state, instr.frD, r);

   if (instr.rc) {
      updateCr1(state);
   }

   return true;
}

/* fsub, fsub. */
bool fsub(State *state, Instruction instr)
{
   return fsubx<double>(state, instr);
}

/* fsubs, fsubs. */
bool fsubs(State *state, Instruction instr)
{
   return fsubx<float>(state, instr);
}

} // namespace Interpreter

} // namespace ppc
