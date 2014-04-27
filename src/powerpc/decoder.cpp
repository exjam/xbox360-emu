#include "cpu.h"
#include "decoder.h"
#include "instructions.h"

using namespace ppc;

#include "../common/emugen_types.h"

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
