#ifndef PPC_CPU_H
#define PPC_CPU_H

#include "util/flags.h"

#include <stdint.h>
#include <vector>

/* Registers are in Little Endian, Memory is in Big Endian */

namespace ppc
{

using reg_t  = uint64_t;
using ureg_t = uint64_t;
using sreg_t = int64_t;
using freg_t = double;

enum class Exceptions {
   SystemReset                  = 0x100,
   MachineCheck                 = 0x200,
   DSI                          = 0x300, /* Data Storage Interrupt */
   DataSegment                  = 0x380,
   ISI                          = 0x400, /* Instruction Storage Interrupt */
   InstructionSegment           = 0x480,
   ExternalInterrupt            = 0x500,
   Alignment                    = 0x600,
   Program                      = 0x700,
   ProgramFloatingPointEnabled  = 0x701,
   ProgramIllegalInstruction    = 0x702,
   ProgramPrivilegedInstruction = 0x704,
   ProgramTrap                  = 0x708,
   FpuUnavailable               = 0x800,
   Decrementer                  = 0x900,
   SystemCall                   = 0xC00,
   Trace                        = 0xD00,
   PerformanceMonitor           = 0xF00,
};

/* Floating-point Status and Control Register */
union Fpscr
{
   /* Floating-Point result flags
   C  <  >  =  ?
   1  0  0  0  1   Quiet NaN
   0  1  0  0  1  -Infinity
   0  1  0  0  0  -Normalised Number
   1  1  0  0  0  -Denormalised Number
   1  0  0  1  0  -Zero
   0  0  0  1  0  +Zero
   1  0  1  0  0  +Denormalized Number
   0  0  1  0  0  +Normalized Number
   0  0  1  0  1  +Infinity
   */

   enum FloatingPointResultFlags
   {
      ClassDescriptor   = 1 << 0, // C
      Negative          = 1 << 1, // <
      Positive          = 1 << 2, // >
      Equal             = 1 << 3, // =
      Unordered         = 1 << 4, // ?
   };

   uint32_t value;

   struct
   {
      uint32_t cr1 : 4;
      uint32_t : 28;
   };

   struct
   {
      uint32_t fx : 1;        /* FP Exception: Any FP Exception has happened */
      uint32_t fex : 1;       /* FP Exception: Any enabled FP Exception has happened */
      uint32_t vx : 1;        /* FP Exception: Invalid Operation */
      uint32_t ox : 1;        /* FP Exception: Overflow */
      uint32_t ux : 1;        /* FP Exception: Underflow */
      uint32_t zx : 1;        /* FP Exception: Zero Divide */
      uint32_t xx : 1;        /* FP Exception: Inexact */
      uint32_t vxsnan : 1;    /* FP Exception: Invalid Operation for SNaN */
      uint32_t vxisi : 1;     /* FP Exception: Invalid Operation for [Inf - Inf] */
      uint32_t vxidi : 1;     /* FP Exception: Invalid Operation for [Inf / Inf] */
      uint32_t vxzdz : 1;     /* FP Exception: Invalid Operation for [Zero / Zero] */
      uint32_t vximz : 1;     /* FP Exception: Invalid Operation for [Inf * Zero] */
      uint32_t vxvc : 1;      /* FP Exception: Invalid Operation for Compare */
      uint32_t fr : 1;        /* FP State: Fraction Rounded */
      uint32_t fi : 1;        /* FP State: Inexact Fraction */
      uint32_t fprf : 5;      /* Floating-Point result flags */
      uint32_t _reserved : 1;
      uint32_t vxsoft : 1;    /* Enable FP Exception: Invalid Operation for Software Request */
      uint32_t vxsqrt : 1;    /* Enable FP Exception: Invalid Operation for Square Root */
      uint32_t vxcvi : 1;     /* Enable FP Exception: Invalid Operation for Integer Convert */
      uint32_t ve : 1;        /* Enable FP Exception: Invalid Operation */
      uint32_t oe : 1;        /* Enable FP Exception: Overflow */
      uint32_t ue : 1;        /* Enable FP Exception: Underflow */
      uint32_t ze : 1;        /* Enable FP Exception: Zero Divide */
      uint32_t xe : 1;        /* Enable FP Exception: Inexact */
      uint32_t ni : 1;        /* Non-IEEE mode */
      uint32_t rn : 2;        /* 00 nearest, 01 zero, 10 +inf, 11 -inf */
   };
};

/* Condition Register */
union Cr
{
   enum Cr0Flags
   {
      Negative        = 1 << 0,
      Positive        = 1 << 1,
      Zero            = 1 << 2,
      SummaryOverflow = 1 << 3,
   };

