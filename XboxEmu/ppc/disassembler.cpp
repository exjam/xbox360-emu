#include "ppc/disassembler.h"
#include "ppc/instructions.h"
#include "ppc/cpu.h"

#include "util/bits.h"

#include <sstream>
#include <iomanip>

namespace ppc
{
namespace Disassembler
{

static inline Operand addr(uint64_t value)
{
   Operand op;
   op.type = Operand::Addr;
   op.addr.value = value;
   return op;
}

static inline Operand simm(int64_t value)
{
   Operand op;
   op.type = Operand::SImm;
   op.simm.value = value;
   return op;
}

static inline Operand uimm(int64_t value)
{
   Operand op;
   op.type = Operand::UImm;
   op.uimm.value = value;
   return op;
}

static inline Operand gpr(uint32_t id, Operand::Access access = Operand::Read)
{
   Operand op;
   op.type = Operand::Gpr;
   op.gpr.id = id;
   op.gpr.access = access;
   return op;
}

static inline Operand spr(uint32_t id, Operand::Access access = Operand::Read)
{
   Operand op;
   op.type = Operand::Spr;
   op.spr.id = ((id & 0x1f) << 5) | ((id >> 5) & 0x1f);
   op.spr.access = access;
   return op;
}

static inline Operand gpr_ofs(uint32_t gpr, int64_t offset, Operand::Access access = Operand::Read)
{
   Operand op;
   op.type = Operand::GprOfs;
   op.gpr.id = gpr;
   op.gpr.offset = offset;
   op.gpr.access = access;
   return op;
}

static inline std::string oerc(const std::string& code, uint32_t oe, uint32_t rc)
{
   std::string result = code;
   if (oe) { result.push_back('o'); }
   if (rc) { result.push_back('.'); }
   return result;
}

static inline std::string lkaa(const std::string& code, uint32_t lk, uint32_t aa)
{
   std::string result = code;
   if (lk) { result.push_back('l'); }
   if (aa) { result.push_back('a'); }
   return result;
}

std::string toString(State *state)
{
   std::stringstream ss;
   ss << state->result.code << " ";
   bool first = true;

   for (auto op : state->result.operands.ops) {
      if (!first) {
         ss << ", ";
      }

      switch (op.type) {
      case Operand::Gpr:
         ss << "r" << op.gpr.id;
         break;
      case Operand::Spr:
         ss << "s" << op.spr.id;
         break;
      case Operand::UImm:
         ss << op.uimm.value;
         break;
      case Operand::SImm:
         ss << op.simm.value;
         break;
      case Operand::Addr:
         ss << "0x" << std::hex << std::setfill('0') << std::setw(8) << op.addr.value << std::dec;
         break;
      case Operand::GprOfs:
         ss << "r" << op.gpr.id;

         if (op.gpr.offset < 0) {
            ss << " - ";
         } else {
            ss << " + ";
         }

         ss << "0x" << std::hex << std::setfill('0') << abs(op.gpr.offset) << std::dec;
         break;
      }

      first = false;
   }

   return ss.str();
}

/* add, addc, adde */
bool add(State *state, Instruction instr)
{
   state->result.code = "add";
   state->result.name = "Add";
   state->result.operands
      << gpr(instr.rD, Operand::Write)
      << gpr(instr.rA)
      << gpr(instr.rB);

   if (instr.subop9_oe == ppc::op::addc) {
      state->result.code += 'c';
      state->result.name += " Carrying";
   } else if (instr.subop9_oe == ppc::op::adde) {
      state->result.code += 'e';
      state->result.name += " Extended";
   }

   state->result.code = oerc(state->result.code, instr.oe, instr.rc);
   return true;
}

/* addi, addis */
bool addi(State *state, Instruction instr)
{
   state->result.code = "addi";
   state->result.name = "Add Immediate";
   state->result.operands
      << gpr(instr.rD, Operand::Write)
      << gpr(instr.rA)
      << simm(bits::signExtend<16>(instr.simm));

   if (instr.subop9_oe == ppc::op::addis) {
      state->result.code += 's';
      state->result.name += " Shifted";
   }

   return true;
}

/* addic, addic. */
bool addic(State *state, Instruction instr)
{
   state->result.code = "addi";
   state->result.name = "Add Immediate Carrying";
   state->result.operands
      << gpr(instr.rD, Operand::Write)
      << gpr(instr.rA)
      << simm(bits::signExtend<16>(instr.simm));

   if (instr.subop9_oe == ppc::op::addicr) {
      state->result.code += '.';
      state->result.name += " and Record";
   }

   return true;
}

/* addme */
bool addme(State *state, Instruction instr)
{
   state->result.code = oerc("addme", instr.oe, instr.rc);
   state->result.name = "Add to Minus One Extended";
   state->result.operands
      << gpr(instr.rD, Operand::Write)
      << gpr(instr.rA);

   return true;
}

/* addze */
bool addze(State *state, Instruction instr)
{
   state->result.code = oerc("addze", instr.oe, instr.rc);
   state->result.name = "Add to Zero Extended";
   state->result.operands
      << gpr(instr.rD, Operand::Write)
      << gpr(instr.rA);

   return true;
}

/* and */
bool and(State *state, Instruction instr)
{
   state->result.code = oerc("and", 0, instr.rc);
   state->result.name = "AND";
   state->result.operands
      << gpr(instr.rA, Operand::Write)
      << gpr(instr.rS)
      << gpr(instr.rB);

   return true;
}

/* andc */
bool andc(State *state, Instruction instr)
{
   state->result.code = oerc("and", 0, instr.rc);
   state->result.name = "AND with Complement";
   state->result.operands
      << gpr(instr.rA, Operand::Write)
      << gpr(instr.rS)
      << gpr(instr.rB);

   return true;
}

/* andi, andis */
bool andi(State *state, Instruction instr)
{
   state->result.code = "andi";
   state->result.name = "AND Immediate";
   state->result.operands
      << gpr(instr.rA, Operand::Write)
      << gpr(instr.rS)
      << uimm(instr.uimm);

   if (instr.opcode == ppc::op::andis) {
      state->result.code += 's';
      state->result.name += " Shifted";
   }

   state->result.code += '.';
   return true;
}

/* b */
bool b(State *state, Instruction instr)
{
   state->result.code = lkaa("b", instr.lk, instr.aa);
   state->result.name = "Branch";
   state->result.operands
      << addr(instr.aa ? 0 : state->cia + bits::signExtend<26>(instr.li << 2));

   return true;
}

/* bclr */
bool bclr(State *state, Instruction instr)
{
   state->result.code = instr.lk ? "bclrl" : "bclr";
   state->result.name = "Branch Conditional to Link Register";
   state->result.operands
      << uimm(instr.bo)
      << uimm(instr.bi)
      << uimm(instr.bh);

   return true;
}

/* std, stdu */
bool std(State *state, Instruction instr)
{
   state->result.code = instr.rc ? "stdu" : "std";
   state->result.name = "Store Double Word";
   state->result.operands
      << gpr(instr.rS)
      << gpr_ofs(instr.rA, bits::signExtend<16>(instr.ds << 2), instr.rc ? Operand::ReadWrite : Operand::Read);

   return false;
}

/* stw, stwu */
bool stw(State *state, Instruction instr)
{
   state->result.code = "stw";
   state->result.name = "Store Word";
   state->result.operands
      << gpr(instr.rS)
      << gpr_ofs(instr.rA, bits::signExtend<16>(instr.d), (instr.opcode == ppc::op::stwu) ? Operand::ReadWrite : Operand::Read);
   
   if (instr.opcode == ppc::op::stwu) {
      state->result.code += 'u';
      state->result.name += " with Update";
   }

   return false;
}

/* mfspr */
bool mfspr(State *state, Instruction instr)
{
   state->result.code = "mfspr";
   state->result.name = "Move From Special-Purpose Register";
   state->result.operands
      << gpr(instr.rD, Operand::Write)
      << spr(instr.spr);

   return true;
}

}
}
