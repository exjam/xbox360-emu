#ifndef TEST_H
#define TEST_H

#include <string>
#include <vector>
#include <stdint.h>
#include <stdexcept>
#include <stdarg.h>
#include <sstream>

#include "powerpc/interpreter/interpreter.h"
#include "common/bit_field.h"

class TestError
{
public:
   TestError(ppc::Interpreter::State &state, const char * msg, ...)
   {
      va_list args;

      m_message.resize(256);

      va_start(args, msg);
      vsprintf_s(const_cast<char*>(m_message.data()), 256, msg, args);
      va_end(args);

      memcpy(&m_state, &state, sizeof(state));
   }

   const char* what() const
   {
      return m_message.c_str();
   }

   std::string dumpState() const
   {
      std::stringstream out;

      for (int i = 0; i < 32; ++i) {
         out << "gpr[" << i << "] = 0x" << std::hex << m_state.reg.gpr[i];

         if (i < 31) {
            out << ", ";
         }
      }

      out << std::endl;

      for (int i = 0; i < 32; ++i) {
         out << "fpr[" << i << "] = " << std::dec << m_state.reg.fpr[i];

         if (i < 31) {
            out << ", ";
         }
      }

      out << std::endl;

      return out.str();
   }

private:
   std::string m_message;
   ppc::Interpreter::State m_state;
};

class Test
{
public:
   enum class ConditionType
   {
      SetRegister,
      SetMemory,
      CheckRegister,
      CheckMemory
   };

   enum class ValueType
   {
      Register,
      Immediate,
      Address
   };

   struct Value
   {
      ValueType type;
      std::string name;
      uint64_t value;
   };

   struct Condition
   {
      ConditionType type;
      Value target;
      Value value;
   };

   enum class RegisterType
   {
      Unknown,
      Integer32,
      Integer64,
      Float32,
      Double64,
      BitField32,
      BitField64,
   };

   struct Register
   {
      Register() :
      type(RegisterType::Unknown)
      {
      }

      template<typename PtrType>
      Register(RegisterType type, PtrType *ptr) :
         type(type), ptr(ptr)
      {
      }

      template<typename ValType>
      Register(RegisterType type, dyn_bit_field_t<ValType> &val) :
         type(type)
      {
         new (buf) dyn_bit_field_t<ValType>(val);
      }

      RegisterType type;
      void *ptr;
      char buf[32];
   };

public:
   Test();
   ~Test();

   bool load(const std::string &path);
   bool run();

private:
   bool checkConditions(ppc::Interpreter::State &state, std::vector<Condition> &conds);

   bool setRegister(ppc::Interpreter::State &state, const std::string &name, Value &value);
   bool checkRegister(ppc::Interpreter::State &state, const std::string &name, Value &value);

   bool setMemory(ppc::Interpreter::State &state, uint64_t addr, Value &value);
   bool checkMemory(ppc::Interpreter::State &state, uint64_t addr, Value &value);

   uint64_t translateAddress(uint64_t addr);
   bool getValue(Test::Value &in, int64_t &out);
   Register getRegister(ppc::Interpreter::State &state, const std::string &name);

private:
   uint8_t *m_code;
   uint64_t m_codeSize;

   std::vector<Condition> m_preconditions;
   std::vector<Condition> m_postconditions;
};

#endif
