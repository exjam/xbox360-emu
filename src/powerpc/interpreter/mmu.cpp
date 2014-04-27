#include "interpreter.h"
#include "regs.h"

#include "powerpc/instructions.h"

#include "common/log.h"
#include "common/memory.h"

#include <limits>

namespace ppc
{

namespace Interpreter
{

/* Data Cache Block Flush */
bool dcbf(State *state, Instruction instr)
{
   auto ea = gpr0(instr.rA) + gpr(instr.rB);
   /* TODO: dcbf Data Cache Block Flush */
   return false;
}

/* Data Cache Block Store */
bool dcbst(State *state, Instruction instr)
{
   auto ea = gpr0(instr.rA) + gpr(instr.rB);
   /* TODO: dcbst Data Cache Block Store */
   return false;
}

/* Data Cache Block Touch */
bool dcbt(State *state, Instruction instr)
{
   auto ea = gpr0(instr.rA) + gpr(instr.rB);
   /* TODO: dcbt Data Cache Block Touch */
   return false;
}

/* Data Cache Block Touch for Store */
bool dcbtst(State *state, Instruction instr)
{
   auto ea = gpr0(instr.rA) + gpr(instr.rB);
   /* TODO: dcbtst Data Cache Block Touch for Store */
   return false;
}

/* Data Cache Block Zero */
bool dcbz(State *state, Instruction instr)
{
   auto ea = gpr0(instr.rA) + gpr(instr.rB);
   auto n = 32; /* TODO: dcbz block size */
   Memory::zero(ea & ~(n -1), n);
   return false;
}

/* External Control In Word Indexed */
bool eciwx(State *state, Instruction instr)
{
   auto ea = gpr0(instr.rA) + gpr(instr.rB);

   /* TODO: eciwx Load from physical address ea of device EAR[RID] */

   if ((state->reg.ear & 0x80000000) == 0) {
      raise(state, ppc::Exceptions::DSI);
   }

   if (ea & 0x3) {
      raise(state, ppc::Exceptions::Alignment);
   }

   return false;
}

/* External Control Out Word Indexed */
bool ecowx(State *state, Instruction instr)
{
   auto ea = gpr0(instr.rA) + gpr(instr.rB);

   /* TODO: ecowx Store to physical address ea of device EAR[RID] */

   if ((state->reg.ear & 0x80000000) == 0) {
      raise(state, ppc::Exceptions::DSI);
   }

   if (ea & 0x3) {
      raise(state, ppc::Exceptions::Alignment);
   }

   return true;
}

/* Enforice In-Order Execution of I/O */
bool eieio(State *state, Instruction instr)
{
   _mm_mfence();
   return true;
}

/* Instruction Cache Block Invalidate */
bool icbi(State *state, Instruction instr)
{
   /* Invalidates instruction cache
    * not needed for interpreter, only for JIT
    */
   auto ea = gpr0(instr.rA) + gpr(instr.rB);
   return true;
}

/* Instruction Synchronize */
bool isync(State *state, Instruction instr)
{
   /* Should be an instruction execution fence, not memory fence */
   _mm_mfence();
   return true;
}

/* Segment Register bitfield */
union RegSr
{
   ppc::reg_t value;

   struct
   {
      ppc::reg_t : 33;
      ppc::reg_t Ks : 1;
      ppc::reg_t Kp : 1;
      ppc::reg_t N : 1;
      ppc::reg_t L : 1;
      ppc::reg_t vsid_up : 27;
   };
};

/* Move from Segment Register x */
bool mfsrx(State *state, Instruction instr, int index)
{
   if (index > state->reg.slb.size()) {
      raise(state, ppc::Exceptions::ProgramIllegalInstruction);
      return true;
   }

   RegSr sr = { 0 };
   auto &entry = state->reg.slb[index];

   sr.vsid_up = static_cast<uint64_t>(entry.vsid) >> 25;
   sr.Ks = entry.Ks;
   sr.Kp = entry.Kp;
   sr.N = entry.N;
   sr.L = entry.L;
   gpr(instr.rD) = sr.value;

   return true;
}

/* Move from Segment Register */
bool mfsr(State *state, Instruction instr)
{
   return mfsrx(state, instr, instr.sr);
}

/* Move from Segment Register Indirect */
bool mfsrin(State *state, Instruction instr)
{
   return mfsrx(state, instr, (gpr(instr.rB) >> 32) & 0xf);
}

/* Move to Segment Register x */
bool mtsrx(State *state, Instruction instr, int index)
{
   if (index > state->reg.slb.size()) {
      raise(state, ppc::Exceptions::ProgramIllegalInstruction);
      return true;
   }

   RegSr sr = { gpr(instr.rS) };
   auto &entry = state->reg.slb[index];

   entry.valid = 1;
   entry.esid = static_cast<uint64_t>(index) << 32;
   entry.vsid = static_cast<uint64_t>(sr.vsid_up) << 25;
   entry.Ks = sr.Ks;
   entry.Kp = sr.Kp;
   entry.N = sr.N;
   entry.L = sr.L;
   entry.C = 0;

   return true;
}

/* Move to Segment Register */
bool mtsr(State *state, Instruction instr)
{
   return mtsrx(state, instr, instr.sr);
}

/* Move to Segment Register Indirect */
bool mtsrin(State *state, Instruction instr)
{
   return mtsrx(state, instr, (gpr(instr.rB) >> 32) & 0xf);
}

/* SLB VSID Register bitfield */
union RegVsid
{
   ppc::reg_t value;

