
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
static __emugen_table<4> __emugen_table_opcd_30_xo7;
static __emugen_table<4> &__emugen_table_opcd_30_xo8 = __emugen_table_opcd_30_xo7;
static __emugen_table<10> __emugen_table_opcd_31_xo2;
static __emugen_table<10> &__emugen_table_opcd_31_xo3 = __emugen_table_opcd_31_xo2;
static __emugen_table<1> __emugen_table_opcd_31_xo2_19_xo6;
static __emugen_table<1> __emugen_table_opcd_31_xo2_144_xo6;
static __emugen_table<2> __emugen_table_opcd_58_xo5;
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

bool decode_opcd_30(State *state, Instruction instr)
{
   unsigned int val = instr.value;
   unsigned int op;

   op = (val >> 1) & 0xf;
   if (__emugen_table_opcd_30_xo7[op]) {
      return __emugen_table_opcd_30_xo7[op](state, instr);
   }

   op = (val >> 2) & 0x7;
   if (__emugen_table_opcd_30_xo8[op]) {
      return __emugen_table_opcd_30_xo8[op](state, instr);
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

bool decode_opcd_31_xo2_19(State *state, Instruction instr)
{
   unsigned int val = instr.value;
   unsigned int op;

   op = (val >> 20) & 0x1;
   if (__emugen_table_opcd_31_xo2_19_xo6[op]) {
      return __emugen_table_opcd_31_xo2_19_xo6[op](state, instr);
   }

   return false;
}

bool decode_opcd_31_xo2_144(State *state, Instruction instr)
{
   unsigned int val = instr.value;
   unsigned int op;

   op = (val >> 20) & 0x1;
   if (__emugen_table_opcd_31_xo2_144_xo6[op]) {
      return __emugen_table_opcd_31_xo2_144_xo6[op](state, instr);
   }

   return false;
}

bool decode_opcd_58(State *state, Instruction instr)
{
   unsigned int val = instr.value;
   unsigned int op;

   op = (val >> 0) & 0x3;
   if (__emugen_table_opcd_58_xo5[op]) {
      return __emugen_table_opcd_58_xo5[op](state, instr);
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
   __emugen_table_opcd[2] = tdi;
   __emugen_table_opcd[3] = twi;
   __emugen_table_opcd[7] = mulli;
   __emugen_table_opcd[8] = subfic;
   __emugen_table_opcd[10] = cmpli;
   __emugen_table_opcd[11] = cmpi;
   __emugen_table_opcd[12] = addic;
   __emugen_table_opcd[13] = addico;
   __emugen_table_opcd[14] = addi;
   __emugen_table_opcd[15] = addis;
   __emugen_table_opcd[16] = bc;
   __emugen_table_opcd[17] = sc;
   __emugen_table_opcd[18] = b;
   __emugen_table_opcd[19] = mcrf;
   __emugen_table_opcd[20] = rlwimi;
   __emugen_table_opcd[21] = rlwinm;
   __emugen_table_opcd[23] = rlwnm;
   __emugen_table_opcd[24] = ori;
   __emugen_table_opcd[25] = oris;
   __emugen_table_opcd[26] = xori;
   __emugen_table_opcd[27] = xoris;
   __emugen_table_opcd[28] = andio;
   __emugen_table_opcd[29] = andiso;
   __emugen_table_opcd[32] = lwz;
   __emugen_table_opcd[33] = lwzu;
   __emugen_table_opcd[34] = lbz;
   __emugen_table_opcd[35] = lbzu;
   __emugen_table_opcd[36] = stw;
   __emugen_table_opcd[37] = stwu;
   __emugen_table_opcd[38] = stb;
   __emugen_table_opcd[39] = stbu;
   __emugen_table_opcd[40] = lhz;
   __emugen_table_opcd[41] = lhzu;
   __emugen_table_opcd[42] = lha;
   __emugen_table_opcd[43] = lhau;
   __emugen_table_opcd[44] = sth;
   __emugen_table_opcd[45] = sthu;
   __emugen_table_opcd[46] = lmw;
   __emugen_table_opcd[47] = stmw;
   __emugen_table_opcd[48] = lfs;
   __emugen_table_opcd[49] = lfsu;
   __emugen_table_opcd[50] = lfd;
   __emugen_table_opcd[51] = lfdu;
   __emugen_table_opcd[52] = stfs;
   __emugen_table_opcd[53] = stfsu;
   __emugen_table_opcd[54] = stfd;
   __emugen_table_opcd[55] = stfdu;
   __emugen_table_opcd[63] = mcrfs;
   __emugen_table_opcd[19] = decode_opcd_19;
   __emugen_table_opcd_19_xo2[16] = bclr;
   __emugen_table_opcd_19_xo2[18] = rfid;
   __emugen_table_opcd_19_xo2[33] = crnor;
   __emugen_table_opcd_19_xo2[129] = crandc;
   __emugen_table_opcd_19_xo2[193] = crxor;
   __emugen_table_opcd_19_xo2[225] = crnand;
   __emugen_table_opcd_19_xo2[257] = crand;
   __emugen_table_opcd_19_xo2[289] = creqv;
   __emugen_table_opcd_19_xo2[417] = crorc;
   __emugen_table_opcd_19_xo2[449] = cror;
   __emugen_table_opcd_19_xo2[528] = bcctr;
   __emugen_table_opcd[30] = decode_opcd_30;
   __emugen_table_opcd_30_xo7[8] = rldcl;
   __emugen_table_opcd_30_xo7[9] = rldcr;
   __emugen_table_opcd[30] = decode_opcd_30;
   __emugen_table_opcd_30_xo8[0] = rldicl;
   __emugen_table_opcd_30_xo8[1] = rldicr;
   __emugen_table_opcd_30_xo8[2] = rldic;
   __emugen_table_opcd[31] = decode_opcd_31;
   __emugen_table_opcd_31_xo2[0] = cmp;
   __emugen_table_opcd_31_xo2[4] = tw;
   __emugen_table_opcd_31_xo2[20] = lwarx;
   __emugen_table_opcd_31_xo2[21] = ldx;
   __emugen_table_opcd_31_xo2[23] = lwzx;
   __emugen_table_opcd_31_xo2[24] = slw;
   __emugen_table_opcd_31_xo2[26] = cntlzw;
   __emugen_table_opcd_31_xo2[27] = sld;
   __emugen_table_opcd_31_xo2[28] = and;
   __emugen_table_opcd_31_xo2[31] = td;
   __emugen_table_opcd_31_xo2[32] = cmpl;
   __emugen_table_opcd_31_xo2[53] = ldux;
   __emugen_table_opcd_31_xo2[54] = dcbst;
   __emugen_table_opcd_31_xo2[55] = lwzux;
   __emugen_table_opcd_31_xo2[58] = cntlzd;
   __emugen_table_opcd_31_xo2[60] = andc;
   __emugen_table_opcd_31_xo2[83] = mfmsr;
   __emugen_table_opcd_31_xo2[84] = ldarx;
   __emugen_table_opcd_31_xo2[86] = dcbf;
   __emugen_table_opcd_31_xo2[87] = lbzx;
   __emugen_table_opcd_31_xo2[119] = lbzux;
   __emugen_table_opcd_31_xo2[124] = nor;
   __emugen_table_opcd_31_xo2[146] = mtmsr;
   __emugen_table_opcd_31_xo2[149] = stdx;
   __emugen_table_opcd_31_xo2[150] = isync;
   __emugen_table_opcd_31_xo2[151] = stwx;
   __emugen_table_opcd_31_xo2[178] = mtmsrd;
   __emugen_table_opcd_31_xo2[181] = stdux;
   __emugen_table_opcd_31_xo2[183] = stwux;
   __emugen_table_opcd_31_xo2[210] = mtsr;
   __emugen_table_opcd_31_xo2[214] = stdcx;
   __emugen_table_opcd_31_xo2[242] = mtsrin;
   __emugen_table_opcd_31_xo2[246] = dcbtst;
   __emugen_table_opcd_31_xo2[274] = tlbiel;
   __emugen_table_opcd_31_xo2[278] = dcbt;
   __emugen_table_opcd_31_xo2[279] = lhzx;
   __emugen_table_opcd_31_xo2[306] = tlbie;
   __emugen_table_opcd_31_xo2[310] = eciwx;
   __emugen_table_opcd_31_xo2[311] = lhzux;
   __emugen_table_opcd_31_xo2[316] = xor;
   __emugen_table_opcd_31_xo2[339] = mfspr;
   __emugen_table_opcd_31_xo2[341] = lwax;
   __emugen_table_opcd_31_xo2[343] = lhax;
   __emugen_table_opcd_31_xo2[370] = tlbia;
   __emugen_table_opcd_31_xo2[371] = mftb;
   __emugen_table_opcd_31_xo2[373] = lwaux;
   __emugen_table_opcd_31_xo2[375] = lhaux;
   __emugen_table_opcd_31_xo2[402] = slbmte;
   __emugen_table_opcd_31_xo2[407] = sthx;
   __emugen_table_opcd_31_xo2[412] = orc;
   __emugen_table_opcd_31_xo2[434] = slbie;
   __emugen_table_opcd_31_xo2[438] = ecowx;
   __emugen_table_opcd_31_xo2[439] = sthux;
   __emugen_table_opcd_31_xo2[444] = or;
   __emugen_table_opcd_31_xo2[467] = mtspr;
   __emugen_table_opcd_31_xo2[476] = nand;
   __emugen_table_opcd_31_xo2[498] = slbia;
   __emugen_table_opcd_31_xo2[533] = lswx;
   __emugen_table_opcd_31_xo2[534] = lwbrx;
   __emugen_table_opcd_31_xo2[535] = lfsx;
   __emugen_table_opcd_31_xo2[536] = srw;
   __emugen_table_opcd_31_xo2[539] = srd;
   __emugen_table_opcd_31_xo2[566] = tlbsync;
   __emugen_table_opcd_31_xo2[567] = lfsux;
   __emugen_table_opcd_31_xo2[595] = mfsr;
   __emugen_table_opcd_31_xo2[597] = lswi;
   __emugen_table_opcd_31_xo2[598] = sync;
   __emugen_table_opcd_31_xo2[599] = lfdx;
   __emugen_table_opcd_31_xo2[631] = lfdux;
   __emugen_table_opcd_31_xo2[659] = mfsrin;
   __emugen_table_opcd_31_xo2[661] = stswx;
   __emugen_table_opcd_31_xo2[662] = stwbrx;
   __emugen_table_opcd_31_xo2[663] = stfsx;
   __emugen_table_opcd_31_xo2[725] = stswi;
   __emugen_table_opcd_31_xo2[727] = stfdx;
   __emugen_table_opcd_31_xo2[759] = stfdux;
   __emugen_table_opcd_31_xo2[790] = lhbrx;
   __emugen_table_opcd_31_xo2[792] = sraw;
   __emugen_table_opcd_31_xo2[794] = srad;
   __emugen_table_opcd_31_xo2[795] = stfsux;
   __emugen_table_opcd_31_xo2[824] = srawi;
   __emugen_table_opcd_31_xo2[826] = sradi;
   __emugen_table_opcd_31_xo2[827] = sradi;
   __emugen_table_opcd_31_xo2[851] = slbmfev;
   __emugen_table_opcd_31_xo2[854] = eieio;
   __emugen_table_opcd_31_xo2[915] = slbmfee;
   __emugen_table_opcd_31_xo2[918] = sthbrx;
   __emugen_table_opcd_31_xo2[922] = extsh;
   __emugen_table_opcd_31_xo2[954] = extsb;
   __emugen_table_opcd_31_xo2[982] = icbi;
   __emugen_table_opcd_31_xo2[983] = stfiwx;
   __emugen_table_opcd_31_xo2[986] = extsw;
   __emugen_table_opcd_31_xo2[1014] = dcbz;
   __emugen_table_opcd_31_xo2[19] = decode_opcd_31_xo2_19;
   __emugen_table_opcd_31_xo2_19_xo6[0] = mfcr;
   __emugen_table_opcd_31_xo2_19_xo6[1] = mfocrf;
   __emugen_table_opcd_31_xo2[144] = decode_opcd_31_xo2_144;
   __emugen_table_opcd_31_xo2_144_xo6[0] = mtcrf;
   __emugen_table_opcd_31_xo2_144_xo6[1] = mtocrf;
   __emugen_table_opcd[31] = decode_opcd_31;
   __emugen_table_opcd_31_xo3[8] = subfc;
   __emugen_table_opcd_31_xo3[9] = mulhdu;
   __emugen_table_opcd_31_xo3[10] = addc;
   __emugen_table_opcd_31_xo3[11] = mulhwu;
   __emugen_table_opcd_31_xo3[31] = neg;
   __emugen_table_opcd_31_xo3[40] = subf;
   __emugen_table_opcd_31_xo3[73] = mulhd;
   __emugen_table_opcd_31_xo3[75] = mulhw;
   __emugen_table_opcd_31_xo3[136] = subfe;
   __emugen_table_opcd_31_xo3[138] = adde;
   __emugen_table_opcd_31_xo3[200] = subfze;
   __emugen_table_opcd_31_xo3[202] = addze;
   __emugen_table_opcd_31_xo3[215] = stbx;
   __emugen_table_opcd_31_xo3[232] = subfme;
   __emugen_table_opcd_31_xo3[233] = mulld;
   __emugen_table_opcd_31_xo3[234] = addme;
   __emugen_table_opcd_31_xo3[235] = mullw;
   __emugen_table_opcd_31_xo3[247] = stbux;
   __emugen_table_opcd_31_xo3[266] = add;
   __emugen_table_opcd_31_xo3[284] = eqv;
   __emugen_table_opcd_31_xo3[457] = divdu;
   __emugen_table_opcd_31_xo3[459] = divwu;
   __emugen_table_opcd_31_xo3[489] = divd;
   __emugen_table_opcd_31_xo3[491] = divw;
   __emugen_table_opcd[58] = decode_opcd_58;
   __emugen_table_opcd_58_xo5[0] = ld;
   __emugen_table_opcd_58_xo5[1] = ldu;
   __emugen_table_opcd_58_xo5[2] = lwa;
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
   __emugen_table_opcd_63_xo2[38] = mtfsb1;
   __emugen_table_opcd_63_xo2[40] = fneg;
   __emugen_table_opcd_63_xo2[70] = mtfsb0;
   __emugen_table_opcd_63_xo2[72] = fmr;
   __emugen_table_opcd_63_xo2[134] = mtfsfi;
   __emugen_table_opcd_63_xo2[136] = fnabs;
   __emugen_table_opcd_63_xo2[264] = fabs;
   __emugen_table_opcd_63_xo2[583] = mffs;
   __emugen_table_opcd_63_xo2[711] = mtfsf;
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
