#include "emugen.h"

#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <assert.h>
#include <algorithm>
#include <functional>

EmuGen::EmuGen()
{
}

EmuGen::~EmuGen()
{
}

bool EmuGen::run(const std::string &definition, const std::string &outDirectory)
{
   if (!parseFile(definition)) {
      return false;
   }

   /* Create List */
   for (auto &group : m_ast.opcodes) {
      for (auto &instr : group.opcodes) {
         instr.category = &group.declaration;
         m_instructionList[instr.disasm.name] = &instr;
      }
   }

   /* Create Tree */
   for (auto &instr : m_instructionList) {
      std::string xo1 = instr.second->category->primary;
      auto *table = &m_rootTable.table[xo1];

      for (auto &cat : instr.second->category->secondary) {
         auto next = table->find(cat.first);
         DecodeTable *nextTable;

         if (next == table->end()) {
            nextTable = new DecodeTable();
            table->insert(std::make_pair(cat.first, nextTable));
         } else {
            assert(next->second->type == DecodeNode::Table);
            nextTable = reinterpret_cast<DecodeTable*>(next->second);
         }

         table = &nextTable->table[cat.second];
      }

      assert(table->find(instr.second->id) == table->end());

      DecodeInstr *decodeInstr = new DecodeInstr();
      decodeInstr->op = instr.second;
      table->insert(std::make_pair(instr.second->id, decodeInstr));
   }

   createCpuInfo(outDirectory + "/emugen_cpu_info.h");

   createInstructionList(outDirectory + "/emugen_instructionid.h");
   createInstructionTableSource(outDirectory + "/emugen_table.h");

   createStubs(outDirectory + "/emugen_stubs.cpp");
   createStubsHeader(outDirectory + "/emugen_stubs.h");

   createInstructionTable(outDirectory + "/emugen_instructions.h");
   createDecoder(outDirectory + "/emugen_decoder.h");

   return true;
}

template <class T>
int numDigits(T number)
{
   int digits = 0;

   while (number) {
      number /= 10;
      digits++;
   }

   return digits;
}

bool EmuGen::createDecoder(const std::string &path)
{
   std::fstream out;
   std::vector<std::string> tables;

   out.open(path, std::ifstream::out);

   if (!out.is_open()) {
      std::cout << "Could not open " << path << " for writing" << std::endl;
      return false;
   }
   
   std::function<void(DecodeTable*, std::string name)> printDecodeTables =
      [&](DecodeTable* table, std::string name) {
         std::string first;

         for (auto &entry : table->table) {
            std::stringstream tableName;
            tableName << name << "_" << entry.first;

            auto field = findInstructionField(entry.first);

            if (first.length()) {
               out << "DecodeEntry *";
            } else {
               out << "DecodeEntry ";
            }

            out << "table" << tableName.str();

            if (first.length()) {
               out << " = table" << first << ";";
            } else {
               out << "[" << (1 << field->bitrange.size()) << "];";
            }

            out << std::endl;
            first = tableName.str();
         }

         for (auto &entry : table->table) {
            for (auto &child : entry.second) {
               if (child.second->type == DecodeNode::Table) {
                  std::stringstream tableName;
                  tableName << name << "_" << entry.first << "_" << child.first;
                  printDecodeTables(reinterpret_cast<DecodeTable*>(child.second), tableName.str());
               }
            }
         }
      };

   printDecodeTables(&m_rootTable, "");
   out << std::endl;

   std::function<void(DecodeTable*, std::string name)> printDecodeFunction =
      [&out, &printDecodeFunction](DecodeTable* table, std::string name) {
         out << "InstructionID decode" << name << "(Instruction instr) {" << std::endl;
         out << "   " << "DecodeEntry entry;" << std::endl;
         out << std::endl;

         for (auto &entry : table->table) {
            std::stringstream tableName;
            tableName << name << "_" << entry.first;

            out << "   " << "entry = table" << tableName.str() << "[instr." << entry.first << "];" << std::endl;
            out << "   " << "if (entry.value) {" << std::endl;
            out << "   " << "   " << "if (entry.isTable()) {" << std::endl;
            out << "   " << "   " << "   " << "return entry.table(instr);" << std::endl;
            out << "   " << "   " << "} else {" << std::endl;
            out << "   " << "   " << "   " << "return entry.instr();" << std::endl;
            out << "   " << "   " << "}" << std::endl;
            out << "   " << "}" << std::endl;
            out << std::endl;
         }

         out << "   " << "return InstructionID::Unknown;" << std::endl;
         out << "}" << std::endl;
         out << std::endl;

         for (auto &entry : table->table) {
            for (auto &child : entry.second) {
               if (child.second->type == DecodeNode::Table) {
                  std::stringstream tableName;
                  tableName << name << "_" << entry.first << "_" << child.first;
                  printDecodeFunction(reinterpret_cast<DecodeTable*>(child.second), tableName.str());
               }
            }
         }
      };

   printDecodeFunction(&m_rootTable, "");
   out << std::endl;

   std::function<void(DecodeTable*, std::string name)> printDecodeInit =
      [&out, &printDecodeInit](DecodeTable* table, std::string name) {
         for (auto &entry : table->table) {
            std::stringstream tableName;
            tableName << name << "_" << entry.first;

            for (auto &child : entry.second) {
               out << "   " << "table" << tableName.str();
               out << "[" << child.first << "] = ";

               if (child.second->type == DecodeNode::Instruction) {
                  DecodeInstr *instr = reinterpret_cast<DecodeInstr*>(child.second);
                  out << "InstructionID::" << getSafeFunctionName(instr->op->disasm.name);
               } else if (child.second->type == DecodeNode::Table) {
                  out << "decode" << tableName.str() << "_" << child.first;
               }

               out << ";" << std::endl;
            }
         }

         for (auto &entry : table->table) {
            for (auto &child : entry.second) {
               if (child.second->type == DecodeNode::Table) {
                  std::stringstream tableName;
                  tableName << name << "_" << entry.first << "_" << child.first;
                  printDecodeInit(reinterpret_cast<DecodeTable*>(child.second), tableName.str());
               }
            }
         }
      };

   out << "bool init()" << std::endl;
   out << "{" << std::endl;
   printDecodeInit(&m_rootTable, "");
   out << "   " << "return true;" << std::endl;
   out << "}" << std::endl;
   
   out.close();
   std::cout << "EmuGen: " << path << std::endl;
   return true;
}

