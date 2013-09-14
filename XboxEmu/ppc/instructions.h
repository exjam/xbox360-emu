#ifndef PPC_INSTRUCTIONS_H
#define PPC_INSTRUCTIONS_H

namespace ppc
{

namespace op
{

#define AddTable(id)
#define AddInstrAlias(orig, id, name) name = (id),
#define AddInstr(id, name) name = (id),

enum Ops {
#include "tablePrimary.inc"
#include "table19.inc"
#include "table31.inc"
};

#undef AddTable
#undef AddInstr
#undef AddInstrAlias

} // namespace op

} // namespace ppc

#endif // PPC_INSTRUCTIONS_H
