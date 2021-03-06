#ifndef EMUGEN_TYPES_H
#define EMUGEN_TYPES_H

#include <vector>

namespace emugen
{

struct InstrEntry
{
   struct Opcode
   {
      Field field;
      unsigned value;
   };

   const char *name;
   std::vector<Opcode> opcodes;
   std::vector<Field> outputs;
   std::vector<Field> inputs;
   std::vector<Field> mods;
   const char *fullname;
};

struct DecodeEntry
{
   using fptr_t = InstructionID(*)(Instruction instr);

   DecodeEntry &operator=(fptr_t decoder)
   {
      value = reinterpret_cast<std::size_t>(decoder);
      return *this;
   }

   DecodeEntry &operator=(InstructionID instr)
   {
      value = static_cast<std::size_t>(instr);
      return *this;
   }

   bool isTable()
   {
      return value > static_cast<std::size_t>(InstructionID::InstructionMax);
   }

   InstructionID table(Instruction instr)
   {
      return reinterpret_cast<fptr_t>(value)(instr);
   }

   InstructionID instr()
   {
      return static_cast<InstructionID>(value);
   }

   std::size_t value;
};

}; // namespace emugen

#endif // ifndef EMUGEN_TYPES_H
