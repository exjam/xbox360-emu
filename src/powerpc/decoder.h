#ifndef PPC_DECODER_H
#define PPC_DECODER_H

#include "cpu.h"
#include "instructions.h"

namespace ppc
{
   InstructionID decode(Instruction instr);
};

#endif
