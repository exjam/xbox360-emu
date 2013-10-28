#include "ppc/disassembler.h"
#include "ppc/instructions.h"
#include "ppc/cpu.h"

#include "util/bits.h"
#include "util/log.h"

#include <sstream>
#include <iomanip>

using bits::signExtend;

namespace ppc
{

namespace Disassembler
{

#include "emugen_instr_table.cpp"

#include "emugen_instr_disasm.cpp"

} // namespace Disassembler

} //namespace ppc
