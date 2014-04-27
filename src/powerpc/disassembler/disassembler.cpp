#include "disassembler.h"

#include "powerpc/instructions.h"
#include "powerpc/decoder.h"
#include "powerpc/cpu.h"

#include "common/log.h"

#include <sstream>
#include <iomanip>
#include <map>
#include <assert.h>

std::map<unsigned, std::string> g_sprNames {
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

std::string getSPRName(unsigned id)
{
   auto itr = g_sprNames.find(id);

   if (itr == g_sprNames.end()) {
      return "spr" + std::to_string(id);
   } else {
      return itr->second;
   }
}

Result::Arg disField(State *state, Instruction instr, ppc::Field field)
{
   Result::Arg arg;
   arg.type = Result::Invalid;

   switch (field) {
   case Fields::bd:
      arg.type = Result::Address;

      if (instr.aa) {
         arg.address = little_endian::signExtend<16>(static_cast<uint64_t>(instr.bd) << 2);
      } else {
         arg.address = state->cia + little_endian::signExtend<16>(static_cast<uint64_t>(instr.bd) << 2);
      }
      break;
   case Fields::crfD:
      arg.type = Result::Register;
      arg.text = "cr" + std::to_string(instr.crfD);
      break;
   case Fields::crfS:
      arg.type = Result::Register;
      arg.text = "cr" + std::to_string(instr.crfS);
      break;
   case Fields::d:
      arg.type = Result::ConstantSigned;
      arg.constantSigned = little_endian::signExtend<16, int64_t>(instr.d);
      break;
   case Fields::ds:
      arg.type = Result::ConstantSigned;
      arg.constantSigned = little_endian::signExtend<16>(static_cast<int64_t>(instr.ds) << 2);
      break;
   case Fields::l:
      arg.type = Result::Constant;
      arg.constant = instr.l;
      break;
   case Fields::frA:
      arg.type = Result::Register;
      arg.text = "f" + std::to_string(instr.frA);
      break;
   case Fields::frB:
      arg.type = Result::Register;
      arg.text = "f" + std::to_string(instr.frB);
      break;
   case Fields::frC:
      arg.type = Result::Register;
      arg.text = "f" + std::to_string(instr.frC);
      break;;
   case Fields::frD:
      arg.type = Result::Register;
      arg.text = "f" + std::to_string(instr.frD);
      break;
   case Fields::frS:
      arg.type = Result::Register;
      arg.text = "f" + std::to_string(instr.frS);
      break;
   case Fields::li:
      arg.type = Result::Address;

      if (instr.aa) {
         arg.address = little_endian::signExtend<26>(static_cast<uint64_t>(instr.li) << 2);
      } else {
         arg.address = state->cia + little_endian::signExtend<26>(static_cast<uint64_t>(instr.li) << 2);
      }
      break;
   case Fields::simm:
      arg.type = Result::ConstantSigned;
      arg.constantSigned = little_endian::signExtend<16, int64_t>(instr.simm);
      break;
   case Fields::rA:
      arg.type = Result::Register;
      arg.text = "r" + std::to_string(instr.rA);
      break;
   case Fields::rB:
      arg.type = Result::Register;
      arg.text = "r" + std::to_string(instr.rB);
      break;
   case Fields::rD:
      arg.type = Result::Register;
      arg.text = "r" + std::to_string(instr.rD);
      break;
   case Fields::rS:
      arg.type = Result::Register;
      arg.text = "r" + std::to_string(instr.rS);
      break;
   case Fields::spr:
      arg.type = Result::Register;
      arg.text = g_sprNames[(instr.spr >> 5) & 0x1f | (instr.spr & 0x1f)];
      break;
   case Fields::xo1:
   case Fields::xo2:
   case Fields::xo3:
   case Fields::xo4:
   case Fields::xo5:
   case Fields::xo6:
   case Fields::xo7:
   case Fields::xo8:
   case Fields::bi:
   case Fields::bo:
   case Fields::bh:
   case Fields::crbA:
   case Fields::crbB:
   case Fields::crbD:
   case Fields::crm:
   case Fields::uimm:
   case Fields::th:
   case Fields::sr:
   case Fields::tbr:
   case Fields::fm:
   case Fields::nb:
   case Fields::l15:
   case Fields::syncl:
   case Fields::tlbl:
   case Fields::sh:
   case Fields::shd04:
   case Fields::shd5:
   case Fields::mb:
   case Fields::mbd:
   case Fields::me:
   case Fields::med:
   case Fields::to:
   case Fields::aa:
   case Fields::lk:
   case Fields::oe:
   case Fields::rc:
      arg.type = Result::Constant;
      arg.constant = instr.get(field);
      break;
   default:
      assert(0 && "Unknown field in disField");
   }

   return arg;
}

std::string resArgToText(Result::Arg &arg)
{
   std::stringstream ss;

   switch (arg.type) {
   case Result::Address:
      ss << std::hex << "0x" << arg.address;
      return ss.str();
   case Result::Register:
      return arg.text;
   case Result::Constant:
      return std::to_string(arg.constant);
   case Result::ConstantSigned:
      return std::to_string(arg.constantSigned);
   }

   return std::string();
}

enum BO
{
   CtrValue = 1 << 1,
   NoCheckCtr = 1 << 2,
   CondValue = 1 << 3,
   NoCheckCond = 1 << 4
};

bool disBranchConditional(State *state, Instruction instr)
{
   auto bo = instr.bo;
   auto bi = instr.bi;

   auto ctrValue      = bo & (1 << 1);
   auto noDecrement   = bo & (1 << 2);
   auto condTrue      = bo & (1 << 3);
   auto unconditional = bo & (1 << 4);

   auto crf = bi >> 2;
   auto crb = bi & 0x3;

   auto a = 0;
   auto t = 0;

   std::string result = "b";

   if (!unconditional && noDecrement) {
      a = bo & 2;
      t = bo & 1;

      if (condTrue) {
         switch (crb) {
         case 0:
            result += "lt";
            break;
         case 1:
            result += "gt";
            break;
         case 2:
            result += "eq";
            break;
         case 3:
            result += "so";
            break;
         }
      } else {
         switch (crb) {
         case 0:
            result += "ge";
            break;
         case 1:
            result += "le";
            break;
         case 2:
            result += "neq";
            break;
         case 3:
            result += "ns";
            break;
         }
      }
   } else {
      if (noDecrement == 0) {
         result += "d";

         if (ctrValue == 0) {
            result += "nz";
         } else {
            result += "z";
         }
      }

      if (!unconditional) {
         if (condTrue) {
            result += "t";
         } else {
            result += "f";
         }
      }

      if (unconditional && noDecrement == 0) {
         a = bo & 8;
         t = bo & 1;
      }
   }

   if (ppc::isa<ppc::InstructionID::bclr>(instr)) {
      result += "lr";
   } else if (ppc::isa<ppc::InstructionID::bcctr>(instr)) {
      result += "ctr";
   }

   if (instr.lk) {
      result += "l";
   }

   if (ppc::isa<ppc::InstructionID::bc>(instr)) {
      if (instr.aa) {
         result += "a";
      }
   }

   if (a && !t) {
      result += "-";
   } else if (a && t) {
      result += "+";
   }

   state->result.name = result;
   return true;
}

bool decode(State *state, Instruction instr)
{
   auto id = ppc::decode(instr);

   if (id == InstructionID::Unknown) {
      return false;
   }

   auto &data = ppc::getInstruction(id);

   state->result.name = data.name;
   state->result.fullname = data.fullname;

   for (auto &out : data.outputs) {
      state->result.args.push_back(disField(state, instr, out));
   }

   for (auto &in : data.inputs) {
      state->result.args.push_back(disField(state, instr, in));
   }

   for (auto &mod : data.mods) {
      switch (mod) {
      case Fields::aa:
         if (instr.aa) {
            state->result.name += 'a';
         }
         break;
      case Fields::oe:
         if (instr.oe) {
            state->result.name += 'o';
         }
         break;
      case Fields::rc:
         if (instr.rc) {
            state->result.name += '.';
         }
         break;
      }
   }

   if (ppc::isa<ppc::InstructionID::bc>(instr) || ppc::isa<ppc::InstructionID::bclr>(instr) || ppc::isa<ppc::InstructionID::bcctr>(instr)) {
      disBranchConditional(state, instr);
   }

   state->result.disasm = state->result.name;

   for (auto &arg : state->result.args) {
      if (&arg == &state->result.args[0]) {
         state->result.disasm += " ";
      } else {
         state->result.disasm += ", ";
      }

      state->result.disasm += resArgToText(arg);
   }

   return true;
}

} // namespace Disassembler

} //namespace ppc
