#include "interpreter.h"
#include "regs.h"

#include "powerpc/instructions.h"

#include "common/log.h"
#include "common/memory.h"

#include "kernel/module.h"

#include "xex/xex.h"

#include <functional>

namespace ppc 
{

namespace Interpreter
{

/* Kernel Function Call */
bool krncall(State *state, Instruction instr)
{
   auto ptr  = Memory::read<uint64_t>(state->cia + 4);
   auto valid = instr.rD;
   auto args  = instr.rA;

   if (!valid) {
      xex::ImportLibrary *library = reinterpret_cast<xex::ImportLibrary*>(ptr);
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

   // TODO: Revisit if need fpr or vpr instead of gpr
   auto exprt = reinterpret_cast<Module::Export*>(ptr);
   auto func = reinterpret_cast<Module::BoundFunction*>(exprt->ptr);
   func->call(state->reg.gpr);

   state->nia = state->reg.lr;
   return true;
}

} // namespace Interpreter

} // namespace ppc
