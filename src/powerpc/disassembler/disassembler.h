#ifndef PPC_DISASSEMBLER_H
#define PPC_DISASSEMBLER_H

#include "powerpc/cpu.h"

#include <string>

namespace ppc
{

namespace Disassembler
{

struct Result
{
   enum Type
   {
      Invalid,
      Register,
      Constant,
      ConstantSigned,
      Address
   };

   struct Arg
   {
      Type type;
      std::string text;

      union
      {
         uint64_t address;
         uint64_t constant;
         int64_t constantSigned;
      };
   };

   std::string name;
   std::string fullname;
   std::string disasm;
   std::vector<Arg> args;
};

struct State
{
   uint64_t cia;
   Result result;
};

bool decode(State *state, Instruction instr);

} // namespace Disassembler

} // namespace ppc

#endif // PPC_DISASSEMBLER_H