bool EmuGen::createInstructionTable(const std::string &path)
{
   std::fstream out;
   out.open(path, std::ifstream::out);

   if (!out.is_open()) {
      std::cout << "Could not open " << path << " for writing" << std::endl;
      return false;
   }

   std::size_t nameLen = 0;
   std::size_t opcdLen = 0;
   std::size_t fieldsLen = 0;
   std::vector<std::size_t> opnumLens;
   opnumLens.push_back(0);

   /* Get some lengths for formatting stuff */
   for (auto &instr : m_instructionList) {
      nameLen = std::max(nameLen, getSafeFunctionName(instr.first).length());
      opcdLen = std::max(opcdLen, 1 + instr.second->category->secondary.size());

      for (auto i = opnumLens.size(); i < opcdLen; ++i) {
         opnumLens.push_back(0);
      }

      std::size_t i = 0;

      for (; i < instr.second->category->secondary.size(); ++i) {
         std::size_t digits = numDigits(instr.second->category->secondary[i].first);
         opnumLens[i] = std::max(opnumLens[i], digits);
      }

      opnumLens[i] = std::max(opnumLens[i], static_cast<std::size_t>(numDigits(instr.second->id)));

      std::size_t readCount = 0, writeCount = 0, modCount = 0;
      std::size_t readLen = 0, writeLen = 0, modLen = 0;

      for (auto &opr : instr.second->disasm.operands) {
         if (opr.prefix == '+') {
            writeCount++;
            writeLen += opr.name.length();
         } else {
            readCount++;
            readLen += opr.name.length();
         }
      }

      for (auto &extra : instr.second->extras) {
         auto found = std::find_if(m_ast.insf.fields.begin(), m_ast.insf.fields.end(),
                                   [&extra](ast_insf_field& field) {
            return field.name.compare(extra.first) == 0;
         });

         if (found == m_ast.insf.fields.end()) {
            continue;
         }

         modCount++;
         modLen += extra.first.length();
      }

      if (readCount) {
         readLen += 1 + (readCount - 1) * 2;
      }

      if (writeCount) {
         writeLen += 1 + (writeCount - 1) * 2;
      }

      if (modCount) {
         modLen += 1 + (modCount - 1) * 2;
      }

      instr.second->fieldsLen = readLen + writeLen + modLen;
      fieldsLen = std::max(fieldsLen, instr.second->fieldsLen);
   }

   out.fill(' ');

   /* Enum is 1 index based */
   out << "InstrEntry Instructions[] = {" << std::endl;
   out << "   { }," << std::endl;

   for (auto &instr : m_instructionList) {
      std::string name = getSafeFunctionName(instr.first);

      out << "   { ";

      /* Instruction Name */
      out << '"' << name << '"' << ", ";

      for (std::size_t i = name.length(); i < nameLen; ++i) {
         out << ' ';
      }

      /* Opcode List { xo, 31 }, { xo2, 28 } */
      {
         std::size_t opnum = 0;

         out << "{ ";

         out << "{ " << instr.second->category->primary;

         for (auto &cat : instr.second->category->secondary) {
            out << ", ";

            out.setf(std::ios::adjustfield, std::ios::right);
            out.width(opnumLens[opnum++]);

            out << cat.first;

            out.width(0);
            out.unsetf(std::ios::adjustfield);

            out << " }, { " << cat.second;
         }

         out << ", ";

         out.setf(std::ios::adjustfield, std::ios::right);
         out.width(opnumLens[opnum]);

         out << instr.second->id;

         out.width(0);
         out.unsetf(std::ios::adjustfield);

         out << " }";

         for (std::size_t i = 1 + instr.second->category->secondary.size(); i < opcdLen; ++i) {
            for (std::size_t j = 0; j < 11 + opnumLens[i]; ++j) {
               out << ' ';
            }
         }

         out << " }, ";
      }

      /* Output Fields */
      {
         int count = 0;

         out << "{ ";

         for (auto &opr : instr.second->disasm.operands) {
            if (opr.prefix != '+') {
               continue;
            }

            if (count != 0) {
               out << ", ";
            }

            out << opr.name;
            ++count;
         }

         if (count > 0) {
            out << ' ';
         }

         out << "}, ";
      }

      /* Input Fields */
      {
         int count = 0;

         out << "{ ";

         for (auto &opr : instr.second->disasm.operands) {
            if (opr.prefix == '+') {
               continue;
            }

            if (count != 0) {
               out << ", ";
            }

            out << opr.name;
            ++count;
         }

         if (count > 0) {
            out << ' ';
         }

         out << "}, ";
      }

      /* Modifiers */
      {
         int count = 0;

         out << "{ ";

         for (auto &extra : instr.second->extras) {
            auto found = std::find_if(m_ast.insf.fields.begin(), m_ast.insf.fields.end(),
                                   [&extra](ast_insf_field& field) {
                                      return field.name.compare(extra.first) == 0;
                                   });

            if (found == m_ast.insf.fields.end()) {
               continue;
            }

            if (count != 0) {
               out << ", ";
            }

            out << extra.first;
            ++count;
         }

         if (count > 0) {
            out << ' ';
         }

         out << "}, ";
      }

      for (auto i = instr.second->fieldsLen; i < fieldsLen; ++i) {
         out << ' ';
      }

      /* Fullname */
      {
         out << '"' << *instr.second->extras["fullname"].string << '"';
      }

      out << " }," << std::endl;
   }
   out << "};" << std::endl;

   out.close();
   std::cout << "EmuGen: " << path << std::endl;
   return true;
}

