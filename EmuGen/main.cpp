#include "emugen.h"
#include <iostream>

int main(int argc, char **argv)
{
   if (argc < 3) {
      std::cout << "Usage: " << argv[0] << " <definition file> <output directory>" << std::endl;
      return 0;
   }

   EmuGen app;
   app.run(argv[1], argv[2]);

   return 0;
}

#if 0

#include <iostream>  
#include <fstream>
#include <string>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <regex>
#include <map>

static inline std::string ltrim(const std::string &_s)
{
   auto s = _s;
   s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
   return s;
}

static inline std::string rtrim(const std::string &_s)
{
   auto s = _s;
   s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
   return s;
}

static inline std::string stripComments(const std::string &_s)
{
   auto s = _s;
   auto p = s.rfind("//");

   if (p != std::string::npos) {
      s.erase(s.begin() + p, s.end());
   }

   return s;
}

static inline std::string trim(const std::string &_s)
{
   return ltrim(rtrim(_s));
}

static inline std::vector<std::string>& trim(std::vector<std::string> &v)
{
   for (std::string& s : v) {
      s = trim(s);
   }

   return v;
}

static inline std::vector<std::string> explodeWords(const std::string &_s)
{
   std::regex rxDefineOpcodes("[a-zA-Z_][a-zA-Z_0-9]*");
   std::vector<std::string> result;
   std::smatch rxMatch;
   auto s = trim(_s);

   while(true) {
      if (!std::regex_search(s, rxMatch, rxDefineOpcodes)) {
         break;
      }

      if (rxMatch[0].first != s.begin()) {
         result.push_back(std::string(s.begin(), rxMatch[0].first));
      }

      result.push_back(rxMatch[0].str());
      s.erase(s.begin(), rxMatch[0].second);
   }

   if (s.length()) {
      result.push_back(s);
   }

   return result;
}

static inline std::string firstWord(const std::string &_s)
{
   auto s = ltrim(_s);
   return std::string(s.begin(), std::find_if(s.begin(), s.end(), std::ptr_fun<int, int>(std::isspace)));
}

static inline std::vector<std::string> split(const std::string &s, std::string token)
{
   std::vector<std::string> result;
   auto lastSplit = 0;

   for (auto pos = s.find(token); pos != std::string::npos; pos = s.find(token, lastSplit)) {
      result.push_back(s.substr(lastSplit, pos - lastSplit));
      lastSplit = pos + token.length();
   }
   
   result.push_back(s.substr(lastSplit));
   return result;
}

static inline unsigned long long getBitMask(int bits)
{
   return (1ull << bits) - 1;
}

static inline std::string safeName(const std::string &_s)
{
   std::string s = _s;
   std::replace(s.begin(), s.end(), '.', 'o');
   return s;
}

struct BitRange
{
   BitRange() : 
      start(0), end(0)
   {
   }

   int start;
   int end;

   int size()
   {
      return 1 + end - start;
   }

   static BitRange extract(const std::string &line)
   {
      std::regex rxBitRange("([0-9]+)(?:\\.\\.\\.([0-9]+))?");
      std::smatch rxMatch;
      BitRange result;

      if (!std::regex_search(line, rxMatch, rxBitRange)) {
         return result;
      }

      result.start = std::stoi(rxMatch[1].str());

      if (rxMatch[2].str().length()) {
         result.end = std::stoi(rxMatch[2].str());
      } else {
         result.end = result.start;
      }

      return result;
   }
};

class SyntaxError : public std::exception {
public:
   SyntaxError(const std::string &line)
   {
      m_what = "Syntax Error: ";
      m_what += line;
   }

   SyntaxError(const std::string& msg, const std::string &line)
   {
      m_what = msg;
      m_what += ": ";
      m_what += line;
   }

   virtual const char* what() const throw()
   {
      return m_what.c_str();
   }

private:
   std::string m_what;
};

class Parser {
public:
   Parser(const std::string &name) :
      m_name(name)
   {
   }

   virtual bool parse(const std::string &line) = 0;
   virtual bool canParseCategory(const std::string &category) = 0;

protected:
   std::string m_name;
};

class ParserManager {
public:
   void addParser(Parser *parser)
   {
      m_parsers.push_back(parser);
   }

