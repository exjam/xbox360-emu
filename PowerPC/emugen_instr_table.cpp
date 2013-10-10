
typedef bool (*__emugen_instr_fptr)(State* state, Instruction instr);

template<int bits>
struct __emugen_table 
{
   __emugen_table()
   {
      memset(this, 0, sizeof(__emugen_table<bits>));
   }

   __emugen_instr_fptr& operator[](int i)
   {
      return m_table[i];
   }

   __emugen_instr_fptr m_table[1 << bits];
};

static __emugen_table<6> __emugen_table_opcd;
static __emugen_table<10> __emugen_table_opcd_19_xo2;
static __emugen_table<10> __emugen_table_opcd_31_xo2;
static __emugen_table<10> &__emugen_table_opcd_31_xo3 = __emugen_table_opcd_31_xo2;
static __emugen_table<10> __emugen_table_opcd_59_xo2;
static __emugen_table<10> &__emugen_table_opcd_59_xo4 = __emugen_table_opcd_59_xo2;
static __emugen_table<1> __emugen_table_opcd_62_rc;
static __emugen_table<10> __emugen_table_opcd_63_xo2;
static __emugen_table<10> &__emugen_table_opcd_63_xo4 = __emugen_table_opcd_63_xo2;

bool decode(State *state, Instruction instr)
{
   unsigned int val = instr.value;
   unsigned int op;

   op = (val >> 26) & 0x3f;
   if (__emugen_table_opcd[op]) {
      return __emugen_table_opcd[op](state, instr);
   }

   return false;
}

bool decode_opcd_19(State *state, Instruction instr)
{
   unsigned int val = instr.value;
   unsigned int op;

   op = (val >> 1) & 0x3ff;
   if (__emugen_table_opcd_19_xo2[op]) {
      return __emugen_table_opcd_19_xo2[op](state, instr);
   }

   return false;
}

bool decode_opcd_31(State *state, Instruction instr)
{
   unsigned int val = instr.value;
   unsigned int op;

   op = (val >> 1) & 0x3ff;
   if (__emugen_table_opcd_31_xo2[op]) {
      return __emugen_table_opcd_31_xo2[op](state, instr);
   }

   op = (val >> 1) & 0x1ff;
   if (__emugen_table_opcd_31_xo3[op]) {
      return __emugen_table_opcd_31_xo3[op](state, instr);
   }

   return false;
}

bool decode_opcd_59(State *state, Instruction instr)
{
   unsigned int val = instr.value;
   unsigned int op;

   op = (val >> 1) & 0x3ff;
   if (__emugen_table_opcd_59_xo2[op]) {
      return __emugen_table_opcd_59_xo2[op](state, instr);
   }

   op = (val >> 1) & 0x1f;
   if (__emugen_table_opcd_59_xo4[op]) {
      return __emugen_table_opcd_59_xo4[op](state, instr);
   }

   return false;
}

bool decode_opcd_62(State *state, Instruction instr)
{
   unsigned int val = instr.value;
   unsigned int op;

   op = (val >> 0) & 0x1;
   if (__emugen_table_opcd_62_rc[op]) {
      return __emugen_table_opcd_62_rc[op](state, instr);
   }

   return false;
}

bool decode_opcd_63(State *state, Instruction instr)
{
   unsigned int val = instr.value;
   unsigned int op;

   op = (val >> 1) & 0x3ff;
   if (__emugen_table_opcd_63_xo2[op]) {
      return __emugen_table_opcd_63_xo2[op](state, instr);
   }

   op = (val >> 1) & 0x1f;
   if (__emugen_table_opcd_63_xo4[op]) {
      return __emugen_table_opcd_63_xo4[op](state, instr);
   }

   return false;
}

