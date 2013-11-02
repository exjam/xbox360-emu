/* Add */
bool add(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "add";
   state->result.name = "Add";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rD, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Add Carrying */
bool addc(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "addc";
   state->result.name = "Add Carrying";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rD, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Add Extended */
bool adde(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "adde";
   state->result.name = "Add Extended";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rD, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Add Immediate */
bool addi(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "addi";
   state->result.name = "Add Immediate";
   sprintf_s(buffer, 64, "r%u, r%u, %lli", instr.rD, instr.rA, bits::signExtend<16, uint64_t>(instr.simm));
   state->result.operands = buffer;
   return true;
}

/* Add Immediate Carrying */
bool addic(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "addic";
   state->result.name = "Add Immediate Carrying";
   sprintf_s(buffer, 64, "r%u, r%u, %lli", instr.rD, instr.rA, bits::signExtend<16, uint64_t>(instr.simm));
   state->result.operands = buffer;
   return true;
}

/* Add Immediate Carrying and Record */
bool addico(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "addic.";
   state->result.name = "Add Immediate Carrying and Record";
   sprintf_s(buffer, 64, "r%u, r%u, %lli", instr.rD, instr.rA, bits::signExtend<16, uint64_t>(instr.simm));
   state->result.operands = buffer;
   return true;
}

/* Add Immediate Shifted */
bool addis(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "addis";
   state->result.name = "Add Immediate Shifted";
   sprintf_s(buffer, 64, "r%u, r%u, %lli", instr.rD, instr.rA, bits::signExtend<16, uint64_t>(instr.simm));
   state->result.operands = buffer;
   return true;
}

/* Add to Minus One Extended */
bool addme(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "addme";
   state->result.name = "Add to Minus One Extended";
   sprintf_s(buffer, 64, "r%u, r%u", instr.rD, instr.rA);
   state->result.operands = buffer;
   return true;
}

/* Add to Zero Extended */
bool addze(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "addze";
   state->result.name = "Add to Zero Extended";
   sprintf_s(buffer, 64, "r%u, r%u", instr.rD, instr.rA);
   state->result.operands = buffer;
   return true;
}

/* AND */
bool and(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "and";
   state->result.name = "AND";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rA, instr.rS, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* AND with Complement */
bool andc(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "andc";
   state->result.name = "AND with Complement";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rA, instr.rS, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* AND Immediate */
bool andio(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "andi.";
   state->result.name = "AND Immediate";
   sprintf_s(buffer, 64, "r%u, r%u, %u", instr.rA, instr.rS, instr.uimm);
   state->result.operands = buffer;
   return true;
}

/* AND Immediate Shifted */
bool andiso(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "andis.";
   state->result.name = "AND Immediate Shifted";
   sprintf_s(buffer, 64, "r%u, r%u, %u", instr.rA, instr.rS, instr.uimm);
   state->result.operands = buffer;
   return true;
}

/* Branch */
bool b(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "b";
   state->result.name = "Branch";
   sprintf_s(buffer, 64, "0x%08X", (instr.aa ? 0 : state->cia) + bits::signExtend<26, uint64_t>(instr.li << 2));
   state->result.operands = buffer;
   return true;
}

/* Branch Conditional */
bool bc(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "bc";
   state->result.name = "Branch Conditional";
   sprintf_s(buffer, 64, "%u, %u, 0x%08X", instr.bo, instr.bi, (instr.aa ? 0 : state->cia) + bits::signExtend<16, uint64_t>(instr.bd << 2));
   state->result.operands = buffer;
   return true;
}

/* Branch Conditional to Count Register */
bool bcctr(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "bcctr";
   state->result.name = "Branch Conditional to Count Register";
   sprintf_s(buffer, 64, "%u, %u, %u", instr.bo, instr.bi, instr.bh);
   state->result.operands = buffer;
   return true;
}

/* Branch Conditional to Link Register */
bool bclr(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "bclr";
   state->result.name = "Branch Conditional to Link Register";
   sprintf_s(buffer, 64, "%u, %u, %u", instr.bo, instr.bi, instr.bh);
   state->result.operands = buffer;
   return true;
}

/* Compare */
bool cmp(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "cmp";
   state->result.name = "Compare";
   sprintf_s(buffer, 64, "%u, %u, r%u, r%u", instr.crfD, instr.l, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Compare Immediate */
bool cmpi(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "cmpi";
   state->result.name = "Compare Immediate";
   sprintf_s(buffer, 64, "%u, %u, r%u, %lli", instr.crfD, instr.l, instr.rA, bits::signExtend<16, uint64_t>(instr.simm));
   state->result.operands = buffer;
   return true;
}

/* Compare Logical */
bool cmpl(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "cmpl";
   state->result.name = "Compare Logical";
   sprintf_s(buffer, 64, "%u, %u, r%u, r%u", instr.crfD, instr.l, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Compare Logical Immediate */
bool cmpli(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "cmpli";
   state->result.name = "Compare Logical Immediate";
   sprintf_s(buffer, 64, "%u, %u, r%u, %u", instr.crfD, instr.l, instr.rA, instr.uimm);
   state->result.operands = buffer;
   return true;
}

/* Count Leading Zeros Doubleword */
bool cntlzd(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "cntlzd";
   state->result.name = "Count Leading Zeros Doubleword";
   sprintf_s(buffer, 64, "r%u, r%u", instr.rA, instr.rS);
   state->result.operands = buffer;
   return true;
}

/* Count Leading Zeros Word */
bool cntlzw(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "cntlzw";
   state->result.name = "Count Leading Zeros Word";
   sprintf_s(buffer, 64, "r%u, r%u", instr.rA, instr.rS);
   state->result.operands = buffer;
   return true;
}

/* Condition Register AND */
bool crand(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "crand";
   state->result.name = "Condition Register AND";
   sprintf_s(buffer, 64, "%u, %u, %u", instr.crbD, instr.crbA, instr.crbB);
   state->result.operands = buffer;
   return true;
}

/* Condition Register AND with Complement */
bool crandc(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "crandc";
   state->result.name = "Condition Register AND with Complement";
   sprintf_s(buffer, 64, "%u, %u, %u", instr.crbD, instr.crbA, instr.crbB);
   state->result.operands = buffer;
   return true;
}

/* Condition Register Equivalent */
bool creqv(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "creqv";
   state->result.name = "Condition Register Equivalent";
   sprintf_s(buffer, 64, "%u, %u, %u", instr.crbD, instr.crbA, instr.crbB);
   state->result.operands = buffer;
   return true;
}

/* Condition Register NAND */
bool crnand(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "crnand";
   state->result.name = "Condition Register NAND";
   sprintf_s(buffer, 64, "%u, %u, %u", instr.crbD, instr.crbA, instr.crbB);
   state->result.operands = buffer;
   return true;
}

/* Condition Register NOR */
bool crnor(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "crnor";
   state->result.name = "Condition Register NOR";
   sprintf_s(buffer, 64, "%u, %u, %u", instr.crbD, instr.crbA, instr.crbB);
   state->result.operands = buffer;
   return true;
}

/* Condition Register OR */
bool cror(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "cror";
   state->result.name = "Condition Register OR";
   sprintf_s(buffer, 64, "%u, %u, %u", instr.crbD, instr.crbA, instr.crbB);
   state->result.operands = buffer;
   return true;
}

/* Condition Register OR with Complement */
bool crorc(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "crorc";
   state->result.name = "Condition Register OR with Complement";
   sprintf_s(buffer, 64, "%u, %u, %u", instr.crbD, instr.crbA, instr.crbB);
   state->result.operands = buffer;
   return true;
}

/* Condition Register XOR */
bool crxor(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "crxor";
   state->result.name = "Condition Register XOR";
   sprintf_s(buffer, 64, "%u, %u, %u", instr.crbD, instr.crbA, instr.crbB);
   state->result.operands = buffer;
   return true;
}

/* Data Cache Block Flush */
bool dcbf(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "dcbf";
   state->result.name = "Data Cache Block Flush";
   sprintf_s(buffer, 64, "r%u, r%u", instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Data Cache Block Store */
bool dcbst(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "dcbst";
   state->result.name = "Data Cache Block Store";
   sprintf_s(buffer, 64, "r%u, r%u", instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Data Cache Block Touch */
bool dcbt(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "dcbt";
   state->result.name = "Data Cache Block Touch";
   sprintf_s(buffer, 64, "r%u, r%u, %u", instr.rA, instr.rB, instr.th);
   state->result.operands = buffer;
   return true;
}

/* Data Cache Block Touch for Store */
bool dcbtst(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "dcbtst";
   state->result.name = "Data Cache Block Touch for Store";
   sprintf_s(buffer, 64, "r%u, r%u", instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Data Cache Clear to Zero */
bool dcbz(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "dcbz";
   state->result.name = "Data Cache Clear to Zero";
   sprintf_s(buffer, 64, "r%u, r%u", instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Divide Doubleword */
bool divd(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "divd";
   state->result.name = "Divide Doubleword";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rD, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Divide Doubleword Unsigned */
bool divdu(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "divdu";
   state->result.name = "Divide Doubleword Unsigned";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rD, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Divide Word */
bool divw(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "divw";
   state->result.name = "Divide Word";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rD, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Divide Word Unsigned */
bool divwu(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "divwu";
   state->result.name = "Divide Word Unsigned";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rD, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* External Control In Word Indexed */
bool eciwx(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "eciwx";
   state->result.name = "External Control In Word Indexed";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rD, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* External Control Out Word Indexed */
bool ecowx(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "ecowx";
   state->result.name = "External Control Out Word Indexed";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rS, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Enforce In-Order Execution of I/O */
bool eieio(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "eieio";
   state->result.name = "Enforce In-Order Execution of I/O";
   sprintf_s(buffer, 64, "");
   state->result.operands = buffer;
   return true;
}

/* Equivalent */
bool eqv(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "eqv";
   state->result.name = "Equivalent";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rA, instr.rS, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Extend Sign Byte */
bool extsb(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "extsb";
   state->result.name = "Extend Sign Byte";
   sprintf_s(buffer, 64, "r%u, r%u", instr.rA, instr.rS);
   state->result.operands = buffer;
   return true;
}

/* Extend Sign Halfword */
bool extsh(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "extsh";
   state->result.name = "Extend Sign Halfword";
   sprintf_s(buffer, 64, "r%u, r%u", instr.rA, instr.rS);
   state->result.operands = buffer;
   return true;
}

/* Extend Sign Word */
bool extsw(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "extsw";
   state->result.name = "Extend Sign Word";
   sprintf_s(buffer, 64, "r%u, r%u", instr.rA, instr.rS);
   state->result.operands = buffer;
   return true;
}

/* Floating Absolute Value */
bool fabs(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "fabs";
   state->result.name = "Floating Absolute Value";
   sprintf_s(buffer, 64, "fr%u, fr%u", instr.frD, instr.frA);
   state->result.operands = buffer;
   return true;
}

/* Floating Add (Double-Precision) */
bool fadd(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "fadd";
   state->result.name = "Floating Add (Double-Precision)";
   sprintf_s(buffer, 64, "fr%u, fr%u, fr%u", instr.frD, instr.frA, instr.frB);
   state->result.operands = buffer;
   return true;
}

/* Floating Add Single */
bool fadds(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "fadds";
   state->result.name = "Floating Add Single";
   sprintf_s(buffer, 64, "fr%u, fr%u, fr%u", instr.frD, instr.frA, instr.frB);
   state->result.operands = buffer;
   return true;
}

/* Floating Convert from Integer Doubleword */
bool fcfid(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "fcfid";
   state->result.name = "Floating Convert from Integer Doubleword";
   sprintf_s(buffer, 64, "fr%u, fr%u", instr.frD, instr.frB);
   state->result.operands = buffer;
   return true;
}

/* Floating Compare Ordered */
bool fcmpo(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "fcmpo";
   state->result.name = "Floating Compare Ordered";
   sprintf_s(buffer, 64, "%u, fr%u, fr%u", instr.crfD, instr.frA, instr.frB);
   state->result.operands = buffer;
   return true;
}

/* Floating Compare Unordered */
bool fcmpu(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "fcmpu";
   state->result.name = "Floating Compare Unordered";
   sprintf_s(buffer, 64, "%u, fr%u, fr%u", instr.crfD, instr.frA, instr.frB);
   state->result.operands = buffer;
   return true;
}

/* Floating Convert to Integer Doubleword */
bool fctid(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "fctid";
   state->result.name = "Floating Convert to Integer Doubleword";
   sprintf_s(buffer, 64, "fr%u, fr%u", instr.frD, instr.frB);
   state->result.operands = buffer;
   return true;
}

/* Floating Convert to Integer Doubleword with round toward Zero */
bool fctidz(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "fctidz";
   state->result.name = "Floating Convert to Integer Doubleword with round toward Zero";
   sprintf_s(buffer, 64, "fr%u, fr%u", instr.frD, instr.frB);
   state->result.operands = buffer;
   return true;
}

/* Floating Convert to Integer Word */
bool fctiw(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "fctiw";
   state->result.name = "Floating Convert to Integer Word";
   sprintf_s(buffer, 64, "fr%u, fr%u", instr.frD, instr.frB);
   state->result.operands = buffer;
   return true;
}

/* Floating Convert to Integer Word with round toward Zero */
bool fctiwz(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "fctiwz";
   state->result.name = "Floating Convert to Integer Word with round toward Zero";
   sprintf_s(buffer, 64, "fr%u, fr%u", instr.frD, instr.frB);
   state->result.operands = buffer;
   return true;
}

/* Floating Divide (Double-Precision) */
bool fdiv(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "fdiv";
   state->result.name = "Floating Divide (Double-Precision)";
   sprintf_s(buffer, 64, "fr%u, fr%u, fr%u", instr.frD, instr.frA, instr.frB);
   state->result.operands = buffer;
   return true;
}

/* Floating Divide Single */
bool fdivs(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "fdivs";
   state->result.name = "Floating Divide Single";
   sprintf_s(buffer, 64, "fr%u, fr%u, fr%u", instr.frD, instr.frA, instr.frB);
   state->result.operands = buffer;
   return true;
}

/* Floating Multiply-Add (Double-Precision) */
bool fmadd(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "fmadd";
   state->result.name = "Floating Multiply-Add (Double-Precision)";
   sprintf_s(buffer, 64, "fr%u, fr%u, fr%u, fr%u", instr.frD, instr.frA, instr.frC, instr.frB);
   state->result.operands = buffer;
   return true;
}

/* Floating Multiply-Add Single */
bool fmadds(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "fmadds";
   state->result.name = "Floating Multiply-Add Single";
   sprintf_s(buffer, 64, "fr%u, fr%u, fr%u, fr%u", instr.frD, instr.frA, instr.frC, instr.frB);
   state->result.operands = buffer;
   return true;
}

/* Floating Move Register (Double-Precision) */
bool fmr(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "fmr";
   state->result.name = "Floating Move Register (Double-Precision)";
   sprintf_s(buffer, 64, "fr%u, fr%u", instr.frD, instr.frB);
   state->result.operands = buffer;
   return true;
}

/* Floating Multiply-Subtract (Double-Precision) */
bool fmsub(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "fmsub";
   state->result.name = "Floating Multiply-Subtract (Double-Precision)";
   sprintf_s(buffer, 64, "fr%u, fr%u, fr%u, fr%u", instr.frD, instr.frA, instr.frC, instr.frB);
   state->result.operands = buffer;
   return true;
}

/* Floating Multiply-Subtract Single */
bool fmsubs(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "fmsubs";
   state->result.name = "Floating Multiply-Subtract Single";
   sprintf_s(buffer, 64, "fr%u, fr%u, fr%u, fr%u", instr.frD, instr.frA, instr.frC, instr.frB);
   state->result.operands = buffer;
   return true;
}

/* Floating Multiply (Double-Precision) */
bool fmul(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "fmul";
   state->result.name = "Floating Multiply (Double-Precision)";
   sprintf_s(buffer, 64, "fr%u, fr%u, fr%u", instr.frD, instr.frA, instr.frC);
   state->result.operands = buffer;
   return true;
}

/* Floating Multiply Single */
bool fmuls(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "fmuls";
   state->result.name = "Floating Multiply Single";
   sprintf_s(buffer, 64, "fr%u, fr%u, fr%u", instr.frD, instr.frA, instr.frC);
   state->result.operands = buffer;
   return true;
}

/* Floating Negative Absolute Value */
bool fnabs(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "fnabs";
   state->result.name = "Floating Negative Absolute Value";
   sprintf_s(buffer, 64, "fr%u, fr%u", instr.frD, instr.frB);
   state->result.operands = buffer;
   return true;
}

/* Floating Negate */
bool fneg(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "fneg";
   state->result.name = "Floating Negate";
   sprintf_s(buffer, 64, "fr%u, fr%u", instr.frD, instr.frB);
   state->result.operands = buffer;
   return true;
}

/* Floating Negative Multiply-Add (Double-Precision) */
bool fnmadd(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "fnmadd";
   state->result.name = "Floating Negative Multiply-Add (Double-Precision)";
   sprintf_s(buffer, 64, "fr%u, fr%u, fr%u, fr%u", instr.frD, instr.frA, instr.frC, instr.frB);
   state->result.operands = buffer;
   return true;
}

/* Floating Negative Multiply-Add Single */
bool fnmadds(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "fnmadds";
   state->result.name = "Floating Negative Multiply-Add Single";
   sprintf_s(buffer, 64, "fr%u, fr%u, fr%u, fr%u", instr.frD, instr.frA, instr.frC, instr.frB);
   state->result.operands = buffer;
   return true;
}

/* Floating Negative Multiply-Subtract (Double-Precision) */
bool fnmsub(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "fnmsub";
   state->result.name = "Floating Negative Multiply-Subtract (Double-Precision)";
   sprintf_s(buffer, 64, "fr%u, fr%u, fr%u, fr%u", instr.frD, instr.frA, instr.frC, instr.frB);
   state->result.operands = buffer;
   return true;
}

/* Floating Negative Multiply-Subtract Single */
bool fnmsubs(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "fnmsubs";
   state->result.name = "Floating Negative Multiply-Subtract Single";
   sprintf_s(buffer, 64, "fr%u, fr%u, fr%u, fr%u", instr.frD, instr.frA, instr.frC, instr.frB);
   state->result.operands = buffer;
   return true;
}

/* Floating Reciprocal Estimate Single */
bool fres(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "fres";
   state->result.name = "Floating Reciprocal Estimate Single";
   sprintf_s(buffer, 64, "fr%u, fr%u", instr.frD, instr.frB);
   state->result.operands = buffer;
   return true;
}

/* Floating Round to Single */
bool frsp(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "frsp";
   state->result.name = "Floating Round to Single";
   sprintf_s(buffer, 64, "fr%u, fr%u", instr.frD, instr.frB);
   state->result.operands = buffer;
   return true;
}

/* Floating Reciprocal Square Root Estimate */
bool frsqrte(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "frsqrte";
   state->result.name = "Floating Reciprocal Square Root Estimate";
   sprintf_s(buffer, 64, "fr%u, fr%u", instr.frD, instr.frB);
   state->result.operands = buffer;
   return true;
}

/* Floating Select */
bool fsel(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "fsel";
   state->result.name = "Floating Select";
   sprintf_s(buffer, 64, "fr%u, fr%u, fr%u, fr%u", instr.frD, instr.frA, instr.frC, instr.frB);
   state->result.operands = buffer;
   return true;
}

/* Floating Square Root (Double-Precision) */
bool fsqrt(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "fsqrt";
   state->result.name = "Floating Square Root (Double-Precision)";
   sprintf_s(buffer, 64, "fr%u, fr%u", instr.frD, instr.frB);
   state->result.operands = buffer;
   return true;
}

/* Floating Square Root Single */
bool fsqrts(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "fsqrts";
   state->result.name = "Floating Square Root Single";
   sprintf_s(buffer, 64, "fr%u, fr%u", instr.frD, instr.frB);
   state->result.operands = buffer;
   return true;
}

/* Floating Subtract (Double-Precision) */
bool fsub(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "fsub";
   state->result.name = "Floating Subtract (Double-Precision)";
   sprintf_s(buffer, 64, "fr%u, fr%u, fr%u", instr.frD, instr.frA, instr.frB);
   state->result.operands = buffer;
   return true;
}

/* Floating Subtract Single */
bool fsubs(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "fsubs";
   state->result.name = "Floating Subtract Single";
   sprintf_s(buffer, 64, "fr%u, fr%u, fr%u", instr.frD, instr.frA, instr.frB);
   state->result.operands = buffer;
   return true;
}

/* Instruction Cache Block Invalidate */
bool icbi(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "icbi";
   state->result.name = "Instruction Cache Block Invalidate";
   sprintf_s(buffer, 64, "r%u, r%u", instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Instruction Synchronize */
bool isync(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "isync";
   state->result.name = "Instruction Synchronize";
   sprintf_s(buffer, 64, "r%u, r%u", instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Kernel Function Call */
bool krncall(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "krncall";
   state->result.name = "Kernel Function Call";
   sprintf_s(buffer, 64, "%u", instr.uimm);
   state->result.operands = buffer;
   return true;
}

/* Load Byte and Zero */
bool lbz(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "lbz";
   state->result.name = "Load Byte and Zero";
   sprintf_s(buffer, 64, "r%u, %lli(r%u)", instr.rD, bits::signExtend<16, uint64_t>(instr.d), instr.rA);
   state->result.operands = buffer;
   return true;
}

/* Load Byte and Zero with Update */
bool lbzu(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "lbzu";
   state->result.name = "Load Byte and Zero with Update";
   sprintf_s(buffer, 64, "r%u, %lli(r%u)", instr.rD, bits::signExtend<16, uint64_t>(instr.d), instr.rA);
   state->result.operands = buffer;
   return true;
}

/* Load Byte and Zero with Update Indexed */
bool lbzux(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "lbzux";
   state->result.name = "Load Byte and Zero with Update Indexed";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rD, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Load Byte and Zero Indexed */
bool lbzx(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "lbzx";
   state->result.name = "Load Byte and Zero Indexed";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rD, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Load Doubleword */
bool ld(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "ld";
   state->result.name = "Load Doubleword";
   sprintf_s(buffer, 64, "r%u, %lli(r%u)", instr.rD, bits::signExtend<16, uint64_t>(instr.ds << 2), instr.rA);
   state->result.operands = buffer;
   return true;
}

/* Load Doubleword and Reserve Indexed */
bool ldarx(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "ldarx";
   state->result.name = "Load Doubleword and Reserve Indexed";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rD, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Load Doubleword with Update */
bool ldu(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "ldu";
   state->result.name = "Load Doubleword with Update";
   sprintf_s(buffer, 64, "r%u, %lli(r%u)", instr.rD, bits::signExtend<16, uint64_t>(instr.ds << 2), instr.rA);
   state->result.operands = buffer;
   return true;
}

/* Load Doubleword with Update Indexed */
bool ldux(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "ldux";
   state->result.name = "Load Doubleword with Update Indexed";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rD, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Load Doubleword Indexed */
bool ldx(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "ldx";
   state->result.name = "Load Doubleword Indexed";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rD, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Load Floating-Point Double */
bool lfd(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "lfd";
   state->result.name = "Load Floating-Point Double";
   sprintf_s(buffer, 64, "fr%u, %lli(r%u)", instr.frD, bits::signExtend<16, uint64_t>(instr.d), instr.rA);
   state->result.operands = buffer;
   return true;
}

/* Load Floating-Point Double with Update */
bool lfdu(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "lfdu";
   state->result.name = "Load Floating-Point Double with Update";
   sprintf_s(buffer, 64, "fr%u, %lli(r%u)", instr.frD, bits::signExtend<16, uint64_t>(instr.d), instr.rA);
   state->result.operands = buffer;
   return true;
}

/* Load Floating-Point Double with Update Indexed */
bool lfdux(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "lfdux";
   state->result.name = "Load Floating-Point Double with Update Indexed";
   sprintf_s(buffer, 64, "fr%u, r%u, r%u", instr.frD, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Load Floating-Point Double Indexed */
bool lfdx(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "lfdx";
   state->result.name = "Load Floating-Point Double Indexed";
   sprintf_s(buffer, 64, "fr%u, r%u, r%u", instr.frD, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Load Floating-Point Single */
bool lfs(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "lfs";
   state->result.name = "Load Floating-Point Single";
   sprintf_s(buffer, 64, "fr%u, %lli(r%u)", instr.frD, bits::signExtend<16, uint64_t>(instr.d), instr.rA);
   state->result.operands = buffer;
   return true;
}

/* Load Floating-Point Single with Update */
bool lfsu(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "lfsu";
   state->result.name = "Load Floating-Point Single with Update";
   sprintf_s(buffer, 64, "fr%u, %lli(r%u)", instr.frD, bits::signExtend<16, uint64_t>(instr.d), instr.rA);
   state->result.operands = buffer;
   return true;
}

/* Load Floating-Point Single with Update Indexed */
bool lfsux(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "lfsux";
   state->result.name = "Load Floating-Point Single with Update Indexed";
   sprintf_s(buffer, 64, "fr%u, r%u, r%u", instr.frD, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Load Floating-Point Single Indexed */
bool lfsx(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "lfsx";
   state->result.name = "Load Floating-Point Single Indexed";
   sprintf_s(buffer, 64, "fr%u, r%u, r%u", instr.frD, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Load Halfword Algebraic */
bool lha(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "lha";
   state->result.name = "Load Halfword Algebraic";
   sprintf_s(buffer, 64, "r%u, %lli(r%u)", instr.rD, bits::signExtend<16, uint64_t>(instr.d), instr.rA);
   state->result.operands = buffer;
   return true;
}

/* Load Halfword Algebraic with Update */
bool lhau(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "lhau";
   state->result.name = "Load Halfword Algebraic with Update";
   sprintf_s(buffer, 64, "r%u, %lli(r%u)", instr.rD, bits::signExtend<16, uint64_t>(instr.d), instr.rA);
   state->result.operands = buffer;
   return true;
}

/* Load Halfword Algebraic with Update Indexed */
bool lhaux(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "lhaux";
   state->result.name = "Load Halfword Algebraic with Update Indexed";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rD, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Load Halfword Algebraic Indexed */
bool lhax(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "lhax";
   state->result.name = "Load Halfword Algebraic Indexed";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rD, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Load Halfword Byte-Reverse Indexed */
bool lhbrx(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "lhbrx";
   state->result.name = "Load Halfword Byte-Reverse Indexed";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rD, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Load Halfword and Zero */
bool lhz(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "lhz";
   state->result.name = "Load Halfword and Zero";
   sprintf_s(buffer, 64, "r%u, %lli(r%u)", instr.rD, bits::signExtend<16, uint64_t>(instr.d), instr.rA);
   state->result.operands = buffer;
   return true;
}

/* Load Halfword and Zero with Update */
bool lhzu(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "lhzu";
   state->result.name = "Load Halfword and Zero with Update";
   sprintf_s(buffer, 64, "r%u, %lli(r%u)", instr.rD, bits::signExtend<16, uint64_t>(instr.d), instr.rA);
   state->result.operands = buffer;
   return true;
}

/* Load Halfword and Zero with Update Indexed */
bool lhzux(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "lhzux";
   state->result.name = "Load Halfword and Zero with Update Indexed";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rD, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Load Halfword and Zero Indexed */
bool lhzx(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "lhzx";
   state->result.name = "Load Halfword and Zero Indexed";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rD, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Load Multiple Word */
bool lmw(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "lmw";
   state->result.name = "Load Multiple Word";
   sprintf_s(buffer, 64, "r%u, %lli(r%u)", instr.rD, bits::signExtend<16, uint64_t>(instr.d), instr.rA);
   state->result.operands = buffer;
   return true;
}

/* Load String Word Immediate */
bool lswi(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "lswi";
   state->result.name = "Load String Word Immediate";
   sprintf_s(buffer, 64, "r%u, r%u, %u", instr.rD, instr.rA, instr.nb);
   state->result.operands = buffer;
   return true;
}

/* Load String Word Indexed */
bool lswx(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "lswx";
   state->result.name = "Load String Word Indexed";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rD, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Load Word Algebraic */
bool lwa(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "lwa";
   state->result.name = "Load Word Algebraic";
   sprintf_s(buffer, 64, "r%u, %lli(r%u)", instr.rD, bits::signExtend<16, uint64_t>(instr.ds << 2), instr.rA);
   state->result.operands = buffer;
   return true;
}

/* Load Word and Reserve Indexed */
bool lwarx(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "lwarx";
   state->result.name = "Load Word and Reserve Indexed";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rD, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Load Word Algebraic with Update Indexed */
bool lwaux(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "lwaux";
   state->result.name = "Load Word Algebraic with Update Indexed";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rD, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Load Word Algebraic Indexed */
bool lwax(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "lwax";
   state->result.name = "Load Word Algebraic Indexed";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rD, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Load Word Byte-Reverse Indexed */
bool lwbrx(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "lwbrx";
   state->result.name = "Load Word Byte-Reverse Indexed";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rD, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Load Word and Zero */
bool lwz(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "lwz";
   state->result.name = "Load Word and Zero";
   sprintf_s(buffer, 64, "r%u, %lli(r%u)", instr.rD, bits::signExtend<16, uint64_t>(instr.d), instr.rA);
   state->result.operands = buffer;
   return true;
}

/* Load Word and Zero with Update */
bool lwzu(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "lwzu";
   state->result.name = "Load Word and Zero with Update";
   sprintf_s(buffer, 64, "r%u, %lli(r%u)", instr.rD, bits::signExtend<16, uint64_t>(instr.d), instr.rA);
   state->result.operands = buffer;
   return true;
}

/* Load Word and Zero with Update Indexed */
bool lwzux(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "lwzux";
   state->result.name = "Load Word and Zero with Update Indexed";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rD, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Load Word and Zero Indexed */
bool lwzx(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "lwzx";
   state->result.name = "Load Word and Zero Indexed";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rD, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Move Condition Register Field */
bool mcrf(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "mcrf";
   state->result.name = "Move Condition Register Field";
   sprintf_s(buffer, 64, "%u, %u", instr.crfD, instr.crfS);
   state->result.operands = buffer;
   return true;
}

/* Move to Condition Register from FPSCR */
bool mcrfs(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "mcrfs";
   state->result.name = "Move to Condition Register from FPSCR";
   sprintf_s(buffer, 64, "%u, %u", instr.crfD, instr.crfS);
   state->result.operands = buffer;
   return true;
}

/* Move from Condition Register */
bool mfcr(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "mfcr";
   state->result.name = "Move from Condition Register";
   sprintf_s(buffer, 64, "r%u", instr.rD);
   state->result.operands = buffer;
   return true;
}

/* Move from FPSCR */
bool mffs(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "mffs";
   state->result.name = "Move from FPSCR";
   sprintf_s(buffer, 64, "fr%u", instr.frD);
   state->result.operands = buffer;
   return true;
}

/* Move from Machine State Register */
bool mfmsr(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "mfmsr";
   state->result.name = "Move from Machine State Register";
   sprintf_s(buffer, 64, "r%u", instr.rD);
   state->result.operands = buffer;
   return true;
}

/* Move from One Condition Register Field */
bool mfocrf(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "mfocrf";
   state->result.name = "Move from One Condition Register Field";
   sprintf_s(buffer, 64, "r%u, %u", instr.rD, instr.crm);
   state->result.operands = buffer;
   return true;
}

/* Move from Special-Purpose Register */
bool mfspr(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "mfspr";
   state->result.name = "Move from Special-Purpose Register";
   sprintf_s(buffer, 64, "r%u, s%u", instr.rD, instr.spr);
   state->result.operands = buffer;
   return true;
}

/* Move from Segment Register */
bool mfsr(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "mfsr";
   state->result.name = "Move from Segment Register";
   sprintf_s(buffer, 64, "r%u, %u", instr.rD, instr.sr);
   state->result.operands = buffer;
   return true;
}

/* Move from Segment Register Indirect */
bool mfsrin(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "mfsrin";
   state->result.name = "Move from Segment Register Indirect";
   sprintf_s(buffer, 64, "r%u, r%u", instr.rD, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Move from Time Base */
bool mftb(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "mftb";
   state->result.name = "Move from Time Base";
   sprintf_s(buffer, 64, "r%u, %u", instr.rD, instr.tbr);
   state->result.operands = buffer;
   return true;
}

/* Move to Condition Register Fields */
bool mtcrf(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "mtcrf";
   state->result.name = "Move to Condition Register Fields";
   sprintf_s(buffer, 64, "%u, r%u", instr.crm, instr.rS);
   state->result.operands = buffer;
   return true;
}

/* Move to FPSCR Bit 0 */
bool mtfsb0(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "mtfsb0";
   state->result.name = "Move to FPSCR Bit 0";
   sprintf_s(buffer, 64, "%u", instr.crbD);
   state->result.operands = buffer;
   return true;
}

/* Move to FPSCR Bit 1 */
bool mtfsb1(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "mtfsb1";
   state->result.name = "Move to FPSCR Bit 1";
   sprintf_s(buffer, 64, "%u", instr.crbD);
   state->result.operands = buffer;
   return true;
}

/* Move to FPSCR Fields */
bool mtfsf(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "mtfsf";
   state->result.name = "Move to FPSCR Fields";
   sprintf_s(buffer, 64, "%u, fr%u", instr.fm, instr.frB);
   state->result.operands = buffer;
   return true;
}

/* Move to FPSCR Field Immediate */
bool mtfsfi(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "mtfsfi";
   state->result.name = "Move to FPSCR Field Immediate";
   sprintf_s(buffer, 64, "%u, %u", instr.crfD, instr.uimm);
   state->result.operands = buffer;
   return true;
}

/* Move to Machine State Register */
bool mtmsr(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "mtmsr";
   state->result.name = "Move to Machine State Register";
   sprintf_s(buffer, 64, "r%u, %u", instr.rS, instr.l15);
   state->result.operands = buffer;
   return true;
}

/* Move to Machine State Register Doubleword */
bool mtmsrd(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "mtmsrd";
   state->result.name = "Move to Machine State Register Doubleword";
   sprintf_s(buffer, 64, "r%u, %u", instr.rS, instr.l15);
   state->result.operands = buffer;
   return true;
}

/* Move to One Condition Register Field */
bool mtocrf(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "mtocrf";
   state->result.name = "Move to One Condition Register Field";
   sprintf_s(buffer, 64, "%u, r%u", instr.crm, instr.rS);
   state->result.operands = buffer;
   return true;
}

/* Move to Special-Purpose Register */
bool mtspr(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "mtspr";
   state->result.name = "Move to Special-Purpose Register";
   sprintf_s(buffer, 64, "s%u, r%u", instr.spr, instr.rS);
   state->result.operands = buffer;
   return true;
}

/* Move to Segment Register */
bool mtsr(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "mtsr";
   state->result.name = "Move to Segment Register";
   sprintf_s(buffer, 64, "%u, r%u", instr.sr, instr.rS);
   state->result.operands = buffer;
   return true;
}

/* Move to Segment Register Indirect */
bool mtsrin(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "mtsrin";
   state->result.name = "Move to Segment Register Indirect";
   sprintf_s(buffer, 64, "r%u, r%u", instr.rS, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Multiply High Doubleword */
bool mulhd(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "mulhd";
   state->result.name = "Multiply High Doubleword";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rD, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Multiply High Doubleword Unsigned */
bool mulhdu(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "mulhdu";
   state->result.name = "Multiply High Doubleword Unsigned";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rD, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Multiply High Word */
bool mulhw(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "mulhw";
   state->result.name = "Multiply High Word";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rD, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Multiply High Word Unsigned */
bool mulhwu(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "mulhwu";
   state->result.name = "Multiply High Word Unsigned";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rD, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Multiply Low Doubleword */
bool mulld(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "mulld";
   state->result.name = "Multiply Low Doubleword";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rD, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Multiply Low Immediate */
bool mulli(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "mulli";
   state->result.name = "Multiply Low Immediate";
   sprintf_s(buffer, 64, "r%u, r%u, %lli", instr.rD, instr.rA, bits::signExtend<16, uint64_t>(instr.simm));
   state->result.operands = buffer;
   return true;
}

/* Multiply Low Word */
bool mullw(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "mullw";
   state->result.name = "Multiply Low Word";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rD, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* NAND */
bool nand(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "nand";
   state->result.name = "NAND";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rA, instr.rS, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Negate */
bool neg(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "neg";
   state->result.name = "Negate";
   sprintf_s(buffer, 64, "r%u, r%u", instr.rD, instr.rA);
   state->result.operands = buffer;
   return true;
}

/* NOR */
bool nor(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "nor";
   state->result.name = "NOR";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rA, instr.rS, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* OR */
bool or(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "or";
   state->result.name = "OR";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rA, instr.rS, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* OR with Complement */
bool orc(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "orc";
   state->result.name = "OR with Complement";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rA, instr.rS, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* OR Immediate */
bool ori(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "ori";
   state->result.name = "OR Immediate";
   sprintf_s(buffer, 64, "r%u, r%u, %u", instr.rA, instr.rS, instr.uimm);
   state->result.operands = buffer;
   return true;
}

/* OR Immediate Shifted */
bool oris(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "oris";
   state->result.name = "OR Immediate Shifted";
   sprintf_s(buffer, 64, "r%u, r%u, %u", instr.rA, instr.rS, instr.uimm);
   state->result.operands = buffer;
   return true;
}

/* Return from Interrupt Doubleword */
bool rfid(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "rfid";
   state->result.name = "Return from Interrupt Doubleword";
   sprintf_s(buffer, 64, "");
   state->result.operands = buffer;
   return true;
}

/* Rotate Left Doubleword then Clear Left */
bool rldcl(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "rldcl";
   state->result.name = "Rotate Left Doubleword then Clear Left";
   sprintf_s(buffer, 64, "r%u, r%u, r%u, %u", instr.rA, instr.rS, instr.rB, instr.mb);
   state->result.operands = buffer;
   return true;
}

/* Rotate Left Doubleword then Clear Right */
bool rldcr(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "rldcr";
   state->result.name = "Rotate Left Doubleword then Clear Right";
   sprintf_s(buffer, 64, "r%u, r%u, r%u, %u", instr.rA, instr.rS, instr.rB, instr.mb);
   state->result.operands = buffer;
   return true;
}

/* Rotate Left Doubleword Immediate then Clear */
bool rldic(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "rldic";
   state->result.name = "Rotate Left Doubleword Immediate then Clear";
   sprintf_s(buffer, 64, "r%u, r%u, %u, %u", instr.rA, instr.rS, instr.sh, instr.mb);
   state->result.operands = buffer;
   return true;
}

/* Rotate Left Doubleword Immediate then Clear Left */
bool rldicl(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "rldicl";
   state->result.name = "Rotate Left Doubleword Immediate then Clear Left";
   sprintf_s(buffer, 64, "r%u, r%u, %u, %u", instr.rA, instr.rS, instr.sh, instr.mb);
   state->result.operands = buffer;
   return true;
}

/* Rotate Left Doubleword Immediate then Clear Right */
bool rldicr(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "rldicr";
   state->result.name = "Rotate Left Doubleword Immediate then Clear Right";
   sprintf_s(buffer, 64, "r%u, r%u, %u, %u", instr.rA, instr.rS, instr.sh, instr.mb);
   state->result.operands = buffer;
   return true;
}

/* Rotate Left Word Immediate then Mask Insert */
bool rlwimi(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "rlwimi";
   state->result.name = "Rotate Left Word Immediate then Mask Insert";
   sprintf_s(buffer, 64, "r%u, r%u, %u, %u, %u", instr.rA, instr.rS, instr.sh, instr.mb, instr.me);
   state->result.operands = buffer;
   return true;
}

/* Rotate Left Word Immediate then AND with Mask */
bool rlwinm(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "rlwinm";
   state->result.name = "Rotate Left Word Immediate then AND with Mask";
   sprintf_s(buffer, 64, "r%u, r%u, %u, %u, %u", instr.rA, instr.rS, instr.sh, instr.mb, instr.me);
   state->result.operands = buffer;
   return true;
}

/* Rotate Left Word then AND with Mask */
bool rlwnm(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "rlwnm";
   state->result.name = "Rotate Left Word then AND with Mask";
   sprintf_s(buffer, 64, "r%u, r%u, r%u, %u, %u", instr.rA, instr.rS, instr.rB, instr.mb, instr.me);
   state->result.operands = buffer;
   return true;
}

/* System Call */
bool sc(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "sc";
   state->result.name = "System Call";
   sprintf_s(buffer, 64, "");
   state->result.operands = buffer;
   return true;
}

/* SLB Invalidate All */
bool slbia(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "slbia";
   state->result.name = "SLB Invalidate All";
   sprintf_s(buffer, 64, "");
   state->result.operands = buffer;
   return true;
}

/* SLB Invalidate Entry */
bool slbie(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "slbie";
   state->result.name = "SLB Invalidate Entry";
   sprintf_s(buffer, 64, "r%u", instr.rB);
   state->result.operands = buffer;
   return true;
}

/* SLB Move From Entry ESID */
bool slbmfee(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "slbmfee";
   state->result.name = "SLB Move From Entry ESID";
   sprintf_s(buffer, 64, "r%u, r%u", instr.rD, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* SLB Move From Entry VSID */
bool slbmfev(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "slbmfev";
   state->result.name = "SLB Move From Entry VSID";
   sprintf_s(buffer, 64, "r%u, r%u", instr.rD, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* SLB Move To Entry */
bool slbmte(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "slbmte";
   state->result.name = "SLB Move To Entry";
   sprintf_s(buffer, 64, "r%u, r%u", instr.rS, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Shift Left Doubleword */
bool sld(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "sld";
   state->result.name = "Shift Left Doubleword";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rA, instr.rS, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Shift Left Word */
bool slw(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "slw";
   state->result.name = "Shift Left Word";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rA, instr.rS, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Shift Right Algebraic Doubleword */
bool srad(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "srad";
   state->result.name = "Shift Right Algebraic Doubleword";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rA, instr.rS, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Shift Right Algebraic Doubleword Immediate */
bool sradi(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "sradi";
   state->result.name = "Shift Right Algebraic Doubleword Immediate";
   sprintf_s(buffer, 64, "r%u, r%u, %u", instr.rA, instr.rS, instr.sh);
   state->result.operands = buffer;
   return true;
}

/* Shift Right Algebraic Word */
bool sraw(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "sraw";
   state->result.name = "Shift Right Algebraic Word";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rA, instr.rS, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Shift Right Algebraic Word Immediate */
bool srawi(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "srawi";
   state->result.name = "Shift Right Algebraic Word Immediate";
   sprintf_s(buffer, 64, "r%u, r%u, %u", instr.rA, instr.rS, instr.sh);
   state->result.operands = buffer;
   return true;
}

/* Shift Right Doubleword */
bool srd(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "srd";
   state->result.name = "Shift Right Doubleword";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rA, instr.rS, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Shift Right Word */
bool srw(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "srw";
   state->result.name = "Shift Right Word";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rA, instr.rS, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Store Byte */
bool stb(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "stb";
   state->result.name = "Store Byte";
   sprintf_s(buffer, 64, "r%u, %lli(r%u)", instr.rS, bits::signExtend<16, uint64_t>(instr.d), instr.rA);
   state->result.operands = buffer;
   return true;
}

/* Store Byte with Update */
bool stbu(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "stbu";
   state->result.name = "Store Byte with Update";
   sprintf_s(buffer, 64, "r%u, %lli(r%u)", instr.rS, bits::signExtend<16, uint64_t>(instr.d), instr.rA);
   state->result.operands = buffer;
   return true;
}

/* Store Byte with Update Indexed */
bool stbux(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "stbux";
   state->result.name = "Store Byte with Update Indexed";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rS, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Store Byte Indexed */
bool stbx(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "stbx";
   state->result.name = "Store Byte Indexed";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rS, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Store Doubleword */
bool std(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "std";
   state->result.name = "Store Doubleword";
   sprintf_s(buffer, 64, "r%u, %lli(r%u)", instr.rS, bits::signExtend<16, uint64_t>(instr.ds << 2), instr.rA);
   state->result.operands = buffer;
   return true;
}

/* Store Doubleword Conditional Indexed */
bool stdcx(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "stdcx";
   state->result.name = "Store Doubleword Conditional Indexed";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rS, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Store Doubleword with Update */
bool stdu(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "stdu";
   state->result.name = "Store Doubleword with Update";
   sprintf_s(buffer, 64, "r%u, %lli(r%u)", instr.rS, bits::signExtend<16, uint64_t>(instr.ds << 2), instr.rA);
   state->result.operands = buffer;
   return true;
}

/* Store Doubleword with Update Indexed */
bool stdux(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "stdux";
   state->result.name = "Store Doubleword with Update Indexed";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rS, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Store Doubleword Indexed */
bool stdx(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "stdx";
   state->result.name = "Store Doubleword Indexed";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rS, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Store Floating-Point Double */
bool stfd(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "stfd";
   state->result.name = "Store Floating-Point Double";
   sprintf_s(buffer, 64, "fr%u, %lli(r%u)", instr.frS, bits::signExtend<16, uint64_t>(instr.d), instr.rA);
   state->result.operands = buffer;
   return true;
}

/* Store Floating-Point Double with Update */
bool stfdu(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "stfdu";
   state->result.name = "Store Floating-Point Double with Update";
   sprintf_s(buffer, 64, "fr%u, %lli(r%u)", instr.frS, bits::signExtend<16, uint64_t>(instr.d), instr.rA);
   state->result.operands = buffer;
   return true;
}

/* Store Floating-Point Double with Update Indexed */
bool stfdux(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "stfdux";
   state->result.name = "Store Floating-Point Double with Update Indexed";
   sprintf_s(buffer, 64, "fr%u, r%u, r%u", instr.frS, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Store Floating-Point Double Indexed */
bool stfdx(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "stfdx";
   state->result.name = "Store Floating-Point Double Indexed";
   sprintf_s(buffer, 64, "fr%u, r%u, r%u", instr.frS, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Store Floating-Point as Integer Word Indexed */
bool stfiwx(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "stfiwx";
   state->result.name = "Store Floating-Point as Integer Word Indexed";
   sprintf_s(buffer, 64, "fr%u, r%u, r%u", instr.frS, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Store Floating-Point Single */
bool stfs(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "stfs";
   state->result.name = "Store Floating-Point Single";
   sprintf_s(buffer, 64, "fr%u, %lli(r%u)", instr.frS, bits::signExtend<16, uint64_t>(instr.d), instr.rA);
   state->result.operands = buffer;
   return true;
}

/* Store Floating-Point Single with Update */
bool stfsu(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "stfsu";
   state->result.name = "Store Floating-Point Single with Update";
   sprintf_s(buffer, 64, "fr%u, %lli(r%u)", instr.frS, bits::signExtend<16, uint64_t>(instr.d), instr.rA);
   state->result.operands = buffer;
   return true;
}

/* Store Floating-Point Single with Update Indexed */
bool stfsux(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "stfsux";
   state->result.name = "Store Floating-Point Single with Update Indexed";
   sprintf_s(buffer, 64, "fr%u, r%u, r%u", instr.frS, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Store Floating-Point Single Indexed */
bool stfsx(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "stfsx";
   state->result.name = "Store Floating-Point Single Indexed";
   sprintf_s(buffer, 64, "fr%u, r%u, r%u", instr.frS, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Store Halfword */
bool sth(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "sth";
   state->result.name = "Store Halfword";
   sprintf_s(buffer, 64, "r%u, %lli(r%u)", instr.rS, bits::signExtend<16, uint64_t>(instr.d), instr.rA);
   state->result.operands = buffer;
   return true;
}

/* Store Halfword Byte-Reverse Indexed */
bool sthbrx(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "sthbrx";
   state->result.name = "Store Halfword Byte-Reverse Indexed";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rS, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Store Halfword with Update */
bool sthu(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "sthu";
   state->result.name = "Store Halfword with Update";
   sprintf_s(buffer, 64, "r%u, %lli(r%u)", instr.rS, bits::signExtend<16, uint64_t>(instr.d), instr.rA);
   state->result.operands = buffer;
   return true;
}

/* Store Halfword with Update Indexed */
bool sthux(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "sthux";
   state->result.name = "Store Halfword with Update Indexed";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rS, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Store Halfword Indexed */
bool sthx(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "sthx";
   state->result.name = "Store Halfword Indexed";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rS, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Store Multiple Word */
bool stmw(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "stmw";
   state->result.name = "Store Multiple Word";
   sprintf_s(buffer, 64, "r%u, %lli(r%u)", instr.rS, bits::signExtend<16, uint64_t>(instr.d), instr.rA);
   state->result.operands = buffer;
   return true;
}

/* Store String Word Immediate */
bool stswi(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "stswi";
   state->result.name = "Store String Word Immediate";
   sprintf_s(buffer, 64, "r%u, r%u, %u", instr.rS, instr.rA, instr.nb);
   state->result.operands = buffer;
   return true;
}

/* Store String Word Indexed */
bool stswx(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "stswx";
   state->result.name = "Store String Word Indexed";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rS, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Store Word */
bool stw(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "stw";
   state->result.name = "Store Word";
   sprintf_s(buffer, 64, "r%u, %lli(r%u)", instr.rS, bits::signExtend<16, uint64_t>(instr.d), instr.rA);
   state->result.operands = buffer;
   return true;
}

/* Store Word Byte-Reverse Indexed */
bool stwbrx(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "stwbrx";
   state->result.name = "Store Word Byte-Reverse Indexed";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rS, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Store Word Conditional Indexed */
bool stwcx(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "stwcx";
   state->result.name = "Store Word Conditional Indexed";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rS, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Store Word with Update */
bool stwu(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "stwu";
   state->result.name = "Store Word with Update";
   sprintf_s(buffer, 64, "r%u, %lli(r%u)", instr.rS, bits::signExtend<16, uint64_t>(instr.d), instr.rA);
   state->result.operands = buffer;
   return true;
}

/* Store Word with Update Indexed */
bool stwux(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "stwux";
   state->result.name = "Store Word with Update Indexed";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rS, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Store Word Indexed */
bool stwx(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "stwx";
   state->result.name = "Store Word Indexed";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rS, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Subtract From */
bool subf(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "subf";
   state->result.name = "Subtract From";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rD, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Subtract From Carrying */
bool subfc(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "subfc";
   state->result.name = "Subtract From Carrying";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rD, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Subtract From Extended */
bool subfe(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "subfe";
   state->result.name = "Subtract From Extended";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rD, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Subtract From Immediate Carrying */
bool subfic(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "subfic";
   state->result.name = "Subtract From Immediate Carrying";
   sprintf_s(buffer, 64, "r%u, r%u, %lli", instr.rD, instr.rA, bits::signExtend<16, uint64_t>(instr.simm));
   state->result.operands = buffer;
   return true;
}

/* Subtract From Minus One Extended */
bool subfme(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "subfme";
   state->result.name = "Subtract From Minus One Extended";
   sprintf_s(buffer, 64, "r%u, r%u", instr.rD, instr.rA);
   state->result.operands = buffer;
   return true;
}

/* Subtract From Zero Extended */
bool subfze(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "subfze";
   state->result.name = "Subtract From Zero Extended";
   sprintf_s(buffer, 64, "r%u, r%u", instr.rD, instr.rA);
   state->result.operands = buffer;
   return true;
}

/* Synchronize */
bool sync(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "sync";
   state->result.name = "Synchronize";
   sprintf_s(buffer, 64, "%u", instr.syncl);
   state->result.operands = buffer;
   return true;
}

/* Trap Doubleword */
bool td(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "td";
   state->result.name = "Trap Doubleword";
   sprintf_s(buffer, 64, "%u, r%u, r%u", instr.to, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Trap Doubleword Immediate */
bool tdi(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "tdi";
   state->result.name = "Trap Doubleword Immediate";
   sprintf_s(buffer, 64, "%u, r%u, %lli", instr.to, instr.rA, bits::signExtend<16, uint64_t>(instr.simm));
   state->result.operands = buffer;
   return true;
}

/* Translation Lookaside Buffer Invalidate All */
bool tlbia(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "tlbia";
   state->result.name = "Translation Lookaside Buffer Invalidate All";
   sprintf_s(buffer, 64, "");
   state->result.operands = buffer;
   return true;
}

/* Translation Lookaside Buffer Invalidate Entry */
bool tlbie(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "tlbie";
   state->result.name = "Translation Lookaside Buffer Invalidate Entry";
   sprintf_s(buffer, 64, "r%u, %u", instr.rB, instr.tlbl);
   state->result.operands = buffer;
   return true;
}

/* Translation Lookaside Buffer Invalidate Entry Local */
bool tlbiel(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "tlbiel";
   state->result.name = "Translation Lookaside Buffer Invalidate Entry Local";
   sprintf_s(buffer, 64, "r%u, %u", instr.rB, instr.tlbl);
   state->result.operands = buffer;
   return true;
}

/* Translation Lookaside Buffer Synchronize */
bool tlbsync(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "tlbsync";
   state->result.name = "Translation Lookaside Buffer Synchronize";
   sprintf_s(buffer, 64, "");
   state->result.operands = buffer;
   return true;
}

/* Trap Word */
bool tw(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "tw";
   state->result.name = "Trap Word";
   sprintf_s(buffer, 64, "%u, r%u, r%u", instr.to, instr.rA, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* Trap Word Immediate */
bool twi(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "twi";
   state->result.name = "Trap Word Immediate";
   sprintf_s(buffer, 64, "%u, r%u, %lli", instr.to, instr.rA, bits::signExtend<16, uint64_t>(instr.simm));
   state->result.operands = buffer;
   return true;
}

/* XOR */
bool xor(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "xor";
   state->result.name = "XOR";
   sprintf_s(buffer, 64, "r%u, r%u, r%u", instr.rA, instr.rS, instr.rB);
   state->result.operands = buffer;
   return true;
}

/* XOR Immediate */
bool xori(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "xori";
   state->result.name = "XOR Immediate";
   sprintf_s(buffer, 64, "r%u, r%u, %u", instr.rA, instr.rS, instr.uimm);
   state->result.operands = buffer;
   return true;
}

/* XOR Immediate Shifted */
bool xoris(State *state, Instruction instr)
{
   char buffer[64];
   state->result.code = "xoris";
   state->result.name = "XOR Immediate Shifted";
   sprintf_s(buffer, 64, "r%u, r%u, %u", instr.rA, instr.rS, instr.uimm);
   state->result.operands = buffer;
   return true;
}

