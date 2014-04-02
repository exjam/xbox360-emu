#ifndef EMUGEN_H
#define EMUGEN_H

#include "ast.h"

#include <string>
#include <map>

struct DecodeNode
{
   enum Type
   {
      Invalid,
      Instruction,
      Table
   };

   DecodeNode(Type type) : type(type)
   {
   }

   Type type;
};

struct DecodeTable : public DecodeNode
{
   DecodeTable() : DecodeNode(DecodeNode::Table)
   {
   }

   std::map<std::string, std::map<int, DecodeNode*>> table;
};

struct DecodeInstr : public DecodeNode
{
   DecodeInstr() : DecodeNode(DecodeNode::Instruction)
   {
   }

   ast_opcd_def *op;
};

class EmuGen
{
public:
   EmuGen();
   ~EmuGen();

   bool run(const std::string &definition, const std::string &outDirectory);

protected:
   bool parseFile(const std::string &path);

   bool createStubs(const std::string &path);
   bool createStubsHeader(const std::string &path);

   bool createCpuInfo(const std::string &path);
   bool createInstructionTable(const std::string &path);
   bool createDecoder(const std::string &path);

   bool createInstructionList(const std::string &path);
   bool createInstructionTableSource(const std::string &path);

   ast_insf_field *findInstructionField(std::string name);
   static std::string getSafeFunctionName(const std::string &name);

private:
   ast_root m_ast;
   std::map<std::string, ast_opcd_def *> m_instructionList;
   DecodeTable m_rootTable;
};

#endif // ifdef EMUGEN_H
