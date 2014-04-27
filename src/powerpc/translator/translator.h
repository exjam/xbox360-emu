#ifndef PPC_TRANSLATOR_H
#define PPC_TRANSLATOR_H

#include "powerpc/cpu.h"
#include "exjit/irbuilder.h"

namespace ppc
{

namespace Translator
{

struct State
{
   State(exjit::Context *context) :
      context(context),
      builder(context)
   {
   }

   exjit::Value *gpr(unsigned i)
   {
      return _gpr[i];
   }

   void setGpr(unsigned i, exjit::Value *value)
   {
      _gpr[i] = value;
   }

   exjit::Value *_gpr[32];
   exjit::Context *context;
   exjit::IRBuilder builder;
};

#include "powerpc/emugen_stubs.h"

} // namespace Translator

} // namespace ppc

#endif // PPC_TRANSLATOR_H
