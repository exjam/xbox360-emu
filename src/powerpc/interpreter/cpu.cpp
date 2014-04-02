#include "interpreter.h"
#include "regs.h"

#include "common/bits.h"

namespace ppc 
{

namespace Interpreter
{

bool handleExceptions(State *state)
{
   auto es1 = bits::makeRange<33, 36>(state->reg.srr1);
   auto es2 = bits::makeRange<42, 47>(state->reg.srr1);

   bits::copy<0>(state->reg.srr1, state->reg.msr.value);
   bits::copy<48, 55>(state->reg.srr1, state->reg.msr.value);
   bits::copy<57, 59>(state->reg.srr1, state->reg.msr.value);
   bits::copy<62, 63>(state->reg.srr1, state->reg.msr.value);

   state->reg.msr.sf = 1;
   state->reg.msr.pow = 0;
   state->reg.msr.ee = 0;
   state->reg.msr.pr = 0;
   state->reg.msr.fp = 0;
   state->reg.msr.fe0 = 0;
   state->reg.msr.se = 0;
   state->reg.msr.be = 0;
   state->reg.msr.fe1 = 0;
   state->reg.msr.pmm = 0;
   state->reg.msr.ir = 0;
   state->reg.msr.dr = 0;
   state->reg.msr.ri = 0;
   state->reg.msr.le = state->reg.msr.ile;

   state->reg.srr0 = state->nia;

   /* Nonmaskable, asynchronous */
   if (state->reg.exception & ppc::Exceptions::SystemReset) {
      es1 = 0;
      es2 = 0;
      state->nia = 0x100;
   } else if (state->reg.exception & ppc::Exceptions::MachineCheck) {
      if (state->reg.msr.me == 1) {
         state->reg.msr.me = 0;
         state->nia = 0x200;
      } else {
         /* Full hardware break / dump */
      }
   } else

   /* Synchronous, precise */
   if (state->reg.exception & ppc::Exceptions::SystemCall) {
      state->nia = 0xC00;
   } else if (state->reg.exception & ppc::Exceptions::FpuUnavailable) {
      state->nia = 0x800;
   } else if (state->reg.exception & ppc::Exceptions::Program) {
      /* TODO: Program set bits 43, 44, 45, 46, 47 */
      state->nia = 0x700;
   } else if (state->reg.exception & ppc::Exceptions::DSI) {
      /* TODO: DSI set dsisr, dar */
      state->nia = 0x300;
   } else if (state->reg.exception & ppc::Exceptions::DataSegment) {
      /* TODO: DataSegment set dar */
      state->nia = 0x380;
   } else if (state->reg.exception & ppc::Exceptions::Alignment) {
      /* TODO: Alignment set dsisr, dar */
      state->nia = 0x600;
   } else if (state->reg.exception & ppc::Exceptions::Trace) {
      state->nia = 0xd00;
   } else if (state->reg.exception & ppc::Exceptions::ISI) {
      /* TODO: ISI set bits 33, 35, 36 */
      state->nia = 0x400;
   } else if (state->reg.exception & ppc::Exceptions::InstructionSegment) {
      state->nia = 0x480;
   } else

   /* Imprecise */
   if (false) {
      /* TODO: Imprecise exception */
   } else

   /* Maskable, asynchronous */
   if (state->reg.exception & ppc::Exceptions::ExternalInterrupt) {
      state->nia = 0x500;
   } else if (state->reg.exception & ppc::Exceptions::Decrementer) {
      state->nia = 0x900;
   } else

   /* Unknown Priority */
   if (state->reg.exception & ppc::Exceptions::PerformanceMonitor) {
      state->nia = 0xf00;
   }
   
   return true;
}

bool raise(State *state, ppc::Exceptions exception)
{
   state->reg.exception |= exception;
   return true;
}

/* Move Condition Register Field */
bool mcrf(State *state, Instruction instr)
{
   crf(instr.crfD) = crf(instr.crfS);
   return true;
}

/* Move from Condition Register */
bool mfcr(State *state, Instruction instr)
{
   gpr(instr.rD) = state->reg.cr.value;
   return true;
}

/* Move from One Condition Register Field */
bool mfocrf(State *state, Instruction instr)
{
   auto crN = -1;

   for (int i = 0; i < 8 && crN == 0; ++i) {
      if (instr.crm & (1 << i)) {
         crN = i;
      }
   }

   if (crN > 0 && crN < 8) {
      auto mask = 0xf << (crN * 4);
      gpr(instr.rD) &= ~mask;
      gpr(instr.rD) |= state->reg.cr.value & mask;
   }

   return true;
}

/* Move from Machine State Register */
bool mfmsr(State *state, Instruction instr)
{
   gpr(instr.rD) = state->reg.msr.value;
   return true;
}

/* Move from Special-Purpose Register */
bool mfspr(State *state, Instruction instr)
{
   auto spr = (instr.spr >> 5) & 0x1f | (instr.spr & 0x1f);
   gpr(instr.rD) = state->reg.getSpr(spr);
   return true;
}

/* Move from Time Base */
bool mftb(State *state, Instruction instr)
{
   auto tbr = (instr.tbr >> 5) & 0x1f | (instr.tbr & 0x1f);
   
   switch (tbr) {
   case 268:
      gpr(instr.rD) = state->reg.tbl;
      break;
   case 269:
      gpr(instr.rD) = state->reg.tbu;
      break;
   }

   return true;
}

/* Move to Condition Register Field */
bool mtcrf(State *state, Instruction instr)
{
   uint32_t mask = 0;

   for (int i = 0; i < 8; ++i) {
      if (instr.crm & (1 << i)) {
         mask |= 0xf << i;
      }
   }

   state->reg.cr.value &= ~mask;
   state->reg.cr.value |= gpr(instr.rD) & mask;
   return true;
}

/* Move to Machine State Register */
bool mtmsr(State *state, Instruction instr)
{
   auto s = gpr(instr.rS);

   if (instr.l15 == 0) {
      state->reg.msr.ir = bits::get(s, 58) | bits::get(s, 49);
      state->reg.msr.dr = bits::get(s, 59) | bits::get(s, 49);

      bits::copy<32, 47>(state->reg.msr.value, s);
      bits::copy<49, 50>(state->reg.msr.value, s);
      bits::copy<52, 57>(state->reg.msr.value, s);
      bits::copy<60, 63>(state->reg.msr.value, s);
   } else {
      bits::copy<48, 62>(state->reg.msr.value, s);
   }

   return true;
}

/* Move to Machine State Register Doubleword */
bool mtmsrd(State *state, Instruction instr)
{
   auto s = gpr(instr.rS);

   if (instr.l15 == 0) {
      state->reg.msr.sf = bits::get(s, 0) | bits::get(s, 1);
      state->reg.msr.dr = bits::get(s, 59) | bits::get(s, 49);

      bits::copy<1, 2>(state->reg.msr.value, s);
      bits::copy<4, 47>(state->reg.msr.value, s);
      bits::copy<49, 50>(state->reg.msr.value, s);
      bits::copy<52, 57>(state->reg.msr.value, s);
      bits::copy<60, 63>(state->reg.msr.value, s);
   } else {
      bits::copy<48, 62>(state->reg.msr.value, s);
   }

   return true;
}

/* Move to One Condition Register Field */
bool mtocrf(State *state, Instruction instr)
{
   auto crN = -1;

   for (int i = 0; i < 8 && crN == 0; ++i) {
      if (instr.crm & (1 << i)) {
         crN = i;
      }
   }

   if (crN > 0 && crN < 8) {
      auto mask = 0xf << (crN * 4);
      state->reg.cr.value &= ~mask;
      state->reg.cr.value |= gpr(instr.rD) & mask;
   }

   return true;
}

/* Move to Special-Purpose Register */
bool mtspr(State *state, Instruction instr)
{
   auto spr = (instr.spr >> 5) & 0x1f | (instr.spr & 0x1f);
   state->reg.setSpr(spr, gpr(instr.rS));
   return true;
}

/* Return from Interrupt Doubleword */
bool rfid(State *state, Instruction instr)
{
   state->reg.msr.sf = bits::get(state->reg.srr1, 0)
                     | bits::get(state->reg.srr1, 1);

   state->reg.msr.ir = bits::get(state->reg.srr1, 58)
                     | bits::get(state->reg.srr1, 49);

   state->reg.msr.dr = bits::get(state->reg.srr1, 59)
                     | bits::get(state->reg.srr1, 49);

   bits::copy< 1,  2>(state->reg.msr.value, state->reg.srr1);
   bits::copy< 4, 32>(state->reg.msr.value, state->reg.srr1);
   bits::copy<37, 41>(state->reg.msr.value, state->reg.srr1);
   bits::copy<52, 58>(state->reg.msr.value, state->reg.srr1);
   bits::copy<60, 63>(state->reg.msr.value, state->reg.srr1);
   
   return true;
}

/* System Call */
bool sc(State *state, Instruction instr)
{
   raise(state, ppc::Exceptions::SystemCall);
   return true;
}

/* Trap x */
enum TrapFlags
{
   TrapUnsignedGreater = 1 << 0,
   TrapUnsignedLess    = 1 << 1,
   TrapEqual           = 1 << 2,
   TrapSignedGreater   = 1 << 3,
   TrapSignedLess      = 1 << 4
};

bool tx(State *state, Instruction instr, uint64_t a, uint64_t b)
{
   auto to = instr.to;
   bool e = false;

   e |= (to & TrapSignedLess)
        && (static_cast<int64_t>(a) < static_cast<int64_t>(b));

   e |= (to & TrapSignedGreater)
        && (static_cast<int64_t>(a) > static_cast<int64_t>(b));

   e |= (to & TrapEqual) && (a == b);
   e |= (to & TrapUnsignedLess) && (a < b);
   e |= (to & TrapUnsignedGreater) && (a > b);

   if (e) {
      raise(state, ppc::Exceptions::ProgramTrap);
   }

   return true;
}

/* Trap Doubleword */
bool td(State *state, Instruction instr)
{
   return tx(state,
             instr,
             gpr(instr.rA),
             gpr(instr.rB));
}

/* Trap Doubleword Immediate */
bool tdi(State *state, Instruction instr)
{
   return tx(state,
             instr,
             gpr(instr.rA),
             bits::signExtend<16, uint64_t>(instr.simm));
}

/* Trap Word */
bool tw(State *state, Instruction instr)
{
   return tx(state,
             instr,
             bits::signExtend<32, uint64_t>(gprw(instr.rA)),
             bits::signExtend<32, uint64_t>(gprw(instr.rB)));
}

/* Trap Word Immediate */
bool twi(State *state, Instruction instr)
{
   return tx(state,
             instr,
             bits::signExtend<32, uint64_t>(gprw(instr.rA)),
             bits::signExtend<16, uint64_t>(instr.simm));
}

} // namespace Interpreter

} // namespace ppc
