DecodeEntry table_xo1[64];
DecodeEntry table_xo1_19_xo2[1024];
DecodeEntry table_xo1_30_xo7[16];
DecodeEntry *table_xo1_30_xo8 = table_xo1_30_xo7;
DecodeEntry table_xo1_31_xo2[1024];
DecodeEntry *table_xo1_31_xo3 = table_xo1_31_xo2;
DecodeEntry table_xo1_31_xo2_19_xo6[2];
DecodeEntry table_xo1_31_xo2_144_xo6[2];
DecodeEntry table_xo1_58_xo5[4];
DecodeEntry table_xo1_59_xo2[1024];
DecodeEntry *table_xo1_59_xo4 = table_xo1_59_xo2;
DecodeEntry table_xo1_62_xo5[4];
DecodeEntry table_xo1_63_xo2[1024];
DecodeEntry *table_xo1_63_xo4 = table_xo1_63_xo2;

InstructionID decode(Instruction instr) {
   DecodeEntry entry;

   entry = table_xo1[instr.xo1];
   if (entry.value) {
      if (entry.isTable()) {
         return entry.table(instr);
      } else {
         return entry.instr();
      }
   }

   return InstructionID::Unknown;
}

InstructionID decode_xo1_19(Instruction instr) {
   DecodeEntry entry;

   entry = table_xo1_19_xo2[instr.xo2];
   if (entry.value) {
      if (entry.isTable()) {
         return entry.table(instr);
      } else {
         return entry.instr();
      }
   }

   return InstructionID::Unknown;
}

InstructionID decode_xo1_30(Instruction instr) {
   DecodeEntry entry;

   entry = table_xo1_30_xo7[instr.xo7];
   if (entry.value) {
      if (entry.isTable()) {
         return entry.table(instr);
      } else {
         return entry.instr();
      }
   }

   entry = table_xo1_30_xo8[instr.xo8];
   if (entry.value) {
      if (entry.isTable()) {
         return entry.table(instr);
      } else {
         return entry.instr();
      }
   }

   return InstructionID::Unknown;
}

InstructionID decode_xo1_31(Instruction instr) {
   DecodeEntry entry;

   entry = table_xo1_31_xo2[instr.xo2];
   if (entry.value) {
      if (entry.isTable()) {
         return entry.table(instr);
      } else {
         return entry.instr();
      }
   }

   entry = table_xo1_31_xo3[instr.xo3];
   if (entry.value) {
      if (entry.isTable()) {
         return entry.table(instr);
      } else {
         return entry.instr();
      }
   }

   return InstructionID::Unknown;
}

InstructionID decode_xo1_31_xo2_19(Instruction instr) {
   DecodeEntry entry;

   entry = table_xo1_31_xo2_19_xo6[instr.xo6];
   if (entry.value) {
      if (entry.isTable()) {
         return entry.table(instr);
      } else {
         return entry.instr();
      }
   }

   return InstructionID::Unknown;
}

InstructionID decode_xo1_31_xo2_144(Instruction instr) {
   DecodeEntry entry;

   entry = table_xo1_31_xo2_144_xo6[instr.xo6];
   if (entry.value) {
      if (entry.isTable()) {
         return entry.table(instr);
      } else {
         return entry.instr();
      }
   }

   return InstructionID::Unknown;
}

InstructionID decode_xo1_58(Instruction instr) {
   DecodeEntry entry;

   entry = table_xo1_58_xo5[instr.xo5];
   if (entry.value) {
      if (entry.isTable()) {
         return entry.table(instr);
      } else {
         return entry.instr();
      }
   }

   return InstructionID::Unknown;
}

InstructionID decode_xo1_59(Instruction instr) {
   DecodeEntry entry;

   entry = table_xo1_59_xo2[instr.xo2];
   if (entry.value) {
      if (entry.isTable()) {
         return entry.table(instr);
      } else {
         return entry.instr();
      }
   }

   entry = table_xo1_59_xo4[instr.xo4];
   if (entry.value) {
      if (entry.isTable()) {
         return entry.table(instr);
      } else {
         return entry.instr();
      }
   }

   return InstructionID::Unknown;
}