bool EmuGen::createCpuInfo(const std::string &path)
{
   std::fstream out;
   out.open(path, std::ifstream::out);

   if (!out.is_open()) {
      std::cout << "Could not open " << path << " for writing" << std::endl;
      return false;
   }

   out << "struct Registers_ {" << std::endl;

   for (auto &reg : m_ast.reg.registers) {
      if (reg.type.bitfield) {
         int pos = 0;

         out << "   union {" << std::endl;
         out << "      " << reg.type.type << " value;" << std::endl;
         out << "      struct {" << std::endl;

         for (auto &field : reg.type.bitfield->bitfield) {
            int bits = 1 + field.second.end - field.second.start;

            if (pos != field.second.start) {
               out << "         " << reg.type.type << " : " << (field.second.start - pos) << ";" << std::endl;
               pos = field.second.start;
            }

            pos += bits;
            out << "         " << reg.type.type << " " << field.first << " : " << bits << ";" << std::endl;
         }

         out << "      };" << std::endl;
         out << "   } " << reg.name << ";" << std::endl;
      } else {
         out << "   " << reg.type.type << " " << reg.name;

         if (reg.type.array) {
            out << "[" << reg.type.array->size << "]";
         }

         out << ";" << std::endl;
      }
   }

   out << "};" << std::endl;
   out << std::endl;

   out << "namespace Fields {" << std::endl;
   out << "enum Field {" << std::endl;

   for (auto &field : m_ast.insf.fields) {
      out << "   " << field.name << "," << std::endl;
   }

   out << "};" << std::endl;
   out << "};" << std::endl;
   out << std::endl;
   out << "typedef unsigned Field;" << std::endl;
   out << std::endl;


   out << "union Instruction {" << std::endl;
   out << "   uint32_t value;" << std::endl;
   out << std::endl;

   out << "   unsigned get(Field field) const;" << std::endl;
   out << "   void set(Field field, unsigned value);" << std::endl;
   out << std::endl;

   for (auto &field : m_ast.insf.fields) {
      int start = field.bitrange.start;
      int end = field.bitrange.end;

      if (m_ast.arch.endian == ast_arch::BigEndian) {
         start = 31 - start;
         end   = 31 - end;
         std::swap(start, end);
      }
      
      out << std::endl;

      out << "   struct {" << std::endl;

      if (start != 0) {
         out << "      uint32_t : " << start << ";" << std::endl;
      }

      out << "      uint32_t " << field.name << " : " << (1 + end - start) << ";" << std::endl;

      if (end != 31) {
         out << "      uint32_t : " << (31 - end) << ";" << std::endl;
      }

      out << "   };" << std::endl;
   }

   out << "};" << std::endl;
   out << std::endl;

   out.close();
   std::cout << "EmuGen: " << path << std::endl;
   return true;
}

