#include "ppc/instructions.h"
#include <assert.h>

using ppc::emugen::InstrEntry;
using namespace ppc::FieldID;

namespace ppc
{
#include "emugen_instructions.h"

Instruction setField(Instruction instr, FieldID::Fields field, unsigned value)
{
   switch (field) {
   case FieldID::xo1:
      instr.xo1 = value;
      break;
   case FieldID::xo2:
      instr.xo2 = value;
      break;
   case FieldID::xo3:
      instr.xo3 = value;
      break;
   case FieldID::xo4:
      instr.xo4 = value;
      break;
   case FieldID::xo5:
      instr.xo5 = value;
      break;
   case FieldID::xo6:
      instr.xo6 = value;
      break;
   case FieldID::xo7:
      instr.xo7 = value;
      break;
   case FieldID::xo8:
      instr.xo8 = value;
      break;
   default:
      assert(false && "Unimplemented setField");
   }

   return instr;
}

unsigned getField(Instruction instr, FieldID::Fields field)
{
   switch (field) {
   case FieldID::xo1:
      return instr.xo1;
   case FieldID::xo2:
      return instr.xo2;
   case FieldID::xo3:
      return instr.xo3;
   case FieldID::xo4:
      return instr.xo4;
   case FieldID::xo5:
      return instr.xo5;
   case FieldID::xo6:
      return instr.xo6;
   case FieldID::xo7:
      return instr.xo7;
   case FieldID::xo8:
      return instr.xo8;
   default:
      assert(0 && "Unimplemented getField");
      return 0;
   }
}

Instruction createInstruction(InstructionID id)
{
   Instruction instr { 0 };

   for (auto &op : ppc::Instructions[static_cast<unsigned>(id)].opcodes) {
      instr = setField(instr, op.field, op.value);
   }

   return instr;
}

const ppc::emugen::InstrEntry &getInstruction(InstructionID id)
{
   return Instructions[static_cast<unsigned>(id)];
}
};