InstructionID decode_xo1_62(Instruction instr) {
   DecodeEntry entry;

   entry = table_xo1_62_xo5[instr.xo5];
   if (entry.value) {
      if (entry.isTable()) {
         return entry.table(instr);
      } else {
         return entry.instr();
      }
   }

   return InstructionID::Unknown;
}

InstructionID decode_xo1_63(Instruction instr) {
   DecodeEntry entry;

   entry = table_xo1_63_xo2[instr.xo2];
   if (entry.value) {
      if (entry.isTable()) {
         return entry.table(instr);
      } else {
         return entry.instr();
      }
   }

   entry = table_xo1_63_xo4[instr.xo4];
   if (entry.value) {
      if (entry.isTable()) {
         return entry.table(instr);
      } else {
         return entry.instr();
      }
   }

   return InstructionID::Unknown;
}


bool init()
{
   table_xo1[2] = InstructionID::tdi;
   table_xo1[3] = InstructionID::twi;
   table_xo1[5] = InstructionID::krncall;
   table_xo1[7] = InstructionID::mulli;
   table_xo1[8] = InstructionID::subfic;
   table_xo1[10] = InstructionID::cmpli;
   table_xo1[11] = InstructionID::cmpi;
   table_xo1[12] = InstructionID::addic;
   table_xo1[13] = InstructionID::addico;
   table_xo1[14] = InstructionID::addi;
   table_xo1[15] = InstructionID::addis;
   table_xo1[16] = InstructionID::bc;
   table_xo1[17] = InstructionID::sc;
   table_xo1[18] = InstructionID::b;
   table_xo1[19] = decode_xo1_19;
   table_xo1[20] = InstructionID::rlwimi;
   table_xo1[21] = InstructionID::rlwinm;
   table_xo1[23] = InstructionID::rlwnm;
   table_xo1[24] = InstructionID::ori;
   table_xo1[25] = InstructionID::oris;
   table_xo1[26] = InstructionID::xori;
   table_xo1[27] = InstructionID::xoris;
   table_xo1[28] = InstructionID::andio;
   table_xo1[29] = InstructionID::andiso;
   table_xo1[30] = decode_xo1_30;
   table_xo1[31] = decode_xo1_31;
   table_xo1[32] = InstructionID::lwz;
   table_xo1[33] = InstructionID::lwzu;
   table_xo1[34] = InstructionID::lbz;
   table_xo1[35] = InstructionID::lbzu;
   table_xo1[36] = InstructionID::stw;
   table_xo1[37] = InstructionID::stwu;
   table_xo1[38] = InstructionID::stb;
   table_xo1[39] = InstructionID::stbu;
   table_xo1[40] = InstructionID::lhz;
   table_xo1[41] = InstructionID::lhzu;
   table_xo1[42] = InstructionID::lha;
   table_xo1[43] = InstructionID::lhau;
   table_xo1[44] = InstructionID::sth;
   table_xo1[45] = InstructionID::sthu;
   table_xo1[46] = InstructionID::lmw;
   table_xo1[47] = InstructionID::stmw;
   table_xo1[48] = InstructionID::lfs;
   table_xo1[49] = InstructionID::lfsu;
   table_xo1[50] = InstructionID::lfd;
   table_xo1[51] = InstructionID::lfdu;
   table_xo1[52] = InstructionID::stfs;
   table_xo1[53] = InstructionID::stfsu;
   table_xo1[54] = InstructionID::stfd;
   table_xo1[55] = InstructionID::stfdu;
   table_xo1[58] = decode_xo1_58;
   table_xo1[59] = decode_xo1_59;
   table_xo1[62] = decode_xo1_62;
   table_xo1[63] = decode_xo1_63;
   table_xo1_19_xo2[0] = InstructionID::mcrf;
   table_xo1_19_xo2[16] = InstructionID::bclr;
   table_xo1_19_xo2[18] = InstructionID::rfid;
   table_xo1_19_xo2[33] = InstructionID::crnor;
   table_xo1_19_xo2[129] = InstructionID::crandc;
   table_xo1_19_xo2[150] = InstructionID::isync;
   table_xo1_19_xo2[193] = InstructionID::crxor;
   table_xo1_19_xo2[225] = InstructionID::crnand;
   table_xo1_19_xo2[257] = InstructionID::crand;
   table_xo1_19_xo2[289] = InstructionID::creqv;
   table_xo1_19_xo2[417] = InstructionID::crorc;
   table_xo1_19_xo2[449] = InstructionID::cror;
   table_xo1_19_xo2[528] = InstructionID::bcctr;
   table_xo1_30_xo7[8] = InstructionID::rldcl;
   table_xo1_30_xo7[9] = InstructionID::rldcr;
   table_xo1_30_xo8[0] = InstructionID::rldicl;
   table_xo1_30_xo8[1] = InstructionID::rldicr;
   table_xo1_30_xo8[2] = InstructionID::rldic;
   table_xo1_30_xo8[3] = InstructionID::rldimi;
   table_xo1_31_xo2[0] = InstructionID::cmp;
   table_xo1_31_xo2[4] = InstructionID::tw;
   table_xo1_31_xo2[19] = decode_xo1_31_xo2_19;
   table_xo1_31_xo2[20] = InstructionID::lwarx;
   table_xo1_31_xo2[21] = InstructionID::ldx;
   table_xo1_31_xo2[23] = InstructionID::lwzx;
   table_xo1_31_xo2[24] = InstructionID::slw;
   table_xo1_31_xo2[26] = InstructionID::cntlzw;
   table_xo1_31_xo2[27] = InstructionID::sld;
   table_xo1_31_xo2[28] = InstructionID::and;
   table_xo1_31_xo2[31] = InstructionID::td;
   table_xo1_31_xo2[32] = InstructionID::cmpl;
   table_xo1_31_xo2[53] = InstructionID::ldux;
   table_xo1_31_xo2[54] = InstructionID::dcbst;
   table_xo1_31_xo2[55] = InstructionID::lwzux;
   table_xo1_31_xo2[58] = InstructionID::cntlzd;
   table_xo1_31_xo2[60] = InstructionID::andc;
   table_xo1_31_xo2[83] = InstructionID::mfmsr;
   table_xo1_31_xo2[84] = InstructionID::ldarx;
   table_xo1_31_xo2[86] = InstructionID::dcbf;
   table_xo1_31_xo2[87] = InstructionID::lbzx;
   table_xo1_31_xo2[119] = InstructionID::lbzux;
   table_xo1_31_xo2[124] = InstructionID::nor;
   table_xo1_31_xo2[144] = decode_xo1_31_xo2_144;
   table_xo1_31_xo2[146] = InstructionID::mtmsr;
   table_xo1_31_xo2[149] = InstructionID::stdx;
   table_xo1_31_xo2[150] = InstructionID::stwcx;
   table_xo1_31_xo2[151] = InstructionID::stwx;
   table_xo1_31_xo2[178] = InstructionID::mtmsrd;
   table_xo1_31_xo2[181] = InstructionID::stdux;
   table_xo1_31_xo2[183] = InstructionID::stwux;
   table_xo1_31_xo2[210] = InstructionID::mtsr;
   table_xo1_31_xo2[214] = InstructionID::stdcx;
   table_xo1_31_xo2[242] = InstructionID::mtsrin;
   table_xo1_31_xo2[246] = InstructionID::dcbtst;
   table_xo1_31_xo2[274] = InstructionID::tlbiel;
   table_xo1_31_xo2[278] = InstructionID::dcbt;
   table_xo1_31_xo2[279] = InstructionID::lhzx;
   table_xo1_31_xo2[306] = InstructionID::tlbie;
   table_xo1_31_xo2[310] = InstructionID::eciwx;
   table_xo1_31_xo2[311] = InstructionID::lhzux;
   table_xo1_31_xo2[316] = InstructionID::xor;
   table_xo1_31_xo2[339] = InstructionID::mfspr;
   table_xo1_31_xo2[341] = InstructionID::lwax;
   table_xo1_31_xo2[343] = InstructionID::lhax;
   table_xo1_31_xo2[370] = InstructionID::tlbia;
   table_xo1_31_xo2[371] = InstructionID::mftb;
   table_xo1_31_xo2[373] = InstructionID::lwaux;
   table_xo1_31_xo2[375] = InstructionID::lhaux;
   table_xo1_31_xo2[402] = InstructionID::slbmte;
   table_xo1_31_xo2[407] = InstructionID::sthx;
   table_xo1_31_xo2[412] = InstructionID::orc;
   table_xo1_31_xo2[434] = InstructionID::slbie;
   table_xo1_31_xo2[438] = InstructionID::ecowx;
   table_xo1_31_xo2[439] = InstructionID::sthux;
   table_xo1_31_xo2[444] = InstructionID::or;
   table_xo1_31_xo2[467] = InstructionID::mtspr;
   table_xo1_31_xo2[476] = InstructionID::nand;
   table_xo1_31_xo2[498] = InstructionID::slbia;
   table_xo1_31_xo2[533] = InstructionID::lswx;
   table_xo1_31_xo2[534] = InstructionID::lwbrx;
   table_xo1_31_xo2[535] = InstructionID::lfsx;
   table_xo1_31_xo2[536] = InstructionID::srw;
   table_xo1_31_xo2[539] = InstructionID::srd;
   table_xo1_31_xo2[566] = InstructionID::tlbsync;
   table_xo1_31_xo2[567] = InstructionID::lfsux;
   table_xo1_31_xo2[595] = InstructionID::mfsr;
   table_xo1_31_xo2[597] = InstructionID::lswi;
   table_xo1_31_xo2[598] = InstructionID::sync;
   table_xo1_31_xo2[599] = InstructionID::lfdx;
   table_xo1_31_xo2[631] = InstructionID::lfdux;
   table_xo1_31_xo2[659] = InstructionID::mfsrin;
   table_xo1_31_xo2[661] = InstructionID::stswx;
   table_xo1_31_xo2[662] = InstructionID::stwbrx;
   table_xo1_31_xo2[663] = InstructionID::stfsx;
   table_xo1_31_xo2[725] = InstructionID::stswi;
   table_xo1_31_xo2[727] = InstructionID::stfdx;
   table_xo1_31_xo2[759] = InstructionID::stfdux;
   table_xo1_31_xo2[790] = InstructionID::lhbrx;
   table_xo1_31_xo2[792] = InstructionID::sraw;
   table_xo1_31_xo2[794] = InstructionID::srad;
   table_xo1_31_xo2[795] = InstructionID::stfsux;
   table_xo1_31_xo2[824] = InstructionID::srawi;
   table_xo1_31_xo2[827] = InstructionID::sradi;
   table_xo1_31_xo2[851] = InstructionID::slbmfev;
   table_xo1_31_xo2[854] = InstructionID::eieio;
   table_xo1_31_xo2[915] = InstructionID::slbmfee;
   table_xo1_31_xo2[918] = InstructionID::sthbrx;
   table_xo1_31_xo2[922] = InstructionID::extsh;
   table_xo1_31_xo2[954] = InstructionID::extsb;
   table_xo1_31_xo2[982] = InstructionID::icbi;
   table_xo1_31_xo2[983] = InstructionID::stfiwx;
   table_xo1_31_xo2[986] = InstructionID::extsw;
   table_xo1_31_xo2[1014] = InstructionID::dcbz;
   table_xo1_31_xo3[8] = InstructionID::subfc;
   table_xo1_31_xo3[9] = InstructionID::mulhdu;
   table_xo1_31_xo3[10] = InstructionID::addc;
   table_xo1_31_xo3[11] = InstructionID::mulhwu;
   table_xo1_31_xo3[31] = InstructionID::neg;
   table_xo1_31_xo3[40] = InstructionID::subf;
   table_xo1_31_xo3[73] = InstructionID::mulhd;
   table_xo1_31_xo3[75] = InstructionID::mulhw;
   table_xo1_31_xo3[136] = InstructionID::subfe;
   table_xo1_31_xo3[138] = InstructionID::adde;
   table_xo1_31_xo3[200] = InstructionID::subfze;
   table_xo1_31_xo3[202] = InstructionID::addze;
   table_xo1_31_xo3[215] = InstructionID::stbx;
   table_xo1_31_xo3[232] = InstructionID::subfme;
   table_xo1_31_xo3[233] = InstructionID::mulld;
   table_xo1_31_xo3[234] = InstructionID::addme;
   table_xo1_31_xo3[235] = InstructionID::mullw;
   table_xo1_31_xo3[247] = InstructionID::stbux;
   table_xo1_31_xo3[266] = InstructionID::add;
   table_xo1_31_xo3[284] = InstructionID::eqv;
   table_xo1_31_xo3[457] = InstructionID::divdu;
   table_xo1_31_xo3[459] = InstructionID::divwu;
   table_xo1_31_xo3[489] = InstructionID::divd;
   table_xo1_31_xo3[491] = InstructionID::divw;
   table_xo1_31_xo2_19_xo6[0] = InstructionID::mfcr;
   table_xo1_31_xo2_19_xo6[1] = InstructionID::mfocrf;
   table_xo1_31_xo2_144_xo6[0] = InstructionID::mtcrf;
   table_xo1_31_xo2_144_xo6[1] = InstructionID::mtocrf;
   table_xo1_58_xo5[0] = InstructionID::ld;
   table_xo1_58_xo5[1] = InstructionID::ldu;
   table_xo1_58_xo5[2] = InstructionID::lwa;
   table_xo1_59_xo2[18] = InstructionID::fdivs;
   table_xo1_59_xo2[20] = InstructionID::fsubs;
   table_xo1_59_xo2[21] = InstructionID::fadds;
   table_xo1_59_xo2[22] = InstructionID::fsqrts;
   table_xo1_59_xo4[24] = InstructionID::fres;
   table_xo1_59_xo4[25] = InstructionID::fmuls;
   table_xo1_59_xo4[28] = InstructionID::fmsubs;
   table_xo1_59_xo4[29] = InstructionID::fmadds;
   table_xo1_59_xo4[30] = InstructionID::fnmsubs;
   table_xo1_59_xo4[31] = InstructionID::fnmadds;
   table_xo1_62_xo5[0] = InstructionID::std;
   table_xo1_62_xo5[1] = InstructionID::stdu;
   table_xo1_63_xo2[0] = InstructionID::fcmpu;
   table_xo1_63_xo2[12] = InstructionID::frsp;
   table_xo1_63_xo2[14] = InstructionID::fctiw;
   table_xo1_63_xo2[15] = InstructionID::fctiwz;
   table_xo1_63_xo2[18] = InstructionID::fdiv;
   table_xo1_63_xo2[20] = InstructionID::fsub;
   table_xo1_63_xo2[21] = InstructionID::fadd;
   table_xo1_63_xo2[22] = InstructionID::fsqrt;
   table_xo1_63_xo2[32] = InstructionID::fcmpo;
   table_xo1_63_xo2[38] = InstructionID::mtfsb1;
   table_xo1_63_xo2[40] = InstructionID::fneg;
   table_xo1_63_xo2[64] = InstructionID::mcrfs;
   table_xo1_63_xo2[70] = InstructionID::mtfsb0;
   table_xo1_63_xo2[72] = InstructionID::fmr;
   table_xo1_63_xo2[134] = InstructionID::mtfsfi;
   table_xo1_63_xo2[136] = InstructionID::fnabs;
   table_xo1_63_xo2[264] = InstructionID::fabs;
   table_xo1_63_xo2[583] = InstructionID::mffs;
   table_xo1_63_xo2[711] = InstructionID::mtfsf;
   table_xo1_63_xo2[814] = InstructionID::fctid;
   table_xo1_63_xo2[815] = InstructionID::fctidz;
   table_xo1_63_xo2[846] = InstructionID::fcfid;
   table_xo1_63_xo4[23] = InstructionID::fsel;
   table_xo1_63_xo4[25] = InstructionID::fmul;
   table_xo1_63_xo4[26] = InstructionID::frsqrte;
   table_xo1_63_xo4[28] = InstructionID::fmsub;
   table_xo1_63_xo4[29] = InstructionID::fmadd;
   table_xo1_63_xo4[30] = InstructionID::fnmsub;
   table_xo1_63_xo4[31] = InstructionID::fnmadd;
   return true;
}