bool EmuGen::createStubs(const std::string &path)
{
   std::fstream out;
   out.open(path, std::ifstream::out);

   if (!out.is_open()) {
      std::cout << "Could not open " << path << " for writing" << std::endl;
      return false;
   }
   
   for (auto &instr : m_instructionList) {
      auto &fullname = *instr.second->extras["fullname"].string;

      out << "/* " << fullname << " */" << std::endl;
      out << "bool " << getSafeFunctionName(instr.first) << "(State *state, Instruction instr)" << std::endl;
      out << "{" << std::endl;
      out << "   return false;" << std::endl;
      out << "}" << std::endl << std::endl;
   }

   out.close();
   std::cout << "EmuGen: " << path << std::endl;
   return true;
}

bool EmuGen::createInstructionList(const std::string &path)
{
   std::fstream out;
   out.open(path, std::ifstream::out);
   
   if (!out.is_open()) {
      std::cout << "Could not open " << path << " for writing" << std::endl;
      return false;
   }

   out << "enum class InstructionID {" << std::endl;

   out << "   Unknown," << std::endl;

   for (auto &instr : m_instructionList) {
      out << "   " << getSafeFunctionName(instr.first) << "," << std::endl;
   }

   out << "   InstructionMax," << std::endl;

   out << "};" << std::endl;

   out.close();
   std::cout << "EmuGen: " << path << std::endl;
   return true;
}

bool EmuGen::createStubsHeader(const std::string &path)
{
   std::fstream out;
   out.open(path, std::ifstream::out);

   if (!out.is_open()) {
      std::cout << "Could not open " << path << " for writing" << std::endl;
      return false;
   }

   out << std::endl;

   for (auto &instr : m_instructionList) {
      out << "bool " << getSafeFunctionName(instr.first) << "(State *state, Instruction instr);" << std::endl;
   }

   out.close();
   std::cout << "EmuGen: " << path << std::endl;
   return true;
}

bool EmuGen::createInstructionTableSource(const std::string &path)
{
   std::fstream out;
   out.open(path, std::ifstream::out);

   if (!out.is_open()) {
      std::cout << "Could not open " << path << " for writing" << std::endl;
      return false;
   }


   out << "fptr_t _emugenTable[] = {" << std::endl;
   out << "   nullptr," << std::endl;

   for (auto &instr : m_instructionList) {
      out << "   " << getSafeFunctionName(instr.first) << "," << std::endl;
   }

   out << "};" << std::endl;

   out.close();
   std::cout << "EmuGen: " << path << std::endl;
   return true;
}

std::string EmuGen::getSafeFunctionName(const std::string &name)
{
   std::string result = name;
   std::replace(result.begin(), result.end(), '.', 'o');
   return result;
}

ast_insf_field *EmuGen::findInstructionField(std::string name)
{
   if (name[0] == '(') {
      name.erase(name.begin());
      name.erase(name.end() - 1);
   }

   for (auto &field : m_ast.insf.fields) {
      if (name.compare(field.name) == 0) {
         return &field;
      }
   }

   return nullptr;
}
