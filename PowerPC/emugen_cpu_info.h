struct Registers_ {
   uint64_t gpr[32];
   double fpr[32];
   union {
      uint32_t value;
      struct {
         uint32_t cr0 : 4;
         uint32_t cr1 : 4;
         uint32_t cr2 : 4;
         uint32_t cr3 : 4;
         uint32_t cr4 : 4;
         uint32_t cr5 : 4;
         uint32_t cr6 : 4;
         uint32_t cr7 : 5;
      };
   } cr;
   union {
      uint32_t value;
      struct {
         uint32_t fx : 1;
         uint32_t fex : 1;
         uint32_t vx : 1;
         uint32_t ox : 1;
         uint32_t ux : 1;
         uint32_t zx : 1;
         uint32_t xx : 1;
         uint32_t vxsnan : 1;
         uint32_t vxisi : 1;
         uint32_t vxidi : 1;
         uint32_t vxzdz : 1;
         uint32_t vximz : 1;
         uint32_t vxvc : 1;
         uint32_t fr : 1;
         uint32_t fi : 1;
         uint32_t fprf : 5;
         uint32_t : 1;
         uint32_t vxsoft : 1;
         uint32_t vxsqrt : 1;
         uint32_t vxcvi : 1;
         uint32_t ve : 1;
         uint32_t oe : 1;
         uint32_t ue : 1;
         uint32_t ze : 1;
         uint32_t xe : 1;
         uint32_t ni : 1;
         uint32_t rn : 2;
      };
   } fpscr;
   union {
      uint64_t value;
      struct {
         uint64_t : 32;
         uint64_t so : 1;
         uint64_t ov : 1;
         uint64_t ca : 1;
         uint64_t : 22;
         uint64_t bt : 7;
      };
   } xer;
   uint64_t lr;
   uint64_t ctr;
   uint32_t tbu;
   uint32_t tbl;
   uint64_t msr;
   uint32_t pvr;
   uint64_t sdr1;
   uint64_t asr;
   uint64_t dar;
   uint32_t dsisr;
   uint64_t srr0;
   uint64_t srr1;
   uint64_t sprg0;
   uint64_t sprg1;
   uint64_t sprg2;
   uint64_t sprg3;
   uint64_t fpecr;
   uint32_t dec;
   uint64_t dabr;
   uint64_t pir;
   uint32_t ear;
   uint64_t ctrl;
   uint64_t iabr;
};

namespace FieldID {
enum Fields {
   xo1,
   xo2,
   xo3,
   xo4,
   xo5,
   xo6,
   xo7,
   xo8,
   bd,
   bi,
   bo,
   bh,
   crbA,
   crbB,
   crbD,
   crfD,
   crfS,
   d,
   ds,
   frA,
   frB,
   frC,
   frD,
   frS,
   uimm,
   simm,
   l,
   li,
   rA,
   rB,
   rD,
   rS,
   spr,
   th,
   sr,
   tbr,
   crm,
   fm,
   nb,
   l15,
   syncl,
   tlbl,
   sh,
   shd04,
   shd5,
   mb,
   mbd,
   me,
   med,
   to,
   aa,
   lk,
   oe,
   rc,
};
};

union Instruction {
   uint32_t value;

   struct {
      uint32_t : 26;
      uint32_t xo1 : 6;
   };

   struct {
      uint32_t : 1;
      uint32_t xo2 : 10;
      uint32_t : 21;
   };

   struct {
      uint32_t : 1;
      uint32_t xo3 : 9;
      uint32_t : 22;
   };

   struct {
      uint32_t : 1;
      uint32_t xo4 : 5;
      uint32_t : 26;
   };

   struct {
      uint32_t xo5 : 2;
      uint32_t : 30;
   };

   struct {
      uint32_t : 20;
      uint32_t xo6 : 1;
      uint32_t : 11;
   };

   struct {
      uint32_t : 1;
      uint32_t xo7 : 4;
      uint32_t : 27;
   };

   struct {
      uint32_t : 2;
      uint32_t xo8 : 3;
      uint32_t : 27;
   };

   struct {
      uint32_t : 2;
      uint32_t bd : 14;
      uint32_t : 16;
   };

   struct {
      uint32_t : 16;
      uint32_t bi : 5;
      uint32_t : 11;
   };

   struct {
      uint32_t : 21;
      uint32_t bo : 5;
      uint32_t : 6;
   };