   enum Cr1Flags
   {
      FloatingPointException        = 1 << 0,
      FloatingPointEnabledException = 1 << 1,
      FloatingPointInvalidOperation = 1 << 2,
      FloatingPointOverflow         = 1 << 3,
   };

   enum CrNFlags
   {
      Less      = 1 << 0,
      Greater   = 1 << 1,
      Equal     = 1 << 2,
      Unordered = 1 << 3,
   };

   uint32_t value;

   struct
   {
      uint32_t cr0 : 4;
      uint32_t cr1 : 4;
      uint32_t cr2 : 4;
      uint32_t cr3 : 4;
      uint32_t cr4 : 4;
      uint32_t cr5 : 4;
      uint32_t cr6 : 4;
      uint32_t cr7 : 4;
   };
};

/* XER Register */
union Xer
{
   uint64_t value;

   struct {
      uint64_t : 32;
      uint64_t so : 1;        /* Sticky OV */
      uint64_t ov : 1;        /* Overflow */
      uint64_t ca : 1;        /* Carry */
      uint64_t : 22;
      uint64_t byteCount : 7; /* For lmwx, stmwx */
   };
};

/* Machine State Register */
union Msr
{
   uint64_t value;

   struct
   {
      uint64_t sf : 1;  /* 64 bit mode */
      uint64_t : 44;
      uint64_t pow : 1; /* Power Management Enabled */
      uint64_t : 1;
      uint64_t ile : 1; /* Little Endian Mode, copied to le on interrupt */
      uint64_t ee : 1;  /* External Interrupt Enabled */
      uint64_t pr : 1;  /* Privilege Level */
      uint64_t fp : 1;  /* Floating-point Available */
      uint64_t me : 1;  /* Machine Check Enabled */
      uint64_t fe0 : 1; /* Floating-point Exception Mode 0 */
      uint64_t se : 1;  /* Single-step Trace Enabled */
      uint64_t be : 1;  /* Branch Trace Enable */
      uint64_t fe1 : 1; /* Floating-point Exception Mode 1 */
      uint64_t : 2;
      uint64_t ir : 1;  /* Instruction Address Translation Enabled */
      uint64_t dr : 1;  /* Data Address Translation Enabled */
      uint64_t : 1;
      uint64_t pmm : 1; /* Performance Monitor Mark */
      uint64_t ri : 1;  /* Recoverable Exception */
      uint64_t le : 1;  /* Little Endian Mode */
   };
};

/* Processor Version Register */
union Pvr
{
   uint32_t value;

   struct
   {
      uint32_t version : 16;
      uint32_t revision : 16;
   };
};

/* SDR1 Register */
union SDR1
{
   uint64_t value;

   struct
   {
      uint64_t htaborg : 46;
      uint64_t : 13;
      uint64_t htabsize : 5;
   };
};

/* Segment Lookaside Buffer (7.4.2.1) */
struct SlbEntry
{
   int valid : 1;      /* Entry Valid */
   uint64_t esid : 36; /* Effective Segment ID */
   uint64_t vsid : 52; /* Virtual Segment ID */
   int Ks : 1;         /* Supervisor State Storage Key */
   int Kp : 1;         /* User (Problem) State Storage Key */
   int N : 1;          /* No Execute Segment */
   int L : 1;          /* Large Virtual Pages */
   int C : 1;          /* Class */
};

struct Registers
{
   /* User Model UISA */
   reg_t gpr[32];    /* 32 General Purpose Registers */
   freg_t fpr[32];   /* 32 Floating Point Registers */
   Cr cr;            /* Condition Register 8x 4bit cr0-cr7 */
   Fpscr fpscr;      /* Floating Point Status & Control Register */
   Xer xer;          /* Fixed-Point Exception Register */
   uint64_t lr;      /* Link Register */
   uint64_t ctr;     /* Count Register */

