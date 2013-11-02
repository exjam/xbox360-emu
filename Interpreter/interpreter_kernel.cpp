#include "ppc/interpreter.h"
#include "ppc/instructions.h"

#include "util/log.h"
#include "util/bits.h"
#include "util/be/memory.h"

#include "common.h"

#include <functional>

namespace ppc 
{

namespace Interpreter
{

void kernelCall(State *state, uint64_t fptr)
{
}

typedef uint64_t (*callKernelUnkArg)(...);

/* Kernel Function Call */
bool krncall(State *state, Instruction instr)
{
   auto fptr = be::Memory::read<uint64_t>(state->cia + 4);
   auto valid = instr.rD;
   auto args  = instr.rA;

   if (!valid) {
      xex::ImportLibrary *library = reinterpret_cast<xex::ImportLibrary*>(fptr);
      xex::ImportLibrary::Import *import = nullptr;
      auto ordinal = instr.uimm;

      for (auto &imp : library->imports) {
         if (imp.ordinal == ordinal) {
            import = &imp;
         }
      }

      if (import) {
         xDebug() << "Kernel call to unresolved import " << library->name << "(" << ordinal << ") " << import->name;
      } else {
         xDebug() << "Kernel call to unresolved import " << library->name << "(" << ordinal << ")";
      }

      assert(0);
      return false;
   }

   callKernelUnkArg func = reinterpret_cast<callKernelUnkArg>(fptr);

   if (args == 0) {
      state->reg.gpr[3] = func();
   } else if (args == 1) {
      state->reg.gpr[3] = func(state->reg.gpr[3]);
   } else if (args == 2) {
      state->reg.gpr[3] = func(state->reg.gpr[3], state->reg.gpr[4]);
   } else if (args == 3) {
      state->reg.gpr[3] = func(state->reg.gpr[3], state->reg.gpr[4], state->reg.gpr[5]);
   } else if (args == 4) {
      state->reg.gpr[3] = func(state->reg.gpr[3], state->reg.gpr[4], state->reg.gpr[5], state->reg.gpr[6]);
   } else if (args == 5) {
      state->reg.gpr[3] = func(state->reg.gpr[3], state->reg.gpr[4], state->reg.gpr[5], state->reg.gpr[6], state->reg.gpr[7]);
   } else if (args == 6) {
      state->reg.gpr[3] = func(state->reg.gpr[3], state->reg.gpr[4], state->reg.gpr[5], state->reg.gpr[6], state->reg.gpr[7], state->reg.gpr[8]);
   } else if (args == 7) {
      state->reg.gpr[3] = func(state->reg.gpr[3], state->reg.gpr[4], state->reg.gpr[5], state->reg.gpr[6], state->reg.gpr[7], state->reg.gpr[8], state->reg.gpr[9]);
   } else if (args == 8) {
      state->reg.gpr[3] = func(state->reg.gpr[3], state->reg.gpr[4], state->reg.gpr[5], state->reg.gpr[6], state->reg.gpr[7], state->reg.gpr[8], state->reg.gpr[9], state->reg.gpr[10]);
   }

   state->nia = state->reg.lr;
   return true;
}

} // namespace Interpreter

} // namespace ppc
