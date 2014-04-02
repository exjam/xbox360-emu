#include "test.h"

#include "elf.h"
#include "xenon.h"
#include "parser.h"
#include "tester.h"

#include <ppc/interpreter.h>
#include <util/memory.h>
#include <util/bits.h>
#include <regex>

Test::Test() :
   m_code(nullptr), m_codeSize(0)
{
}

Test::~Test()
{
   delete[] m_code;
}

/* Load test from file */
bool Test::load(const std::string &path)
{
   ast_test astTest;
   std::string tmpElf;

   tmpElf = g_tester.getTestRoot() + "\\tmp.elf";

   /* Assemble test to elf */
   if (!xenon::assemble(path, tmpElf)) {
      return false;
   }

   /* Load code from elf for execution */
   if (!elf::load(tmpElf, &m_codeSize, &m_code)) {
      return false;
   }

   /* Parse test file for pre&post conditions */
   if (!parseTest(path, astTest)) {
      return false;
   }

   /* Parse Pre-Conditions */
   for (auto &astPre : astTest.pre) {
      Condition cond;

      if (astPre.reg_set) {
         cond.type = ConditionType::SetRegister;

         cond.target.type = ValueType::Register;
         cond.target.name = astPre.reg_set->reg_name;

         if (astPre.reg_set->mem_addr) {
            cond.value.type = ValueType::Address;
            cond.value.value = *astPre.reg_set->mem_addr;
         } else if (astPre.reg_set->reg_value) {
            cond.value.type = ValueType::Immediate;
            cond.value.value = *astPre.reg_set->reg_value;
         }
      } else if (astPre.mem_set) {
         cond.type = ConditionType::SetMemory;

         cond.target.type = ValueType::Address;
         cond.target.value = astPre.mem_set->mem_addr;

         cond.value.type = ValueType::Immediate;
         cond.value.value = astPre.mem_set->mem_value;
      }

      m_preconditions.push_back(cond);
   }

   /* Parse Post-Conditions */
   for (auto &astPost : astTest.post) {
      Condition cond;

      if (astPost.reg_check) {
         cond.type = ConditionType::CheckRegister;

         cond.target.type = ValueType::Register;
         cond.target.name = astPost.reg_check->reg_name;

         if (astPost.reg_check->mem_addr) {
            cond.value.type = ValueType::Address;
            cond.value.value = *astPost.reg_check->mem_addr;
         } else if (astPost.reg_check->reg_value) {
            cond.value.type = ValueType::Immediate;
            cond.value.value = *astPost.reg_check->reg_value;
         }
      }

      m_postconditions.push_back(cond);
   }

   return true;
}

/* Run interpreter on a test */
bool Test::run()
{
   ppc::Interpreter::State state;
   memset(&state, 0, sizeof(state));

   state.cia = 0;
   state.nia = state.cia + 4;
   state.reg.lr = m_codeSize;

   if (!checkConditions(state, m_preconditions)) {
      return false;
   }
   
   while (state.cia < m_codeSize) {
      ppc::Instruction instr;

      instr.value = Memory::read<uint32_t>(reinterpret_cast<uint64_t>(m_code + state.cia));
      ppc::Interpreter::decode(&state, instr);

      state.cia = state.nia;
      state.nia = state.cia + 4;
   }

   if (!checkConditions(state, m_postconditions)) {
      return false;
   }

   return true;
}

/* Translate address */
uint64_t Test::translateAddress(uint64_t addr)
{
   auto base = g_tester.getTestMemoryAddress();

   if (addr >= base && addr < base + 0x100) {
      return addr - base + 0x1000;
   } else {
      return addr + base - 0x1000;
   }
}

/* Get value */
bool Test::getValue(Test::Value &in, int64_t &out)
{
   switch (in.type) {
   case Test::ValueType::Immediate:
      out = in.value;
      return true;
   case Test::ValueType::Address:
      out = translateAddress(in.value);
      return true;
   }

   return false;
}

/* Get register by name */
Test::Register Test::getRegister(ppc::Interpreter::State &state, const std::string &name)
{
   std::smatch smatch;

   /* General Purpose Register */
   std::regex gpr("%r([0-9]+)");

   if (std::regex_match(name, smatch, gpr) && smatch.size() == 2) {
      auto reg = std::stol(smatch[1].str());

      if (reg >= 0 && reg <= 31) {
         return Register(RegisterType::Integer64, &state.reg.gpr[reg]);
      }
   }

   /* FPU Register */
   std::regex fpr("%f([0-9]+)");

   if (std::regex_match(name, smatch, fpr) && smatch.size() == 2) {
      auto reg = std::stol(smatch[1].str());

      if (reg >= 0 && reg <= 31) {
         return Register(RegisterType::Double64, &state.reg.fpr[reg]);
      }
   }

   /* XER Link Register */
   if (name.compare("%lr") == 0) {
      return Register(RegisterType::Integer64, &state.reg.lr);
   }

   /* XER Carry */
   if (name.compare("%xer[ca]") == 0) {
      return Register(RegisterType::BitField64, bits::field<uint64_t>(state.reg.xer.value, 34, 1));
   }

   /* XER Overflow */
   if (name.compare("%xer[ov]") == 0) {
      return Register(RegisterType::BitField64, bits::field<uint64_t>(state.reg.xer.value, 33, 1));
   }

   /* Condition Register Field */
   std::regex crf("%crf([0-9]+)");

   if (std::regex_match(name, smatch, crf) && smatch.size() == 2) {
      auto field = std::stol(smatch[1].str());

      if (field >= 0 && field <= 8) {
         return Register(RegisterType::BitField32, bits::field<uint32_t>(state.reg.cr.value, 4 * field, 4));
      }
   }

   /* Condition Register Bit */
   std::regex crb("%crb([0-9]+)");

   if (std::regex_match(name, smatch, crb) && smatch.size() == 2) {
      auto bit = std::stol(smatch[1].str());

      if (bit >= 0 && bit <= 31) {
         return Register(RegisterType::BitField32, bits::field<uint32_t>(state.reg.cr.value, bit, 1));
      }
   }

   return Register();
}