   Parser *getParser(const std::string &category)
   {
      for (auto p : m_parsers) {
         if (p->canParseCategory(category)) {
            return p;
         }
      }

      return NULL;
   }

private:
   std::vector<Parser*> m_parsers;
};

class OpcodeList : public Parser
{
public:
   struct Instruction {
      std::string name;
      std::vector<std::string> operands;
      int id;
      std::map<std::string, std::string> extra;
   };

   struct Node {
      std::string name;
      int value;
      std::string tableName;
      std::map<int, Instruction> instructions;
      std::multimap<int, Node*> children;
   };

public:
   OpcodeList() :
      Parser("Opcode Parser"), m_rootNode(NULL), m_activeNode(NULL)
   {
   }

   Node *root()
   {
      return m_rootNode;
   }

   std::vector<Instruction>& instructionList()
   {
      return m_instructions;
   }

   virtual bool parse(const std::string &line)
   {
      auto properties = trim(split(line, " : "));
      
      if (properties.size() != 3) {
         throw SyntaxError(line);
      }

      auto id = std::stoi(properties[0]);
      auto disasm = properties[1];

      Instruction instr;
      instr.id = id;
      instr.name = firstWord(disasm);

      disasm.erase(0, instr.name.length());
      instr.operands = explodeWords(disasm);
      
      for (auto extra : split(properties[2], ", ")) {
         auto vals = trim(split(extra, " = "));

         if (vals.size() == 2) {
            instr.extra.insert(std::make_pair(vals[0], vals[1]));
         } else if (vals.size() == 1) {
            instr.extra.insert(std::make_pair(vals[0], ""));
         } else {
            throw SyntaxError(properties[2]);
         }
      }

      m_instructions.push_back(instr);
      m_activeNode->instructions.insert(std::make_pair(id, instr));

      return true;
   }

   virtual bool canParseCategory(const std::string &category)
   {
      std::regex rxDefineOpcodes("([a-zA-Z_][a-zA-Z_0-9]* = [0-9]*, )*[a-zA-Z_][a-zA-Z_0-9]*");
      std::smatch rxMatch;

      if (!std::regex_match(category, rxMatch, rxDefineOpcodes)) {
         return false;
      }

      m_activeNode = NULL;
      
      for (auto ops : split(category, " = ")) {
         auto res = trim(split(ops, ", "));

         if (res.size() == 1) {
            if (!m_rootNode) {
               m_rootNode = new Node();
               m_rootNode->name = res[0];
            } else if (m_rootNode->name.compare(res[0]) != 0) {
               throw SyntaxError("Expecting only one primary opcode", category);
            }

            m_activeNode = m_rootNode;
         } else if (res.size() == 2) {
            auto range = m_activeNode->children.equal_range(std::stoi(res[0]));
            bool found = false;

            for (auto itr = range.first; itr != range.second; ++itr) {
               if (itr->second->name.compare(res[1]) == 0) {
                  m_activeNode = itr->second;
                  found = true;
                  break;
               }
            }

            if (!found) {
               auto node = new Node();
               node->name = res[1];
               node->value = std::stoi(res[0]);
               m_activeNode->children.insert(std::make_pair(node->value, node));
               m_activeNode = node;
            }
         } else {
            throw SyntaxError(category);
         }
      }

      return true;
   }

private:
   Node *m_rootNode;
   Node *m_activeNode;
   std::string m_activeCategory;
   std::vector<Instruction> m_instructions;
};

class InstructionFormat : public Parser
{
public:
   struct Field
   {
      std::string name;
      BitRange bitRange;
      std::map<std::string, std::string> extra;
   };

public:
   InstructionFormat() :
      Parser("Instruction Format")
   {
   }

   Field *getField(std::string name)
   {
      auto itr = m_fields.find(name);

      if (itr == m_fields.end()) {
         return nullptr;
      } else {
         return &itr->second;
      }
   }

