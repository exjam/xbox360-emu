#ifndef PPC_CPU_H
#define PPC_CPU_H

#include <stdint.h>

namespace ppc
{

/* Page 50 */
struct Fpscr
{
   uint32_t rn : 2;        /* 00 nearest, 01 zero, 10 +inf, 11 -inf */
   uint32_t ni : 1;        /* Non-IEEE mode */
   uint32_t xe : 1;        /* Enable FP Exception: Inexact */
   uint32_t ze : 1;        /* Enable FP Exception: Zero Divide */
   uint32_t ue : 1;        /* Enable FP Exception: Underflow */
   uint32_t oe : 1;        /* Enable FP Exception: Overflow */
   uint32_t ve : 1;        /* Enable FP Exception: Invalid Operation */
   uint32_t vxcvi : 1;     /* Enable FP Exception: Invalid Operation for Integer Convert */
   uint32_t vxsqrt : 1;    /* Enable FP Exception: Invalid Operation for Square Root */
   uint32_t vxsoft : 1;    /* Enable FP Exception: Invalid Operation for Software Request */
   uint32_t _reserved : 1;
   uint32_t fprfUO : 1;    /* FP Result: Unordered or NaN */
   uint32_t fprfEQ : 1;    /* FP Result: Equal or Zero */
   uint32_t fprfGT : 1;    /* FP Result: Greater Than or Positive */
   uint32_t fprfLT : 1;    /* FP Result: Less Than or Negative */
   uint32_t fprfC : 1;     /* FP Result: Class Descriptor*/
   uint32_t fi : 1;        /* FP State: Inexact Fraction */
   uint32_t fr : 1;        /* FP State: Fraction Rounded */
   uint32_t vxvc : 1;      /* FP Exception: Invalid Operation for Compare */
   uint32_t vximz : 1;     /* FP Exception: Invalid Operation for [Inf * Zero] */
   uint32_t vxzdz : 1;     /* FP Exception: Invalid Operation for [Zero / Zero] */
   uint32_t vxidi : 1;     /* FP Exception: Invalid Operation for [Inf / Inf] */
   uint32_t vxisi : 1;     /* FP Exception: Invalid Operation for [Inf - Inf] */
   uint32_t vxsnan : 1;    /* FP Exception: Invalid Operation for SNaN */
   uint32_t xx : 1;        /* FP Exception: Inexact */
   uint32_t zx : 1;        /* FP Exception: Zero Divide */
   uint32_t ux : 1;        /* FP Exception: Underflow */
   uint32_t ox : 1;        /* FP Exception: Overflow */
   uint32_t vx : 1;        /* FP Exception: Invalid Operation */
   uint32_t fex : 1;       /* FP Exception: Any enabled FP Exception has happened */
   uint32_t fx : 1;        /* FP Exception: Any FP Exception has happened */
};

union Cr {
   uint32_t value;

   struct {
      uint32_t cr7 : 4;
      uint32_t cr6 : 4;
      uint32_t cr5 : 4;
      uint32_t cr4 : 4;
      uint32_t cr3 : 4;
      uint32_t cr2 : 4;
      uint32_t cr1 : 4;
      uint32_t cr0 : 4;
   };

   struct {
      uint32_t : 28;
      uint32_t summaryOverflow;
      uint32_t zero;
      uint32_t positive;
      uint32_t negative;
   };

   struct {
      uint32_t : 28;
      uint32_t so;
      uint32_t eq;
      uint32_t gt;
      uint32_t lt;
   };
};

union Xer {
   uint64_t value;

   struct {
      uint64_t byteCount : 7;
      uint64_t : 22;
      uint64_t ca : 1;
      uint64_t ov : 1;
      uint64_t so : 1; /* Set when OV is set (not including mtspr), remains until cleared by mtspr or mcrxr */
      uint64_t : 32;
   };
};

struct Registers
{
   /* User Model UISA */
   uint64_t gpr[32]; /* 32 General Purpose Registers */
   double fpr[32];   /* 32 Floating Point Registers */
   Cr cr;            /* Condition Register 8x 4bit cr0-cr7 */
   uint32_t fpscr;   /* Floating Point Status & Control Register */
   Xer xer;          /* Fixed-Point Exception Register */
   uint64_t lr;      /* Link Register */
   uint64_t ctr;     /* Count Register */

   /* User Model VEA */
   uint32_t tbu;     /* Time Base Upper */
   uint32_t tbl;     /* Time Base Lower */

   /* Configuration Registers */
   uint64_t msr;     /* Machine State Register */
   uint32_t pvr;     /* Processor Version Register */

   /* Memory Management Registers */
   uint64_t sdr1;
   uint64_t asr;     /* Address Space Register */

   /* Exception Handling Registers */
   uint64_t dar;     /* Data Address Register */
   uint32_t dsisr;
   uint64_t srr0;    /* Save and Restore Registers */
   uint64_t srr1;
   uint64_t sprg0;   /* Software use SPRs */
   uint64_t sprg1;
   uint64_t sprg2;
   uint64_t sprg3;
   uint64_t fpecr;   /* Floating-Point Exception Cause Register */

   /* Misc. Registers */
   uint32_t dec;     /* Decrementer Register */
   uint64_t dabr;    /* Data Address Breakpoint Register */
   uint64_t pir;     /* Processor Identification Register */
   uint32_t ear;     /* External Access Register */
   uint64_t ctrl;    /* Control Register */
   uint64_t iabr;    /* Instruction Address Breakpoint Register */

   /* ID mappings for spr */
   enum spr {   
      XER      = 1,
      LR       = 8,
      CTR      = 9,

      DSISR    = 18,
      DAR      = 19,