   /* User Model VEA */
   uint32_t tbu;     /* Time Base Upper */
   uint32_t tbl;     /* Time Base Lower */

   /* Configuration Registers */
   Msr msr;          /* Machine State Register */
   uint32_t pvr;     /* Processor Version Register */

   /* Memory Management Registers */
   uint64_t sdr1;
   uint64_t asr;     /* Address Space Register */
   std::vector<SlbEntry> slb; /* Segment Lookaside Buffer */

   /* Exception Handling Registers */
   Flags<Exceptions> exception;
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

   /* Reserve load/store */
   uint32_t reserve;
   uint64_t reserveAddress;
   
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
         return fpscr.value;
      case Registers::CR:
         return cr.value;
      case Registers::MSR:
         return msr.value;
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
         fpscr.value = static_cast<uint32_t>(value);
         break;
      case Registers::CR:
         cr.value = static_cast<uint32_t>(value);
         break;
      case Registers::MSR:
         msr.value = value;
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
#if 0
/* Instructions are loaded from memory and thus are in Big Endian format*/
struct Instruction
{
   Instruction(){}
   Instruction(uint32_t v){ value = v; }
   operator uint32_t() { return value; }

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

      struct
      {
         uint32_t: 11;
         uint32_t tbr : 10;
         uint32_t: 11;
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

      struct
      {
         uint32_t : 12;
         uint32_t imm : 4;
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
         uint32_t: 18;
         uint32_t crfS : 3;
         uint32_t l : 1;
         uint32_t : 1;
         uint32_t crfD : 3;
         uint32_t : 6;
      };

      struct {
         uint32_t : 21;
         uint32_t syncl : 2;
         uint32_t : 9;
      };

      struct {
         uint32_t  : 11;
         uint32_t crbB : 5;
         uint32_t crbA : 5;
         uint32_t crbD : 5;
         uint32_t  : 6;
      };

      struct {
         uint32_t : 21;
         uint32_t th : 2;
         uint32_t : 9;
      };

      struct {
         uint32_t : 1;
         uint32_t subop5 : 5;
         uint32_t frC : 5;
         uint32_t frB : 5;
         uint32_t frA : 5;
         uint32_t frD : 5;
         uint32_t : 6;
      };

      struct {
         uint32_t : 21;
         uint32_t frS : 5;
         uint32_t : 6;
      };

      struct
      {
         uint32_t : 12;
         uint32_t crm : 8;
         uint32_t : 12;
      };

      struct
      {
         uint32_t : 16;
         uint32_t sr : 4;
         uint32_t : 12;
      };

      struct
      {
         uint32_t: 17;
         uint32_t fm : 8;
         uint32_t: 7;
      };

      struct
      {
         uint32_t : 16;
         uint32_t l15 : 1;
         uint32_t : 15;
      };

      struct
      {
         uint32_t : 1;
         uint32_t shd5 : 1;  /* shd[5] */
         uint32_t : 3;
         uint32_t mbd : 6;
         uint32_t shd04 : 5; /* shd[0-4] */
         uint32_t : 16;
      };

      struct
      {
         uint32_t : 5;
         uint32_t med : 6;
         uint32_t : 21;
      };

      struct
      {
         uint32_t: 1;
         uint32_t me : 5;
         uint32_t mb : 5;
         uint32_t sh : 5;
         uint32_t: 16;
      };

      struct
      {
         uint32_t : 21;
         uint32_t to : 5;
         uint32_t : 6;
      };
      
      struct
      {
         uint32_t : 11;
         uint32_t nb : 5;
         uint32_t : 16;
      };
      
      struct
      {
         uint32_t : 21;
         uint32_t tlbl : 1;
         uint32_t : 10;
      };
   };
};
#endif

#include <emugen_cpu_info.h>

} // namespace ppc

#endif // PPC_CPU_H
