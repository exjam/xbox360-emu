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

#define fpr(id) state->reg.fpr[id]
#define fpscr(n) bits::field<uint32_t>(state->reg.fpscr.value, n * 4, 4)

#define crf(n) bits::field<uint32_t>(state->reg.cr.value, n * 4, 4)
#define crb(n) bits::field<uint32_t>(state->reg.cr.value, n, 1)

template<typename DstType, typename SrcType>
static inline DstType& reinterpret(SrcType& value)
{
   return *reinterpret_cast<DstType*>(&value);
}

} // namespace Interpreter

} // namespace ppc 
