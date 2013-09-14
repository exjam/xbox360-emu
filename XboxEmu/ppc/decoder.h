#ifndef PPC_DECODER_H
#define PPC_DECODER_H

#include "ppc/cpu.h"
#include "ppc/disassembler.h"
#include "ppc/interpreter.h"

#include "util/log.h"

namespace ppc
{

template<int _Bits>
struct InstructionTable
{
   static const int Bits = _Bits;

   InstructionTable()
   {
      memset(this, 0, sizeof(InstructionTable<Bits>));
   }

   Disassembler::fptr disassemble[1 << (_Bits - 1)];
   Interpreter::fptr interpret[1 << (_Bits - 1)];
};

#define InstrHandler(Name, Namespace, Selector) \
   static inline bool Name##(Namespace##::State *state, Instruction instr) { \
      if (m_table.##Name##[Selector]) { \
         m_table.##Name##[Selector](state, instr); \
         return true; \
      } else { \
         xDebug() << "Unimplemented " << #Name << " " << #Selector << " = " << Selector; \
         return false; \
      } \
   }

#define AddInstr(id, name) \
   m_table.interpret[id] = Interpreter::##name; \
   m_table.disassemble[id] = Disassembler::##name;

#define AddInstrAlias(orig, id, name) \
   m_table.interpret[id] = Interpreter::##orig; \
   m_table.disassemble[id] = Disassembler::##orig;

#define AddTable(id) \
   Table##id##::init(); \
   m_table.interpret[id] = Table##id##::interpret; \
   m_table.disassemble[id] = Table##id##::disassemble;

struct Table19
{
   static void init()
   {
      #include "table19.inc"
   }

   InstrHandler(disassemble, Disassembler, instr.subop10);
   InstrHandler(interpret, Interpreter, instr.subop10);
   static InstructionTable<10> m_table;
};

struct Table31
{
   static void init()
   {
      #include "table31.inc"
   }

   InstrHandler(disassemble, Disassembler, instr.subop10);
   InstrHandler(interpret, Interpreter, instr.subop10);
   static InstructionTable<10> m_table;
};

class Decoder
{
public:
   static void init()
   {
      #include "tablePrimary.inc"
   }

   InstrHandler(disassemble, Disassembler, instr.opcode);
   InstrHandler(interpret, Interpreter, instr.opcode);

private:
   static InstructionTable<6> m_table;
};

#undef InstrHandler
#undef AddInstr
#undef AddTable

} // namespace ppc

#endif // PPC_DECODER_H
