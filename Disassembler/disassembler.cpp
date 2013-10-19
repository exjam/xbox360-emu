#include "ppc/disassembler.h"
#include "ppc/instructions.h"
#include "ppc/cpu.h"

#include "util/bits.h"
#include "util/log.h"

#include <sstream>
#include <iomanip>

namespace ppc
{
namespace Disassembler
{

#define UNIMPLEMENTED(insName) \
   bool insName (State *state, Instruction) { \
      xDebug() << "Unimplemented disassembler instruction "#insName ;  \
      state->result.code = #insName ; \
      return false; \
   }

static inline Operand addr(uint64_t value)
{
   Operand op;
   op.type = Operand::Addr;
   op.addr.value = value;
   return op;
}

static inline Operand simm(uint32_t value)
{
   Operand op;
   op.type = Operand::SImm;
   op.simm.value = bits::signExtend<16>(value);
   return op;
}

static inline Operand uimm(int64_t value)
{
   Operand op;
   op.type = Operand::UImm;
   op.uimm.value = value;
   return op;
}

static inline Operand crfd(int32_t value, Operand::Access access = Operand::Read)
{
   Operand op;
   op.type = Operand::Crfd;
   op.crfd.crN = value;
   op.crfd.access = access;
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

/* add */
bool add(State *state, Instruction instr)
{
   state->result.code = oerc("add", instr.oe, instr.rc);
   state->result.name = "Add";
   state->result.operands
      << gpr(instr.rD, Operand::Write)
      << gpr(instr.rA)
      << gpr(instr.rB);

   return true;
}

/* addc */
bool addc(State *state, Instruction instr)
{
   state->result.code = oerc("addc", instr.oe, instr.rc);
   state->result.name = "Add Carrying";
   state->result.operands
      << gpr(instr.rD, Operand::Write)
      << gpr(instr.rA)
      << gpr(instr.rB);

   return true;
}

/* adde */
bool adde(State *state, Instruction instr)
{
   state->result.code = oerc("adde", instr.oe, instr.rc);
   state->result.name = "Add Extended";
   state->result.operands
      << gpr(instr.rD, Operand::Write)
      << gpr(instr.rA)
      << gpr(instr.rB);

   return true;
}

/* addi */
bool addi(State *state, Instruction instr)
{
   state->result.code = "addi";
   state->result.name = "Add Immediate";
   state->result.operands
      << gpr(instr.rD, Operand::Write)
      << gpr(instr.rA)
      << simm(instr.simm);

   return true;
}

/* addic */
bool addic(State *state, Instruction instr)
{
   state->result.code = "addi";
   state->result.name = "Add Immediate Carrying";
   state->result.operands
      << gpr(instr.rD, Operand::Write)
      << gpr(instr.rA)
      << simm(instr.simm);

   return true;
}

/* addic. */
bool addico(State *state, Instruction instr)
{
   state->result.code = "addic.";
   state->result.name = "Add Immediate Carrying and Record";
   state->result.operands
      << gpr(instr.rD, Operand::Write)
      << gpr(instr.rA)
      << simm(instr.simm);

   return true;
}

/* addis */
bool addis(State *state, Instruction instr)
{
   state->result.code = "addis";
   state->result.name = "Add Immediate Shifted";
   state->result.operands
      << gpr(instr.rD, Operand::Write)
      << gpr(instr.rA)
      << simm(instr.simm);

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

/* andi. */
bool andio(State *state, Instruction instr)
{
   state->result.code = "andi.";
   state->result.name = "AND Immediate";
   state->result.operands
      << gpr(instr.rA, Operand::Write)
      << gpr(instr.rS)
      << uimm(instr.uimm);

   return true;
}

/* andis. */
bool andiso(State *state, Instruction instr)
{
   state->result.code = "andis.";
   state->result.name = "AND Immediate Shifted";
   state->result.operands
      << gpr(instr.rA, Operand::Write)
      << gpr(instr.rS)
      << uimm(instr.uimm);
   
   return true;
}

/* b */
bool b(State *state, Instruction instr)
{
   state->result.code = lkaa("b", instr.lk, instr.aa);
   state->result.name = "Branch";
   state->result.operands
      << addr((instr.aa ? 0 : state->cia) + bits::signExtend<26>(static_cast<uint64_t>(instr.li) << 2));

   return true;
}

/* bc */
bool bc(State *state, Instruction instr)
{
   state->result.code = lkaa("bc", instr.lk, instr.aa);
   state->result.name = "Branch Conditional";
   state->result.operands
      << uimm(instr.bo) //TODO: bo
      << uimm(instr.bi) //TODO: bi
      << addr((instr.aa ? 0 : state->cia) + bits::signExtend<16>(static_cast<uint64_t>(instr.bd) << 2));

   return true;
}

/* bcctr */
bool bcctr(State *state, Instruction instr)
{
   state->result.code = lkaa("bcctr", instr.lk, 0);
   state->result.name = "Branch Conditional to Count Register";
   state->result.operands
      << uimm(instr.bo) //TODO: bo
      << uimm(instr.bi) //TODO: bi
      << uimm(instr.bh);//TODO: bh

   return true;
}

/* bclr */
bool bclr(State *state, Instruction instr)
{
   state->result.code = lkaa("bclr", instr.lk, 0);
   state->result.name = "Branch Conditional to Link Register";
   state->result.operands
      << uimm(instr.bo) //TODO: bo
      << uimm(instr.bi) //TODO: bi
      << uimm(instr.bh);//TODO: bh

   return true;
}

/* cmpw, cmpd */
bool cmp(State *state, Instruction instr)
{
   state->result.code = instr.l ? "cmpw" : "cmpd";
   state->result.name = "Compare";
   state->result.operands
      << crfd(instr.crfD, Operand::Write)
      << gpr(instr.rA)
      << gpr(instr.rB);

   return true;
}

/* cmpwi, cmpdi */
bool cmpi(State *state, Instruction instr)
{
   state->result.code = instr.l ? "cmpwi" : "cmpdi";
   state->result.name = "Compare Immediate";
   state->result.operands
      << crfd(instr.crfD, Operand::Write)
      << gpr(instr.rA)
      << simm(instr.simm);

   return true;
}

/* cmplw, cmpld */
bool cmpl(State *state, Instruction instr)
{
   state->result.code = instr.l ? "cmplw" : "cmpld";
   state->result.name = "Compare Logical";
   state->result.operands
      << crfd(instr.crfD, Operand::Write)
      << gpr(instr.rA)
      << gpr(instr.rB);

   return true;
}

/* cmplwi, cmpldi */
bool cmpli(State *state, Instruction instr)
{
   state->result.code = instr.l ? "cmplwi" : "cmpldi";
   state->result.name = "Compare Logical Immediate";
   state->result.operands
      << crfd(instr.crfD, Operand::Write)
      << gpr(instr.rA)
      << uimm(instr.uimm);

   return true;
}

/* cntlzd */
bool cntlzd(State *state, Instruction instr)
{
   state->result.code = oerc("cntlzd", 0, instr.rc);
   state->result.name = "Count Leading Zeros Doubleword";
   state->result.operands
      << gpr(instr.rA, Operand::Write)
      << gpr(instr.rS);

   return true;
}

/* cntlzd */
bool cntlzw(State *state, Instruction instr)
{
   state->result.code = oerc("cntlzd", 0, instr.rc);
   state->result.name = "Count Leading Zeros Word";
   state->result.operands
      << gpr(instr.rA, Operand::Write)
      << gpr(instr.rS);

   return true;
}

/* crand */
bool crand(State *state, Instruction instr)
{
   state->result.code = "crand";
   state->result.name = "Condition Register AND";
   state->result.operands
      << crfd(instr.crbD, Operand::Write)
      << crfd(instr.crbA)
      << crfd(instr.crbB);

   return true;
}

/* crandc */
bool crandc(State *state, Instruction instr)
{
   state->result.code = "crandc";
   state->result.name = "Condition Register AND with Complement";
   state->result.operands
      << crfd(instr.crbD, Operand::Write)
      << crfd(instr.crbA)
      << crfd(instr.crbB);

   return true;
}

/* creqv */
bool creqv(State *state, Instruction instr)
{
   state->result.code = "creqv";
   state->result.name = "Condition Register Equivalent";
   state->result.operands
      << crfd(instr.crbD, Operand::Write)
      << crfd(instr.crbA)
      << crfd(instr.crbB);

   return true;
}

/* crnand */
bool crnand(State *state, Instruction instr)
{
   state->result.code = "crnand";
   state->result.name = "Condition Register NAND";
   state->result.operands
      << crfd(instr.crbD, Operand::Write)
      << crfd(instr.crbA)
      << crfd(instr.crbB);

   return true;
}

/* crnor */
bool crnor(State *state, Instruction instr)
{
   state->result.code = "crnor";
   state->result.name = "Condition Register NOR";
   state->result.operands
      << crfd(instr.crbD, Operand::Write)
      << crfd(instr.crbA)
      << crfd(instr.crbB);

   return true;
}

/* cror */
bool cror(State *state, Instruction instr)
{
   state->result.code = "cror";
   state->result.name = "Condition Register OR";
   state->result.operands
      << crfd(instr.crbD, Operand::Write)
      << crfd(instr.crbA)
      << crfd(instr.crbB);

   return true;
}

/* crorc */
bool crorc(State *state, Instruction instr)
{
   state->result.code = "crorc";
   state->result.name = "Condition Register OR with Complement";
   state->result.operands
      << crfd(instr.crbD, Operand::Write)
      << crfd(instr.crbA)
      << crfd(instr.crbB);

   return true;
}

/* crxor */
bool crxor(State *state, Instruction instr)
{
   state->result.code = "crxor";
   state->result.name = "Condition Register XOR";
   state->result.operands
      << crfd(instr.crbD, Operand::Write)
      << crfd(instr.crbA)
      << crfd(instr.crbB);

   return true;
}

/* dcbf */
bool dcbf(State *state, Instruction instr)
{
   state->result.code = "dcbf";
   state->result.name = "Data Cache Block Flush";
   state->result.operands
      << gpr(instr.rA)
      << gpr(instr.rB);

   return true;
}

/* dcbst */
bool dcbst(State *state, Instruction instr)
{
   state->result.code = "dcbst";
   state->result.name = "Data Cache Block Store";
   state->result.operands
      << gpr(instr.rA)
      << gpr(instr.rB);

   return true;
}

/* dcbt */
bool dcbt(State *state, Instruction instr)
{
   state->result.code = "dcbt";
   state->result.name = "Data Cache Block Touch";
   state->result.operands
      << gpr(instr.rA)
      << gpr(instr.rB)
      << uimm(instr.th); /* TODO: maybe change this from uimm */

   return true;
}

/* dcbtst */
bool dcbtst(State *state, Instruction instr)
{
   state->result.code = "dcbtst";
   state->result.name = "Data Cache Block Touch for Store";
   state->result.operands
      << gpr(instr.rA)
      << gpr(instr.rB);

   return true;
}

/* dcbz */
bool dcbz(State *state, Instruction instr)
{
   state->result.code = "dcbz";
   state->result.name = "Data Cache Block Clear to Zero";
   state->result.operands
      << gpr(instr.rA)
      << gpr(instr.rB);

   return true;
}

/* divd divd. divdo divdo. */
bool divd(State *state, Instruction instr)
{
   state->result.code = oerc("divd", instr.oe, instr.rc);
   state->result.name = "Divide Doubleword";
   state->result.operands
      << gpr(instr.rD, Operand::Write)
      << gpr(instr.rA)
      << gpr(instr.rB);

   return true;
}

/* divdu divdu. divduo divduo. */
bool divdu(State *state, Instruction instr)
{
   state->result.code = oerc("divdu", instr.oe, instr.rc);
   state->result.name = "Divide Doubleword Unsigned";
   state->result.operands
      << gpr(instr.rD, Operand::Write)
      << gpr(instr.rA)
      << gpr(instr.rB);

   return true;
}

/* divw divw. divwo divwo. */
bool divw(State *state, Instruction instr)
{
   state->result.code = oerc("divw", instr.oe, instr.rc);
   state->result.name = "Divide Word";
   state->result.operands
      << gpr(instr.rD, Operand::Write)
      << gpr(instr.rA)
      << gpr(instr.rB);

   return true;
}

/* divwu divwu. divwuo divwuo. */
bool divwu(State *state, Instruction instr)
{
   state->result.code = oerc("divwu", instr.oe, instr.rc);
   state->result.name = "Divide Word Unsigned";
   state->result.operands
      << gpr(instr.rD, Operand::Write)
      << gpr(instr.rA)
      << gpr(instr.rB);

   return true;
}

/* eciwx */
bool eciwx(State *state, Instruction instr)
{
   state->result.code = "eciwx";
   state->result.name = "External Control In Word Indexed";
   state->result.operands
      << gpr(instr.rD, Operand::Write)
      << gpr(instr.rA)
      << gpr(instr.rB);

   return true;
}

/* ecowx */
bool ecowx(State *state, Instruction instr)
{
   state->result.code = "eciwx";
   state->result.name = "External Control Out Word Indexed";
   state->result.operands
      << gpr(instr.rS)
      << gpr(instr.rA)
      << gpr(instr.rB);

   return true;
}

/* eieio */
bool eieio(State *state, Instruction instr)
{
   state->result.code = "eieio";
   state->result.name = "Enforce In-Order Execution of IO";
   return true;
}

/* eqv, eqv. */
bool eqv(State *state, Instruction instr)
{
   state->result.code = oerc("eqv", 0, instr.rc);
   state->result.name = "Equivalent";
   state->result.operands
      << gpr(instr.rA, Operand::Write)
      << gpr(instr.rS)
      << gpr(instr.rB);

   return true;
}

/* extsb, extsb. */
bool extsb(State *state, Instruction instr)
{
   state->result.code = oerc("extsb", 0, instr.rc);
   state->result.name = "Extend Sign Byte";
   state->result.operands
      << gpr(instr.rA, Operand::Write)
      << gpr(instr.rS);

   return true;
}

/* extsh, extsh. */
bool extsh(State *state, Instruction instr)
{
   state->result.code = oerc("extsh", 0, instr.rc);
   state->result.name = "Extend Sign Halfword";
   state->result.operands
      << gpr(instr.rA, Operand::Write)
      << gpr(instr.rS);

   return true;
}

/* extsw, extsw. */
bool extsw(State *state, Instruction instr)
{
   state->result.code = oerc("extsw", 0, instr.rc);
   state->result.name = "Extend Sign Word";
   state->result.operands
      << gpr(instr.rA, Operand::Write)
      << gpr(instr.rS);

   return true;
}

UNIMPLEMENTED(fabs);
UNIMPLEMENTED(fadd);
UNIMPLEMENTED(fadds);
UNIMPLEMENTED(fcfid);
UNIMPLEMENTED(fcmpo);
UNIMPLEMENTED(fcmpu);
UNIMPLEMENTED(fctid);
UNIMPLEMENTED(fctidz);
UNIMPLEMENTED(fctiw);
UNIMPLEMENTED(fctiwz);
UNIMPLEMENTED(fdiv);
UNIMPLEMENTED(fdivs);
UNIMPLEMENTED(fmadd);
UNIMPLEMENTED(fmadds);
UNIMPLEMENTED(fmr);
UNIMPLEMENTED(fmsub);
UNIMPLEMENTED(fmsubs);
UNIMPLEMENTED(fmul);
UNIMPLEMENTED(fmuls);
UNIMPLEMENTED(fnabs);
UNIMPLEMENTED(fneg);
UNIMPLEMENTED(fnmadd);
UNIMPLEMENTED(fnmadds);
UNIMPLEMENTED(fnmsub);
UNIMPLEMENTED(fnmsubs);
UNIMPLEMENTED(fres);
UNIMPLEMENTED(frsp);
UNIMPLEMENTED(frsqrte);
UNIMPLEMENTED(fsel);
UNIMPLEMENTED(fsqrt);
UNIMPLEMENTED(fsqrts);
UNIMPLEMENTED(fsub);
UNIMPLEMENTED(fsubs);
UNIMPLEMENTED(icbi);
UNIMPLEMENTED(isync);

/* stb */
bool stb(State *state, Instruction instr)
{
   state->result.code = "stb";
   state->result.name = "Store Byte";
   state->result.operands
      << gpr(instr.rS)
      << gpr_ofs(instr.rA, bits::signExtend<16>(instr.d), Operand::Read);

   return true;
}

/* stbu */
bool stbu(State *state, Instruction instr)
{
   state->result.code = "stbu";
   state->result.name = "Store Byte with Update";
   state->result.operands
      << gpr(instr.rS)
      << gpr_ofs(instr.rA, bits::signExtend<16>(instr.d), Operand::ReadWrite);

   return true;
}

/* std */
bool std(State *state, Instruction instr)
{
   state->result.code = "std";
   state->result.name = "Store Double Word";
   state->result.operands
      << gpr(instr.rS)
      << gpr_ofs(instr.rA, bits::signExtend<16>(static_cast<uint64_t>(instr.ds) << 2), Operand::Read);

   return true;
}

/* stdu */
bool stdu(State *state, Instruction instr)
{
   state->result.code = "stdu";
   state->result.name = "Store Double Word with Update";
   state->result.operands
      << gpr(instr.rS)
      << gpr_ofs(instr.rA, bits::signExtend<16>(static_cast<uint64_t>(instr.ds) << 2), Operand::ReadWrite);
   
   return true;
}

/* sth */
bool sth(State *state, Instruction instr)
{
   state->result.code = "sth";
   state->result.name = "Store Halfword";
   state->result.operands
      << gpr(instr.rS)
      << gpr_ofs(instr.rA, bits::signExtend<16>(instr.d), Operand::Read);

   return true;
}

/* sthu */
bool sthu(State *state, Instruction instr)
{
   state->result.code = "sthu";
   state->result.name = "Store Halfword with Update";
   state->result.operands
      << gpr(instr.rS)
      << gpr_ofs(instr.rA, bits::signExtend<16>(instr.d), Operand::ReadWrite);

   return true;
}

/* stw */
bool stw(State *state, Instruction instr)
{
   state->result.code = "stw";
   state->result.name = "Store Word";
   state->result.operands
      << gpr(instr.rS)
      << gpr_ofs(instr.rA, bits::signExtend<16>(instr.d), Operand::Read);

   return true;
}

/* stwu */
bool stwu(State *state, Instruction instr)
{
   state->result.code = "stwu";
   state->result.name = "Store Word with Update";
   state->result.operands
      << gpr(instr.rS)
      << gpr_ofs(instr.rA, bits::signExtend<16>(instr.d), Operand::ReadWrite);

   return true;
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

#include "emugen_instr_table.cpp"

}
}
