#include "ppc/disassembler.h"
#include "ppc/instructions.h"
#include "ppc/cpu.h"
#include <ppc/decoder.h>

#include "util/bits.h"
#include "util/log.h"

#include <sstream>
#include <iomanip>
#include <map>

using bits::signExtend;

std::map<int, std::string> g_sprNames {
   { ppc::Registers::XER,   "XER" },
   { ppc::Registers::LR,    "LR" },
   { ppc::Registers::CTR,   "CTR" },
   { ppc::Registers::DSISR, "DSISR" },
   { ppc::Registers::DAR,   "DAR" },
   { ppc::Registers::DEC,   "DEC" },
   { ppc::Registers::SDR1,  "SDR1" },
   { ppc::Registers::SRR0,  "SRR0" },
   { ppc::Registers::SRR1,  "SRR1" },
   { ppc::Registers::SPRG0, "SPRG0" },
   { ppc::Registers::SPRG1, "SPRG1" },
   { ppc::Registers::SPRG2, "SPRG2" },
   { ppc::Registers::SPRG3, "SPRG3" },
   { ppc::Registers::ASR,   "ASR" },
   { ppc::Registers::EAR,   "EAR" },
   { ppc::Registers::TBU,   "TBU" },
   { ppc::Registers::TBL,   "TBL" },
   { ppc::Registers::DABR,  "DABR" },
};

namespace ppc
{

namespace Disassembler
{

bool decode(State *state, Instruction instr)
{
   InstructionID id = ppc::decode(instr);

   if (id == InstructionID::Unknown) {
      return false;
   }

   auto &data = ppc::getInstruction(id);
   state->result.code = data.name;
   state->result.name = data.fullname;
   state->result.operands = "fuk u";

   return false;
}

} // namespace Disassembler

} //namespace ppc