   virtual bool parse(const std::string &line)
   {
      std::regex rxDefineOpcode("([a-zA-Z_][a-zA-Z_0-9]*) = ([0-9]+(?:\\.\\.\\.[0-9]+)?)(?: : (.*))?");
      std::smatch rxMatch;

      if (!std::regex_match(line, rxMatch, rxDefineOpcode)) {
         throw SyntaxError("Invalid [Instruction Format] field", line);
      }

      Field field;
      field.name = rxMatch[1].str();
      field.bitRange = BitRange::extract(rxMatch[2].str());

      if (rxMatch[3].str().length()) {
         std::string extras = rxMatch[3].str();

         for (auto extra : split(extras, ", ")) {
            auto vals = trim(split(extra, " = "));
            
            if (vals.size() != 2) {
               throw SyntaxError("Invalid [Instruction Format] field extra", line);
            }

            field.extra.insert(std::make_pair(vals[0], vals[1]));
         }
      }

      m_fields.insert(std::make_pair(field.name, field));
      return true;
   }

   virtual bool canParseCategory(const std::string &category)
   {
      return category.compare(m_name) == 0;
   }

private:
   std::map<std::string, Field> m_fields;
};

class EmuGen {
public:
   EmuGen()
   {
      m_parsers.addParser(&m_opcodeParser);
      m_parsers.addParser(&m_instructionFormat);
   }

   bool run(const std::string &definitionFile, const std::string &outDirectory)
   {
      parseFile(definitionFile);

      createInstrListFile(outDirectory + "/emugen_instr_list.h");
      createDisassembler(outDirectory + "/emugen_disassembler.h");
      createInstrTableFileHeader(outDirectory + "/emugen_instr_table.h");
      createInstrTableFileSource(outDirectory + "/emugen_instr_table.cpp");

      return true;
   }

private:
   bool parseFile(const std::string &path)
   {
      std::regex rxCategory("\\[(.*)\\]");
      std::smatch rxMatch;
      std::fstream in;
      Parser *parser;

      in.open(path, std::ifstream::in);

      if (!in.is_open()) {
         std::cout << "Could not open file for reading " << path << std::endl;
         return false;
      }

      parser = NULL;

      for (std::string line; std::getline(in, line); ) {
         line = trim(stripComments(line));

         if (line.length() == 0) {
            continue;
         }

         if (std::regex_match(line, rxMatch, rxCategory)) {
            parser = m_parsers.getParser(rxMatch[1].str());
         } else if (parser) {
            parser->parse(line);
         } else {
            throw SyntaxError(line);
         }
      }

      return true;
   }

   bool createDisassembler(const std::string& path)
   {
      std::fstream out;
      out.open(path, std::ifstream::out);

      if (!out.is_open()) {
         return false;
      }

      auto is = m_opcodeParser.instructionList();

      for (auto i : is) {
         out << "/* " << i.name << " */" << std::endl;
         out << "bool " << safeName(i.name) << "(State *state, Instruction instr) {" << std::endl;
         out << "   state->result.name = \"" << i.name << "\";" << std::endl;
         out << "   state->result.fullname = " << i.extra["fullname"] << ";" << std::endl;
         out << "   state->result.operands";

         for (auto o : i.operands) {
            auto field = m_instructionFormat.getField(o);

            
            if (field) {
               out << std::endl << "      << ";

               if (field->extra.find("base") != field->extra.end()) {
                  int base = std::stoi(field->extra["base"]);

                  if (base == 8) {
                     out << "std::oct << ";
                  } else if (base == 10) {
                     out << "std::dec << ";
                  } else if (base == 16) {
                     out << "std::hex << ";
                  } else {
                     throw SyntaxError("Unknown base", field->extra["base"]);
                  }
               }

               if (field->extra.find("fill") != field->extra.end()) {
                  out << "std::setfill(" << field->extra["fill"] << ") << ";
               }

               if (field->extra.find("width") != field->extra.end()) {
                  out << "std::setw(" << field->extra["width"] << ") << ";
               }

               if (field->extra.find("value") != field->extra.end()) {
                  auto vw = explodeWords(field->extra["value"]);

                  out << "(";

                  for (auto w : vw) {
                     auto wfield = m_instructionFormat.getField(w);

                     if (wfield) {
                        out << "instr." << wfield->name;
                     } else {
                        out << w;
                     }
                  }

                  out << ")";
               } else {
                  out << "instr." << field->name;
               }

               if (field->extra.find("base") != field->extra.end()) {
                  int base = std::stoi(field->extra["base"]);

                  if (base != 10) {
                     out << " << std::dec";
                  }
               }
            } else {
               out << std::endl << "      << \"" << o << "\"";
            }
         }

         out << ";" << std::endl;

         out << "}" << std::endl;
         out << std::endl;
      }

      out.close();
      return true;
   }