bool init()
{
   __emugen_table_opcd[10] = cmpli;
   __emugen_table_opcd[11] = cmpi;
   __emugen_table_opcd[12] = addic;
   __emugen_table_opcd[13] = addico;
   __emugen_table_opcd[14] = addi;
   __emugen_table_opcd[15] = addis;
   __emugen_table_opcd[16] = bc;
   __emugen_table_opcd[18] = b;
   __emugen_table_opcd[28] = andio;
   __emugen_table_opcd[29] = andiso;
   __emugen_table_opcd[36] = stw;
   __emugen_table_opcd[37] = stwu;
   __emugen_table_opcd[38] = stb;
   __emugen_table_opcd[39] = stbu;
   __emugen_table_opcd[44] = sth;
   __emugen_table_opcd[45] = sthu;
   __emugen_table_opcd[19] = decode_opcd_19;
   __emugen_table_opcd_19_xo2[16] = bclr;
   __emugen_table_opcd_19_xo2[33] = crnor;
   __emugen_table_opcd_19_xo2[129] = crandc;
   __emugen_table_opcd_19_xo2[193] = crxor;
   __emugen_table_opcd_19_xo2[225] = crnand;
   __emugen_table_opcd_19_xo2[257] = crand;
   __emugen_table_opcd_19_xo2[289] = creqv;
   __emugen_table_opcd_19_xo2[417] = crorc;
   __emugen_table_opcd_19_xo2[449] = cror;
   __emugen_table_opcd_19_xo2[528] = bcctr;
   __emugen_table_opcd[31] = decode_opcd_31;
   __emugen_table_opcd_31_xo2[0] = cmp;
   __emugen_table_opcd_31_xo2[26] = cntlzw;
   __emugen_table_opcd_31_xo2[32] = cmpl;
   __emugen_table_opcd_31_xo2[54] = dcbst;
   __emugen_table_opcd_31_xo2[58] = cntlzd;
   __emugen_table_opcd_31_xo2[86] = dcbf;
   __emugen_table_opcd_31_xo2[150] = isync;
   __emugen_table_opcd_31_xo2[246] = dcbtst;
   __emugen_table_opcd_31_xo2[278] = dcbt;
   __emugen_table_opcd_31_xo2[310] = eciwx;
   __emugen_table_opcd_31_xo2[339] = mfspr;
   __emugen_table_opcd_31_xo2[438] = ecowx;
   __emugen_table_opcd_31_xo2[854] = eieio;
   __emugen_table_opcd_31_xo2[922] = extsh;
   __emugen_table_opcd_31_xo2[954] = extsb;
   __emugen_table_opcd_31_xo2[982] = icbi;
   __emugen_table_opcd_31_xo2[986] = extsw;
   __emugen_table_opcd_31_xo2[1014] = dcbz;
   __emugen_table_opcd[31] = decode_opcd_31;
   __emugen_table_opcd_31_xo3[10] = addc;
   __emugen_table_opcd_31_xo3[28] = and;
   __emugen_table_opcd_31_xo3[60] = andc;
   __emugen_table_opcd_31_xo3[138] = adde;
   __emugen_table_opcd_31_xo3[202] = addze;
   __emugen_table_opcd_31_xo3[234] = addme;
   __emugen_table_opcd_31_xo3[266] = add;
   __emugen_table_opcd_31_xo3[284] = eqv;
   __emugen_table_opcd_31_xo3[457] = divdu;
   __emugen_table_opcd_31_xo3[459] = divwu;
   __emugen_table_opcd_31_xo3[489] = divd;
   __emugen_table_opcd_31_xo3[491] = divw;
   __emugen_table_opcd[59] = decode_opcd_59;
   __emugen_table_opcd_59_xo2[18] = fdivs;
   __emugen_table_opcd_59_xo2[20] = fsubs;
   __emugen_table_opcd_59_xo2[21] = fadds;
   __emugen_table_opcd_59_xo2[22] = fsqrts;
   __emugen_table_opcd[59] = decode_opcd_59;
   __emugen_table_opcd_59_xo4[24] = fres;
   __emugen_table_opcd_59_xo4[25] = fmuls;
   __emugen_table_opcd_59_xo4[28] = fmsubs;
   __emugen_table_opcd_59_xo4[29] = fmadds;
   __emugen_table_opcd_59_xo4[30] = fnmsubs;
   __emugen_table_opcd_59_xo4[31] = fnmadds;
   __emugen_table_opcd[62] = decode_opcd_62;
   __emugen_table_opcd_62_rc[0] = std;
   __emugen_table_opcd_62_rc[1] = stdu;
   __emugen_table_opcd[63] = decode_opcd_63;
   __emugen_table_opcd_63_xo2[0] = fcmpu;
   __emugen_table_opcd_63_xo2[12] = frsp;
   __emugen_table_opcd_63_xo2[14] = fctiw;
   __emugen_table_opcd_63_xo2[15] = fctiwz;
   __emugen_table_opcd_63_xo2[18] = fdiv;
   __emugen_table_opcd_63_xo2[20] = fsub;
   __emugen_table_opcd_63_xo2[21] = fadd;
   __emugen_table_opcd_63_xo2[22] = fsqrt;
   __emugen_table_opcd_63_xo2[32] = fcmpo;
   __emugen_table_opcd_63_xo2[40] = fneg;
   __emugen_table_opcd_63_xo2[72] = fmr;
   __emugen_table_opcd_63_xo2[136] = fnabs;
   __emugen_table_opcd_63_xo2[264] = fabs;
   __emugen_table_opcd_63_xo2[814] = fctid;
   __emugen_table_opcd_63_xo2[815] = fctidz;
   __emugen_table_opcd_63_xo2[846] = fcfid;
   __emugen_table_opcd[63] = decode_opcd_63;
   __emugen_table_opcd_63_xo4[23] = fsel;
   __emugen_table_opcd_63_xo4[25] = fmul;
   __emugen_table_opcd_63_xo4[26] = frsqrte;
   __emugen_table_opcd_63_xo4[28] = fmsub;
   __emugen_table_opcd_63_xo4[29] = fmadd;
   __emugen_table_opcd_63_xo4[30] = fnmsub;
   __emugen_table_opcd_63_xo4[31] = fnmadd;
   return true;
}
