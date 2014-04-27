#ifndef REGS_H
#define REGS_H

namespace ppc 
{

namespace Interpreter
{

#define UNIMPLEMENTED(insName) \
   bool insName(State *state, Instruction) { \
      xDebug() << "Unimplemented interpreter instruction "#insName; \
      return false; \
   }

#define gpr(r) state->reg.gpr[r]
#define gpr0(r) (r == 0 ? 0 : state->reg.gpr[r])
#define gprs(r) reinterpret<int64_t>(state->reg.gpr[r])
#define gprw(r) reinterpret<uint32_t>(state->reg.gpr[r])
#define gprsw(r) reinterpret<int32_t>(state->reg.gpr[r])

#define fpr(r) state->reg.fpr[r]
#define fpscr(n) state->reg.fpscr.crn[n]

#define crf(n) state->reg.cr.crn[n]
#define crb(n) state->reg.cr.crb[n]

template<typename DstType, typename SrcType>
static inline DstType& reinterpret(SrcType& value)
{
   return *reinterpret_cast<DstType*>(&value);
}

} // namespace Interpreter

} // namespace ppc 

#endif // REGS_H
