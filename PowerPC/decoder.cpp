#include "ppc/cpu.h"
#include "ppc/decoder.h"
#include "ppc/instructions.h"

using namespace ppc;

#include <emugen_types.h>

using emugen::DecodeEntry;

namespace ppc
{
#include "emugen_decoder.h"
};

struct InitOnLoad
{
   InitOnLoad()
   {
      ppc::init();
   }
};

InitOnLoad g_initOnLoad;