   bool createInstrListFile(const std::string& path)
   {
      std::fstream out;
      out.open(path, std::ifstream::out);

      if (!out.is_open()) {
         return false;
      }

      auto is = m_opcodeParser.instructionList();

      std::sort(is.begin(),
                is.end(),
                [](OpcodeList::Instruction &lhs, OpcodeList::Instruction &rhs) {
                   return lhs.name < rhs.name;
                });
      
      out << "enum InstructionList {" << std::endl;

      for (auto i : is) {
         out << safeName(i.name) << " = " << i.id << "," << std::endl;
      }

      out << "};" << std::endl;

      out.close();
      return true;
   }

   bool createInstrTableFileHeader(const std::string& path)
   {
      std::fstream out;
      out.open(path, std::ifstream::out);

      if (!out.is_open()) {
         return false;
      }

      /* Declare instruction functions */
      for (auto instr : m_opcodeParser.instructionList()) {
         out << "bool " << safeName(instr.name) << "(State *state, Instruction instr);" << std::endl;
      }

      out << std::endl;

      /* Declare table functions */
      out << "extern void init();" << std::endl;
      out << "extern bool decode(State *state, Instruction instr);" << std::endl;

      out.close();
      return true;
   }

   bool createInstrTableFileSource(const std::string& path)
   {
      std::fstream out;
      out.open(path, std::ifstream::out);

      if (!out.is_open()) {
         return false;
      }

      /* Declare types for table */
      out << std::endl;
      out << "typedef bool (*__emugen_instr_fptr)(State* state, Instruction instr);" << std::endl;
      out << std::endl;
      out << "template<int bits>" << std::endl;
      out << "struct __emugen_table" << std::endl;
      out << "{" << std::endl;
      out << "   __emugen_table()" << std::endl;
      out << "   {" << std::endl;
      out << "      memset(this, 0, sizeof(__emugen_table<bits>));" << std::endl;
      out << "   }" << std::endl;
      out << std::endl;
      out << "   __emugen_instr_fptr& operator[](int i)" << std::endl;
      out << "   {" << std::endl;
      out << "      return m_table[i];" << std::endl;
      out << "   }" << std::endl;
      out << std::endl;
      out << "   __emugen_instr_fptr m_table[1 << bits];" << std::endl;
      out << "};" << std::endl;
      out << std::endl;

      /* Define tables */
      defineTables(out, m_opcodeParser.root());
      out << std::endl;

      /* Define table decode functions */
      decodeTables(out, m_opcodeParser.root());


      /* Define table init functions */
      out << "void init()" << std::endl;
      out << "{" << std::endl;
      initTables(out, m_opcodeParser.root());
      out << "}" << std::endl;
      out << std::endl;

      out.close();
      return true;
   }

   template<typename T>
   void decodeTables(std::ostream &out, std::pair<T, T>& nodes, const std::string &parent)
   {
      bool bigEndian = true;

      out << "static bool __decode" << parent << "(State *state, Instruction instr) {" << std::endl;
      out << "   unsigned int uiInstr = (unsigned int)instr;" << std::endl;
      out << "   unsigned int op;" << std::endl;

      for (auto i = nodes.first; i != nodes.second; ++i) {
         auto node = i->second;
         auto field = m_instructionFormat.getField(node->name);
         auto shift = bigEndian ? (31 - field->bitRange.end) : field->bitRange.start;
         auto mask = getBitMask(field->bitRange.size());

         out << "   op = (uiInstr >> " << shift << ") & 0x" << std::hex << mask << std::dec << ";" << std::endl;
         out << "   if (" << "__emugen_table" << parent << "_" << node->name << "[op]) {" << std::endl;
         out << "      return __emugen_table" << parent << "_" << node->name << "[op](state, instr);"<<std::endl;
         out << "   }" << std::endl;
      }

      out << "   return false;" << std::endl;
      out << "}" << std::endl;
      out << std::endl;

      for (auto i = nodes.first; i != nodes.second; ++i) {
         auto node = i->second;

         for (auto j = node->children.begin(); j != node->children.end(); j = node->children.upper_bound(j->first)) {
            std::string parentStr = parent + "_" + nodes.first->second->name + "_" + std::to_string(j->first);
            decodeTables(out, node->children.equal_range(j->first), parentStr);
         }
      }
   }