/* Set register to value */
bool Test::setRegister(ppc::Interpreter::State &state, const std::string &name, Test::Value &val)
{
   int64_t value;
   Register reg;

   if (!getValue(val, value)) {
      return false;
   }

   reg = getRegister(state, name);

   switch (reg.type) {
   case RegisterType::Integer32:
      *reinterpret_cast<uint32_t*>(reg.ptr) = static_cast<uint32_t>(value);
      break;
   case RegisterType::Integer64:
      *reinterpret_cast<uint64_t*>(reg.ptr) = static_cast<uint64_t>(value);
      break;
   case RegisterType::Float32:
      *reinterpret_cast<float*>(reg.ptr) = static_cast<float>(value);
      break;
   case RegisterType::Double64:
      *reinterpret_cast<double*>(reg.ptr) = static_cast<double>(value);
      break;
   case RegisterType::BitField32:
      *reinterpret_cast<bits::field<uint32_t>*>(reg.buf) = static_cast<uint32_t>(value);
      break;
   case RegisterType::BitField64:
      *reinterpret_cast<bits::field<uint64_t>*>(reg.buf) = static_cast<uint64_t>(value);
      break;
   default:
      return false;
   }

   return true;
}

/* Check register for value */
bool Test::checkRegister(ppc::Interpreter::State &state, const std::string &name, Test::Value &val)
{
   int64_t value;
   Register reg;

   if (!getValue(val, value)) {
      return false;
   }

   reg = getRegister(state, name);

   switch (reg.type) {
   case RegisterType::Integer32:
      return *reinterpret_cast<uint32_t*>(reg.ptr) == static_cast<uint32_t>(value);

   case RegisterType::Integer64:
      return *reinterpret_cast<uint64_t*>(reg.ptr) == static_cast<uint64_t>(value);

   case RegisterType::Float32:
      return *reinterpret_cast<float*>(reg.ptr) == static_cast<float>(value);

   case RegisterType::Double64:
      return *reinterpret_cast<double*>(reg.ptr) == static_cast<double>(value);

   case RegisterType::BitField32:
      return *reinterpret_cast<bits::field<uint32_t>*>(reg.buf) == static_cast<uint32_t>(value);

   case RegisterType::BitField64:
      return *reinterpret_cast<bits::field<uint64_t>*>(reg.buf) == static_cast<uint64_t>(value);
   }

   return false;
}

/* Set memory to value */
bool Test::setMemory(ppc::Interpreter::State &state, uint64_t dst, Test::Value &val)
{
   int64_t value;
   auto addr = translateAddress(dst);

   if (!addr || !getValue(val, value)) {
      return false;
   }

   Memory::write(addr, value);
   return true;
}

/* Check memory for value */
bool Test::checkMemory(ppc::Interpreter::State &state, uint64_t src, Test::Value &val)
{
   int64_t value;
   auto addr = translateAddress(src);

   if (!addr || !getValue(val, value)) {
      return false;
   }

   return value == Memory::read<uint64_t>(addr);
}

/* Ensure a Condition is met */
bool Test::checkConditions(ppc::Interpreter::State &state, std::vector<Test::Condition> &conds)
{
   for (auto &cond : conds) {
      switch (cond.type) {
      case ConditionType::SetRegister:
         if (cond.target.type != ValueType::Register) {
            return false;
         }

         if (!setRegister(state, cond.target.name, cond.value)) {
            throw new TestError(state, "Could not set register %s", cond.target.name.c_str());
         }

         break;

      case ConditionType::CheckRegister:
         if (cond.target.type != ValueType::Register) {
            return false;
         }

         if (!checkRegister(state, cond.target.name, cond.value)) {
            throw new TestError(state, "Incorrect value in register %s", cond.target.name.c_str());
         }

         break;

      case ConditionType::SetMemory:
         if (cond.target.type != ValueType::Address) {
            return false;
         }

         if (!setMemory(state, cond.target.value, cond.value)) {
            throw new TestError(state, "Could not set memory at 0x%X", translateAddress(cond.target.value));
         }

         break;

      case ConditionType::CheckMemory:
         if (cond.target.type != ValueType::Address) {
            return false;
         }

         if (!checkMemory(state, cond.target.value, cond.value)) {
            throw new TestError(state, "Incorrect value at memory location 0x%X", translateAddress(cond.target.value));
         }

         break;
      }
   }

   return true;
}
