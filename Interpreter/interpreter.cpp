#include "ppc/interpreter.h"
#include "ppc/instructions.h"

#include "util/log.h"
#include "util/bits.h"
#include "util/be/memory.h"

#include "common.h"

namespace ppc 
{

namespace Interpreter
{

#include "emugen_instr_table.cpp"

} // namespace Interpreter

} // namespace ppc