   void decodeTables(std::ostream &out, OpcodeList::Node *node)
   {
      bool bigEndian = true;
      auto field = m_instructionFormat.getField(node->name);
      auto shift = bigEndian ? (31 - field->bitRange.end) : field->bitRange.start;
      auto mask = getBitMask(field->bitRange.size());

      out << "bool decode(State *state, Instruction instr) {" << std::endl;
      out << "   unsigned int uiInstr = (unsigned int)instr;" << std::endl;
      out << "   unsigned int op;" << std::endl;

      out << "   op = (uiInstr >> " << shift << ") & 0x" << std::hex << mask << std::dec << ";" << std::endl;
      out << "   if (" << "__emugen_table_" << node->name << "[op]) {" << std::endl;
      out << "      return __emugen_table_" << node->name << "[op](state, instr);"<<std::endl;
      out << "   }" << std::endl;

      out << "   return false;" << std::endl;
      out << "}" << std::endl;
      out << std::endl;

      for (auto i = node->children.begin(); i != node->children.end(); i = node->children.upper_bound(i->first)) {
         std::string parentStr = "_" + node->name + "_" + std::to_string(i->first);
         decodeTables(out, node->children.equal_range(i->first), parentStr);
      }
   }

   void initTables(std::ostream &out, OpcodeList::Node *node, const std::string &parent = "")
   {
      std::string parentStr;

      for (auto instr : node->instructions) {
         out << "   " << node->tableName;
         out << "[" << instr.first << "]";
         out << " = " << safeName(instr.second.name) << ";" << std::endl;
      }

      for (auto i = node->children.begin(); i != node->children.end(); i = node->children.upper_bound(i->first)) {
         out << "   " << node->tableName;
         out << "[" << i->first << "]";
         out << " = __decode" << parent << "_" << node->name << "_" << i->first << ";" << std::endl;
      }

      for (auto child : node->children) {
         parentStr = parent + "_" + node->name + "_" + std::to_string(child.first);
         initTables(out, child.second, parentStr);
      }
   }

   std::pair<std::string, int> defineTables(std::ostream &out, OpcodeList::Node *node, const std::string &parent = "", std::pair<std::string, int>* refTable = nullptr)
   {
      std::pair<std::string, int> table;
      std::string parentStr;
      auto field = m_instructionFormat.getField(node->name);

      if (field == nullptr) {
         throw SyntaxError("Unknown field name used", node->name);
      }

      table.first = parent;
      table.first += "_";
      table.first += node->name;
      table.second = !refTable ? field->bitRange.size() : refTable->second;

      node->tableName = "__emugen_table" + table.first;

      out << "static __emugen_table<" << table.second << "> ";

      if (refTable) {
         out << "&";
      }

      out << "__emugen_table" << table.first;

      if (refTable) {
         out << " = __emugen_table" << refTable->first;
      }

      out << ";" << std::endl;

      /* Iterate over unique multimap keys */
      for (auto i = node->children.begin(); i != node->children.end(); i = node->children.upper_bound(i->first)) {
         std::pair<int, OpcodeList::Node *> largest(0, nullptr);
         auto eqr = node->children.equal_range(i->first);

         /* Find the largest child table */
         for (auto j = eqr.first; j != eqr.second; ++j) {
            auto field = m_instructionFormat.getField(j->second->name);

            if (field && field->bitRange.size() > largest.first) {
               largest = std::make_pair(field->bitRange.size(), j->second);
            }
         }

         /* Create the largest child table */
         auto largestTable = defineTables(out, largest.second, table.first + "_" + std::to_string(largest.second->value));

         for (auto j = eqr.first; j != eqr.second; ++j) {
            if (j->second != largest.second) {
               defineTables(out, j->second, table.first + "_" + std::to_string(j->first), &largestTable);
            }
         }
      }

      return table;
   }

private:
   ParserManager m_parsers;
   OpcodeList m_opcodeParser;
   InstructionFormat m_instructionFormat;
};
/*
int main(int argc, char **argv)
{
   if (argc < 3) {
      std::cout << "Usage: " << argv[0] << " <definition file> <output directory>" << std::endl;
      return 0;
   }
   
   EmuGen app;
   app.run(argv[1], argv[2]);

   return 0;
}*/

#endif