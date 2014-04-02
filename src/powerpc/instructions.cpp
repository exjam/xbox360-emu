#include "instructions.h"
#include <assert.h>

using ppc::emugen::InstrEntry;
using namespace ppc::Fields;

namespace ppc
{
#include "emugen_instructions.h"

Instruction createInstruction(InstructionID id)
{
   Instruction instr { 0 };

   for (auto &op : ppc::Instructions[static_cast<unsigned>(id)].opcodes) {
      instr.set(op.field, op.value);
   }

   return instr;
}

const ppc::emugen::InstrEntry &getInstruction(InstructionID id)
{
   return Instructions[static_cast<unsigned>(id)];
}

unsigned Instruction::get(Field field) const
{
   switch (field) {
   case Fields::xo1:
      return xo1;
   case Fields::xo2:
      return xo2;
   case Fields::xo3:
      return xo3;
   case Fields::xo4:
      return xo4;
   case Fields::xo5:
      return xo5;
   case Fields::xo6:
      return xo6;
   case Fields::xo7:
      return xo7;
   case Fields::xo8:
      return xo8;
   case Fields::bd:
      return bd;
   case Fields::bi:
      return bi;
   case Fields::bo:
      return bo;
   case Fields::bh:
      return bh;
   case Fields::crbA:
      return crbA;
   case Fields::crbB:
      return crbB;
   case Fields::crbD:
      return crbD;
   case Fields::crfD:
      return crfD;
   case Fields::crfS:
      return crfS;
   case Fields::d:
      return d;
   case Fields::ds:
      return ds;
   case Fields::frA:
      return frA;
   case Fields::frB:
      return frB;
   case Fields::frC:
      return frC;
   case Fields::frD:
      return frD;
   case Fields::frS:
      return frS;
   case Fields::uimm:
      return uimm;
   case Fields::simm:
      return simm;
   case Fields::l:
      return l;
   case Fields::li:
      return li;
   case Fields::rA:
      return rA;
   case Fields::rB:
      return rB;
   case Fields::rD:
      return rD;
   case Fields::rS:
      return rS;
   case Fields::spr:
      return spr;
   case Fields::th:
      return th;
   case Fields::sr:
      return sr;
   case Fields::tbr:
      return tbr;
   case Fields::crm:
      return crm;
   case Fields::fm:
      return fm;
   case Fields::nb:
      return nb;
   case Fields::l15:
      return l15;
   case Fields::syncl:
      return syncl;
   case Fields::tlbl:
      return tlbl;
   case Fields::sh:
      return sh;
   case Fields::shd04:
      return shd04;
   case Fields::shd5:
      return shd5;
   case Fields::mb:
      return mb;
   case Fields::mbd:
      return mbd;
   case Fields::me:
      return me;
   case Fields::med:
      return med;
   case Fields::to:
      return to;
   case Fields::aa:
      return aa;
   case Fields::lk:
      return lk;
   case Fields::oe:
      return oe;
   case Fields::rc:
      return rc;
   default:
      assert(0 && "Unknown field in getField");
      return 0;
   }
}

void Instruction::set(Field field, unsigned value)
{
   switch (field) {
   case Fields::xo1:
      xo1 = value;
      break;
   case Fields::xo2:
      xo2 = value;
      break;
   case Fields::xo3:
      xo3 = value;
      break;
   case Fields::xo4:
      xo4 = value;
      break;
   case Fields::xo5:
      xo5 = value;
      break;
   case Fields::xo6:
      xo6 = value;
      break;
   case Fields::xo7:
      xo7 = value;
      break;
   case Fields::xo8:
      xo8 = value;
      break;
   case Fields::bd:
      bd = value;
      break;
   case Fields::bi:
      bi = value;
      break;
   case Fields::bo:
      bo = value;
      break;
   case Fields::bh:
      bh = value;
      break;
   case Fields::crbA:
      crbA = value;
      break;
   case Fields::crbB:
      crbB = value;
      break;
   case Fields::crbD:
      crbD = value;
      break;
   case Fields::crfD:
      crfD = value;
      break;
   case Fields::crfS:
      crfS = value;
      break;
   case Fields::d:
      d = value;
      break;
   case Fields::ds:
      ds = value;
      break;
   case Fields::frA:
      frA = value;
      break;
   case Fields::frB:
      frB = value;
      break;
   case Fields::frC:
      frC = value;
      break;
   case Fields::frD:
      frD = value;
      break;
   case Fields::frS:
      frS = value;
      break;
   case Fields::uimm:
      uimm = value;
      break;
   case Fields::simm:
      simm = value;
      break;
   case Fields::l:
      l = value;
      break;
   case Fields::li:
      li = value;
      break;
   case Fields::rA:
      rA = value;
      break;
   case Fields::rB:
      rB = value;
      break;
   case Fields::rD:
      rD = value;
      break;
   case Fields::rS:
      rS = value;
      break;
   case Fields::spr:
      spr = value;
      break;
   case Fields::th:
      th = value;
      break;
   case Fields::sr:
      sr = value;
      break;
   case Fields::tbr:
      tbr = value;
      break;
   case Fields::crm:
      crm = value;
      break;
   case Fields::fm:
      fm = value;
      break;
   case Fields::nb:
      nb = value;
      break;
   case Fields::l15:
      l15 = value;
      break;
   case Fields::syncl:
      syncl = value;
      break;
   case Fields::tlbl:
      tlbl = value;
      break;
   case Fields::sh:
      sh = value;
      break;
   case Fields::shd04:
      shd04 = value;
      break;
   case Fields::shd5:
      shd5 = value;
      break;
   case Fields::mb:
      mb = value;
      break;
   case Fields::mbd:
      mbd = value;
      break;
   case Fields::me:
      me = value;
      break;
   case Fields::med:
      med = value;
      break;
   case Fields::to:
      to = value;
      break;
   case Fields::aa:
      aa = value;
      break;
   case Fields::lk:
      lk = value;
      break;
   case Fields::oe:
      oe = value;
      break;
   case Fields::rc:
      rc = value;
      break;
   default:
      assert(0 && "Unknown field in setField");
   }
}
};

