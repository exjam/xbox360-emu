#ifndef PPC_DISASSEMBLER_H
#define PPC_DISASSEMBLER_H

#include <iostream>
#include <stdint.h>
#include <vector>

#include "ppc/cpu.h"

namespace ppc
{
struct Instruction;

namespace Disassembler
{

struct Operand
{
   enum Access {
      Read,
      Write,
      ReadWrite
   };

   enum Type {
      Gpr,
      Spr,
      UImm,
      SImm,
      Addr,
      GprOfs,
      Crfd
   };

   Type type;

   union {
      struct {
         uint32_t id;
         int64_t offset;
         Access access;
      } gpr;

      struct {
         uint32_t id;
         Access access;
      } spr;

      struct {
         uint64_t value;
      } addr;

      struct {
         uint64_t value;
      } uimm;

      struct {
         int64_t value;
      } simm;

      struct {
         int32_t crN;
         Access access;
      } crfd;
   };
};

struct State
{
   uint64_t cia;

   struct {
      struct Operands {
         Operands &operator << (Operand &op) {
            ops.push_back(op);
            return *this;
         }

         std::vector<Operand> ops;
      };

      std::string code;
      std::string name;
      Instruction instr;
      Operands operands;
   } result;
};

std::string toString(State *state);

#include "emugen_instr_table.h"

} // namespace Disassembler

} // namespace ppc

#endif // PPC_DISASSEMBLER_H