   struct {
      uint32_t : 11;
      uint32_t bh : 2;
      uint32_t : 19;
   };

   struct {
      uint32_t : 16;
      uint32_t crbA : 5;
      uint32_t : 11;
   };

   struct {
      uint32_t : 11;
      uint32_t crbB : 5;
      uint32_t : 16;
   };

   struct {
      uint32_t : 21;
      uint32_t crbD : 5;
      uint32_t : 6;
   };

   struct {
      uint32_t : 23;
      uint32_t crfD : 3;
      uint32_t : 6;
   };

   struct {
      uint32_t : 18;
      uint32_t crfS : 3;
      uint32_t : 11;
   };

   struct {
      uint32_t d : 16;
      uint32_t : 16;
   };

   struct {
      uint32_t : 2;
      uint32_t ds : 14;
      uint32_t : 16;
   };

   struct {
      uint32_t : 16;
      uint32_t frA : 5;
      uint32_t : 11;
   };

   struct {
      uint32_t : 11;
      uint32_t frB : 5;
      uint32_t : 16;
   };

   struct {
      uint32_t : 6;
      uint32_t frC : 5;
      uint32_t : 21;
   };

   struct {
      uint32_t : 21;
      uint32_t frD : 5;
      uint32_t : 6;
   };

   struct {
      uint32_t : 21;
      uint32_t frS : 5;
      uint32_t : 6;
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
      uint32_t : 21;
      uint32_t l : 2;
      uint32_t : 9;
   };

   struct {
      uint32_t : 2;
      uint32_t li : 24;
      uint32_t : 6;
   };

   struct {
      uint32_t : 16;
      uint32_t rA : 5;
      uint32_t : 11;
   };

   struct {
      uint32_t : 11;
      uint32_t rB : 5;
      uint32_t : 16;
   };

   struct {
      uint32_t : 21;
      uint32_t rD : 5;
      uint32_t : 6;
   };

   struct {
      uint32_t : 21;
      uint32_t rS : 5;
      uint32_t : 6;
   };

   struct {
      uint32_t : 11;
      uint32_t spr : 10;
      uint32_t : 11;
   };

   struct {
      uint32_t : 21;
      uint32_t th : 2;
      uint32_t : 9;
   };

   struct {
      uint32_t : 16;
      uint32_t sr : 4;
      uint32_t : 12;
   };

   struct {
      uint32_t : 11;
      uint32_t tbr : 10;
      uint32_t : 11;
   };

   struct {
      uint32_t : 12;
      uint32_t crm : 8;
      uint32_t : 12;
   };

   struct {
      uint32_t : 17;
      uint32_t fm : 8;
      uint32_t : 7;
   };

   struct {
      uint32_t : 11;
      uint32_t nb : 5;
      uint32_t : 16;
   };

   struct {
      uint32_t : 16;
      uint32_t l15 : 1;
      uint32_t : 15;
   };

   struct {
      uint32_t : 21;
      uint32_t syncl : 2;
      uint32_t : 9;
   };

   struct {
      uint32_t : 21;
      uint32_t tlbl : 1;
      uint32_t : 10;
   };

   struct {
      uint32_t : 11;
      uint32_t sh : 5;
      uint32_t : 16;
   };

   struct {
      uint32_t : 11;
      uint32_t shd04 : 5;
      uint32_t : 16;
   };

   struct {
      uint32_t : 1;
      uint32_t shd5 : 1;
      uint32_t : 30;
   };

   struct {
      uint32_t : 6;
      uint32_t mb : 5;
      uint32_t : 21;
   };

   struct {
      uint32_t : 5;
      uint32_t mbd : 6;
      uint32_t : 21;
   };

   struct {
      uint32_t : 1;
      uint32_t me : 5;
      uint32_t : 26;
   };

   struct {
      uint32_t : 5;
      uint32_t med : 6;
      uint32_t : 21;
   };

   struct {
      uint32_t : 21;
      uint32_t to : 5;
      uint32_t : 6;
   };

   struct {
      uint32_t : 1;
      uint32_t aa : 1;
      uint32_t : 30;
   };

   struct {
      uint32_t lk : 1;
      uint32_t : 31;
   };

   struct {
      uint32_t : 10;
      uint32_t oe : 1;
      uint32_t : 21;
   };

   struct {
      uint32_t rc : 1;
      uint32_t : 31;
   };
};