   struct
   {
      ppc::reg_t vsid : 52;
      ppc::reg_t Ks : 1;
      ppc::reg_t Kp : 1;
      ppc::reg_t N : 1;
      ppc::reg_t L : 1;
      ppc::reg_t C : 1;
      ppc::reg_t : 7;
   };
};

/* SLB ESID Register bitfield */
union RegEsid
{
   ppc::reg_t value;

   struct
   {
      ppc::reg_t esid : 36;
      ppc::reg_t valid : 1;
      ppc::reg_t : 15;
      ppc::reg_t index : 12;
   };
};

/* SLB Invalidate All */
bool slbia(State *state, Instruction instr)
{
   for (auto &entry : state->reg.slb) {
      entry.valid = 0;
   }

   return true;
}

/* SLB Invalidate Entry */
bool slbie(State *state, Instruction instr)
{
   auto esid = (gpr(instr.rA) >> 28) & little_endian::make_bit_mask<ppc::reg_t, 36>();
   auto cls = (gpr(instr.rA) >> 27) & 1;

   for (auto &entry : state->reg.slb) {
      if (entry.valid && entry.esid == esid && entry.C == cls) {
         entry.valid = 0;
      }
   }

   return true;
}

/* SLB Move From Entry ESID */
bool slbmfee(State *state, Instruction instr)
{
   auto idx = gpr(instr.rB) & little_endian::make_bit_mask<ppc::reg_t, 12>();

   if (idx > state->reg.slb.size()) {
      raise(state, ppc::Exceptions::ProgramIllegalInstruction);
      return true;
   }

   auto &entry = state->reg.slb[idx];
   RegEsid d = { 0 };

   d.valid = entry.valid;
   d.esid = entry.esid;

   gpr(instr.rD) = d.value;

   return true;
}

/* SLB Move From Entry VSID */
bool slbmfev(State *state, Instruction instr)
{
   auto idx = gpr(instr.rB) & little_endian::make_bit_mask<ppc::reg_t, 12>();

   if (idx > state->reg.slb.size()) {
      raise(state, ppc::Exceptions::ProgramIllegalInstruction);
      return true;
   }

   auto &entry = state->reg.slb[idx];
   RegVsid d = { 0 };

   d.vsid = entry.vsid;
   d.Ks = entry.Ks;
   d.Kp = entry.Kp;
   d.N = entry.N;
   d.L = entry.L;
   d.C = entry.C;

   gpr(instr.rD) = d.value;

   return true;
}

/* SLB Move to Entry */
bool slbmte(State *state, Instruction instr)
{
   RegEsid b = { gpr(instr.rB) };
   RegVsid d = { gpr(instr.rD) };

   if (b.index > state->reg.slb.size()) {
      raise(state, ppc::Exceptions::ProgramIllegalInstruction);
      return true;
   }

   auto &entry = state->reg.slb[b.index];

   entry.valid = b.valid;
   entry.esid = b.esid;
   entry.vsid = d.vsid;
   entry.Ks = d.Ks;
   entry.Kp = d.Kp;
   entry.N = d.N;
   entry.L = d.L;
   entry.C = d.C;

   gpr(instr.rD) = d.value;

   return true;
}

/* Synchronize */
bool sync(State *state, Instruction instr)
{
   switch (instr.syncl) {
   case 0: /* Heavyweight Sync */
      break;
   case 1: /* Lightweight Sync */
      break;
   case 2: /* ptesync */
      break;
   case 3: /* reserved */
      break;
   }

   _mm_mfence();
   return true;
}

UNIMPLEMENTED(tlbia);    /* Translation Lookaside Buffer Invalidate All */
UNIMPLEMENTED(tlbie);    /* Translation Lookaside Buffer Invalidate Entry */
UNIMPLEMENTED(tlbiel);   /* Translation Lookaside Buffer Invalidate Entry Local */
UNIMPLEMENTED(tlbsync);  /* Translation Lookaside Buffer Synchronize */

} // namespace Interpreter

} // namespace ppc
