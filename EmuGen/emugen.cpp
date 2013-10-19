#include "emugen.h"

#include <map>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <functional>

struct Node {
   int id;
   ast_insf_field *field;
   std::multimap<int, Node *> children;
   std::map<int, ast_opcd_def *> instructions;
};

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

   createInstructionList(outDirectory + "/emugen_instr_list.h");
   createInstructionTableHeader(outDirectory + "/emugen_instr_table.h");
   createInstructionTableSource(outDirectory + "/emugen_instr_table.cpp");

   createDisassembler(outDirectory + "/emugen_instr_disasm.cpp");

   return true;
}

bool EmuGen::createDisassembler(const std::string &path)
{
   std::fstream out;
   out.open(path, std::ifstream::out);

   if (!out.is_open()) {
      std::cout << "Could not open " << path << " for writing" << std::endl;
      return false;
   }

   for (auto &instr : m_instructionList) {
	  out << "/* " << instr->disasm.name.value << " */" << std::endl;
	  out << "bool " << getSafeFunctionName(instr->disasm.name.value) << "(State *state, Instruction instr)" << std::endl;
      out << "{" << std::endl;
      out << "}" << std::endl;
      out << std::endl;
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

   for (auto &group : m_ast.opcodes) {
      for (auto &opcode : group.opcodes) {
         m_instructionList.push_back(&opcode);
      }
   }

   std::sort(m_instructionList.begin(),
             m_instructionList.end(),
             [](ast_opcd_def *lhs, ast_opcd_def *rhs) {
                return lhs->disasm.name.value < rhs->disasm.name.value;
             });

   out << "enum InstructionList {" << std::endl;

   for (auto &instr : m_instructionList) {
      out << "   " << getSafeFunctionName(instr->disasm.name.value) << " = " << instr->id.value << "," << std::endl;
   }

   out << "};" << std::endl;

   out.close();
   std::cout << "EmuGen: " << path << std::endl;
   return true;
}

bool EmuGen::createInstructionTableHeader(const std::string &path)
{
   std::fstream out;
   out.open(path, std::ifstream::out);

   if (!out.is_open()) {
      std::cout << "Could not open " << path << " for writing" << std::endl;
      return false;
   }

   out << "extern bool init();" << std::endl;
   out << "extern bool decode(State *state, Instruction instr);" << std::endl;
   out << std::endl;

   for (auto &instr : m_instructionList) {
      out << "bool " << getSafeFunctionName(instr->disasm.name.value) << "(State *state, Instruction instr);" << std::endl;
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

   out << "" << std::endl;

   /* Instruction Function Pointer Type */
   out << "typedef bool (*__emugen_instr_fptr)(State* state, Instruction instr);" << std::endl;
   out << std::endl;

   /* Instruction Table Type */
   out << "template<int bits>" << std::endl;
   out << "struct __emugen_table " << std::endl;
   out << "{" << std::endl;
   out << "   __emugen_table()" << std::endl;
   out << "   {" << std::endl;
   out << "      memset(this, 0, sizeof(__emugen_table<bits>));" << std::endl;
   out << "   }" << std::endl;
   out << "" << std::endl;
   out << "   __emugen_instr_fptr& operator[](int i)" << std::endl;
   out << "   {" << std::endl;
   out << "      return m_table[i];" << std::endl;
   out << "   }" << std::endl;
   out << std::endl;
   out << "   __emugen_instr_fptr m_table[1 << bits];" << std::endl;
   out << "};" << std::endl;
   out << std::endl;
   
   /* Create Opcode Tree */
   Node *root = nullptr;

   for (auto &group : m_ast.opcodes) {
      auto primaryField = findInstructionField(group.declaration.primary.value);
      auto cur = root;

      if (!primaryField) {
         std::cout << "Unknown instruction field " << group.declaration.primary.value << std::endl;
         return false;
      }

      if (!root) {
         root = new Node();
         root->id = -1;
         root->field = primaryField;
         cur = root;
      } else if (primaryField != root->field) {
         std::cout << "Found more than one primary opcode " << group.declaration.primary.value << std::endl;
         return false;
      }

      for (auto &secondary : group.declaration.secondary) {
         auto range = cur->children.equal_range(secondary.first.value);
         bool found = false;

         for (auto child = range.first; child != range.second; ++child) {
            if (child->second->field->name.value.compare(secondary.second.value) == 0) {
               cur = child->second;
               found = true;
               break;
            }
         }

         if (!found) {
            Node *node = new Node();
            node->id = secondary.first.value;
            node->field = findInstructionField(secondary.second.value);
            cur->children.insert(std::make_pair(secondary.first.value, node));
            cur = node;

            if (!node->field) {
               std::cout << "Unknown instruction field " << secondary.second.value << std::endl;
               return false;
            }
         }
      }

      for (auto &instr : group.opcodes) {
         cur->instructions.insert(std::make_pair(instr.id.value, &instr));
      }
   }

   /* Declare our opcode tables */
   std::function<std::pair<int, std::string>(Node*, const std::string &)> declareTable =
      [&out]
      (Node *node, const std::string &parent)
      {
         out << "static __emugen_table<" << node->field->bitrange.size() << "> ";
         out << "__emugen_table" << parent << "_" << node->field->name.value << ";" << std::endl;
         return std::make_pair(node->field->bitrange.size(), parent + "_" + node->field->name.value);
      };

   std::function<void(Node*, const std::pair<int, std::string> &, const std::string &)> declareTableReference =
      [&out]
      (Node *node, const std::pair<int, std::string> &value, const std::string &parent)
      {
         out << "static __emugen_table<" << value.first << "> ";
         out << "&__emugen_table" << parent << "_" << node->field->name.value;
         out << " = __emugen_table" << value.second << ";" << std::endl;
      };

   std::function<void(Node*, const std::string &)> checkChildren =
      [&out, &declareTable, &declareTableReference, &checkChildren]
      (Node *node, const std::string &parent)
      {
         /* Iterate through unique keys */
         for (auto itr = node->children.begin(); itr != node->children.end(); itr = node->children.upper_bound(itr->first)) {
            auto biggest = std::make_pair<int, Node *>(0, nullptr);
            auto range = node->children.equal_range(itr->first);
            auto name = parent + "_" + node->field->name.value;

            /* Find largest table for key */
            for (auto jtr = range.first; jtr != range.second; ++jtr) {
               if (jtr->second->field->bitrange.size() > biggest.first) {
                  biggest.first = jtr->second->field->bitrange.size();
                  biggest.second = jtr->second;
               }
            }

            /* Create the largest table */
            auto biggestTable = declareTable(biggest.second, name + "_" + std::to_string(itr->first));

            /* Smaller tables reference largest table */
            for (auto jtr = range.first; jtr != range.second; ++jtr) {
               if (jtr->second != biggest.second) {
                  declareTableReference(jtr->second, biggestTable, name + "_" + std::to_string(itr->first));
               }
            }

            /* Check all children */
            for (auto jtr = range.first; jtr != range.second; ++jtr) {
               checkChildren(jtr->second, name + "_" + std::to_string(itr->first));
            }
         }
      };

   declareTable(root, "");
   checkChildren(root, "");
   out << std::endl;

   std::function<void(ast_arch::Endian, std::vector<Node*>, const std::string &)> decodeFunction =
      [&out](ast_arch::Endian endian, std::vector<Node *> &nodes, const std::string &parent)
      {
         out << "bool decode" << parent << "(State *state, Instruction instr)" << std::endl;
         out << "{" << std::endl;
         out << "   unsigned int val = instr.value;" << std::endl;
         out << "   unsigned int op;" << std::endl;
         out << std::endl;

         for (auto node : nodes) {
            unsigned int shift, mask;

            mask = (1ull << node->field->bitrange.size()) - 1;

            if (endian == ast_arch::BigEndian) {
               shift = 31 - node->field->bitrange.end.value;
            } else {
               shift = node->field->bitrange.start.value;
            }

            out << "   op = (val >> " << shift << ") & 0x" << std::hex << mask << std::dec << ";" << std::endl;
            out << "   if (" << "__emugen_table" << parent << "_" << node->field->name.value << "[op]) {" << std::endl;
            out << "      return __emugen_table" << parent << "_" << node->field->name.value << "[op](state, instr);"<<std::endl;
            out << "   }" << std::endl;
            out << std::endl;
         }

         out << "   return false;" << std::endl;
         out << "}" << std::endl;
         out << std::endl;
      };


   std::function<void(ast_arch::Endian, Node*, const std::string &)> decodeChildren =
      [&out, &decodeFunction](ast_arch::Endian endian, Node *node, const std::string &parent)
      {
         /* Iterate through unique keys */
         for (auto itr = node->children.begin(); itr != node->children.end(); itr = node->children.upper_bound(itr->first)) {
            auto biggest = std::make_pair<int, Node *>(0, nullptr);
            auto range = node->children.equal_range(itr->first);
            auto name = parent + "_" + node->field->name.value;
            std::vector<Node *> nodes;

            /* List of children in descending bitrange size */
            for (auto jtr = range.first; jtr != range.second; ++jtr) {
               nodes.push_back(jtr->second);
            }

            std::sort(
               nodes.begin(),
               nodes.end(),
               [](Node *lhs, Node *rhs) {
                  return lhs->field->bitrange.size() > rhs->field->bitrange.size();
            });

            /* Create the child decode functions */
            decodeFunction(endian, nodes, name + "_" + std::to_string(itr->first));
         }
      };

   decodeFunction(m_ast.arch.endian, std::vector<Node *>(1, root), "");
   decodeChildren(m_ast.arch.endian, root, "");

   std::function<void(Node*, const std::string &)> initOpcodes =
      [&out, &initOpcodes](Node *node, const std::string &parent)
      {
         for (auto &instr : node->instructions) {
            out << "   __emugen_table" << parent << "_" << node->field->name.value;
            out << "[" << instr.first << "] = " << getSafeFunctionName(instr.second->disasm.name.value);
            out << ";" << std::endl;
         }

         for (auto child : node->children) {
            out << "   __emugen_table" << parent << "_" << node->field->name.value;
            out << "[" << child.first << "] = ";
            out << "decode" << parent << "_" << node->field->name.value << "_" << child.first;
            out << ";" << std::endl;

            initOpcodes(child.second, parent + "_" + node->field->name.value + "_" + std::to_string(child.first));
         }
      };

   out << "bool init()" << std::endl;
   out << "{" << std::endl;
   initOpcodes(root, "");
   out << "   return true;" << std::endl;
   out << "}" << std::endl;

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

ast_insf_field *EmuGen::findInstructionField(const std::string &name)
{
   for (auto &field : m_ast.insf.fields) {
      if (name.compare(field.name.value) == 0) {
         return &field;
      }
   }

   return nullptr;
}
