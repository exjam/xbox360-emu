AddInstr(202, addze) AddInstrAlias(addze, 0x200 | 202, addzeo)
AddInstr(234, addme) AddInstrAlias(addme, 0x200 | 234, addmeo)
AddInstr(266, add) AddInstrAlias(add, 0x200 | 266, addo)
  AddInstrAlias(add,  10, addc) AddInstrAlias(add, 0x200 |  10, addco)
  AddInstrAlias(add, 138, adde) AddInstrAlias(add, 0x200 | 138, addeo)

AddInstr(28, and)
AddInstr(60, andc)

AddInstr(0, cmp)
AddInstr(32, cmpl)

AddInstr(86,   dcbf)
AddInstr(54,   dcbst)
AddInstr(278,  dcbt)
AddInstr(246,  dcbtst)
AddInstr(1014, dcbz)

AddInstr(489, divd) AddInstrAlias(divd, 0x200 | 489, divdo)

AddInstr(339, mfspr)
