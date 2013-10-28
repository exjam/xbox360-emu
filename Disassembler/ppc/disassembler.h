#ifndef PPC_DISASSEMBLER_H
#define PPC_DISASSEMBLER_H

#include "ppc/cpu.h"

#include <string>

namespace ppc
{
struct Instruction;

namespace Disassembler
{

struct State
{
   uint64_t cia;

   struct {
      std::string code;
      std::string name;
      std::string operands;
   } result;
};

#include "emugen_instr_table.h"

} // namespace Disassembler

} // namespace ppc

#endif // PPC_DISASSEMBLER_H