      DEC      = 22,
      SDR1     = 25,
      SRR0     = 26,
      SRR1     = 27,

      SPRG0    = 272,
      SPRG1    = 273,
      SPRG2    = 274,
      SPRG3    = 275,

      ASR      = 280,
      EAR      = 282,
      TBU      = 284,
      TBL      = 285,

      DABR     = 1013,

      /* Not used in mtspr/mfspr */
      FPECR,
      FPSCR,
      CR,
      MSR,
      PVR,
      PIR,
      CTRL,
      IABR
   };

   uint64_t getSpr(uint32_t id)
   {
      switch (id) {
      case Registers::XER:
         return xer.value;
      case Registers::LR:
         return lr;
      case Registers::CTR:
         return ctr;
      case Registers::DSISR:
         return dsisr;
      case Registers::DAR:
         return dar;
      case Registers::DEC:
         return dec;
      case Registers::SDR1:
         return sdr1;
      case Registers::SRR0:
         return srr0;
      case Registers::SRR1:
         return srr1;
      case Registers::SPRG0:
         return sprg0;
      case Registers::SPRG1:
         return sprg1;
      case Registers::SPRG2:
         return sprg2;
      case Registers::SPRG3:
         return sprg3;
      case Registers::ASR:
         return asr;
      case Registers::EAR:
         return ear;
      case Registers::TBU:
         return tbu;
      case Registers::TBL:
         return tbl;
      case Registers::DABR:
         return dabr;
      case Registers::FPECR:
         return fpecr;
      case Registers::FPSCR:
         return fpscr;
      case Registers::CR:
         return cr.value;
      case Registers::MSR:
         return msr;
      case Registers::PVR:
         return pvr;
      case Registers::PIR:
         return pir;
      case Registers::CTRL:
         return ctrl;
      case Registers::IABR:
         return iabr;
      default:
         return 0;
      }
   }
   
   void setSpr(uint32_t id, uint64_t value)
   {
      switch (id) {
      case Registers::XER:
         xer.value = value;
         break;
      case Registers::LR:
         lr = value;
         break;
      case Registers::CTR:
         ctr = value;
         break;
      case Registers::DSISR:
         dsisr = static_cast<uint32_t>(value);
         break;
      case Registers::DAR:
         dar = value;
         break;
      case Registers::DEC:
         dec = static_cast<uint32_t>(value);
         break;
      case Registers::SDR1:
         sdr1 = value;
         break;
      case Registers::SRR0:
         srr0 = value;
         break;
      case Registers::SRR1:
         srr1 = value;
         break;
      case Registers::SPRG0:
         sprg0 = value;
         break;
      case Registers::SPRG1:
         sprg1 = value;
         break;
      case Registers::SPRG2:
         sprg2 = value;
         break;
      case Registers::SPRG3:
         sprg3 = value;
         break;
      case Registers::ASR:
         asr = value;
         break;
      case Registers::EAR:
         ear = static_cast<uint32_t>(value);
         break;
      case Registers::TBU:
         tbu = static_cast<uint32_t>(value);
         break;
      case Registers::TBL:
         tbl = static_cast<uint32_t>(value);
         break;
      case Registers::DABR:
         dabr = value;
         break;
      case Registers::FPECR:
         fpecr = value;
         break;
      case Registers::FPSCR:
         fpscr = static_cast<uint32_t>(value);
         break;
      case Registers::CR:
         cr.value = static_cast<uint32_t>(value);
         break;
      case Registers::MSR:
         msr = value;
         break;
      case Registers::PVR:
         pvr = static_cast<uint32_t>(value);
         break;
      case Registers::PIR:
         pir = value;
         break;
      case Registers::CTRL:
         ctrl = value;
         break;
      case Registers::IABR:
         iabr = value;
         break;
      }
   }
};
   

/*
Instruction Groups:

Integer
Floating-Point
Load/Store
Synchronization
Flow-Control
Processor Control (mtspr, mfspr)
Memory/Cache Control
External Control (eciwx, ecowx)
*/
struct Instruction
{
   union {
      uint32_t value;

      struct {
         uint32_t rc : 1;     /* Record Bit */
         uint32_t subop10 : 10;
         uint32_t rB : 5;
         uint32_t rA : 5;
         uint32_t rD : 5;     /* GPRs*/
         uint32_t opcode : 6;
      };

      struct {
         uint32_t : 1; 
         uint32_t subop9_oe : 9;
         uint32_t oe : 1;     /* Setting OV/SO in XER */
         uint32_t : 21;
      };

      struct {
         uint32_t : 11;
         uint32_t spr : 10;
         uint32_t : 11;
      };

      struct {
         uint32_t lk : 1;
         uint32_t aa : 1;
         uint32_t li : 24;
         uint32_t : 5;
      };

      struct {
         uint32_t : 2;
         uint32_t ds : 14;
         uint32_t : 5;
         uint32_t rS : 5;
         uint32_t : 6;
      };

      struct {
         uint32_t d : 16;
         uint32_t : 16;
      };

      struct {
         uint32_t uimm : 16;
         uint32_t : 16;
      };

      struct {
         uint32_t simm : 16;
         uint32_t : 16;
      };

      struct {
         uint32_t : 11;
         uint32_t bh : 2;
         uint32_t : 3;
         uint32_t bi : 5;
         uint32_t bo : 5;
         uint32_t : 6;
      };

      struct {
         uint32_t : 2;
         uint32_t bd : 14;
         uint32_t : 16;
      };

      struct {
         uint32_t : 21;
         uint32_t l : 1;
         uint32_t : 1;
         uint32_t crfd : 3;
         uint32_t : 6;
      };
   };
};

} // namespace ppc

#endif // PPC_CPU_H
