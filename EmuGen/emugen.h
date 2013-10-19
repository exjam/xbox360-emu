#ifndef EMUGEN_H
#define EMUGEN_H

#include "ast.h"

class EmuGen
{
public:
   EmuGen();
   ~EmuGen();

   bool run(const std::string &definition, const std::string &outDirectory);

protected:
   bool parseFile(const std::string &path);

   bool createInstructionList(const std::string &path);
   bool createInstructionTableHeader(const std::string &path);
   bool createInstructionTableSource(const std::string &path);

   bool createDisassembler(const std::string &path);

   ast_insf_field *findInstructionField(const std::string &name);
   static std::string getSafeFunctionName(const std::string &name);

private:
   ast_root m_ast;
   std::vector<ast_opcd_def *> m_instructionList;
};

#endif // ifdef EMUGEN_H
