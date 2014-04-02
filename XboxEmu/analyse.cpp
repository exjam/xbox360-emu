#include "system.h"

#include <util/memory.h>
#include <util/bits.h>
#include <util/log.h>

#include <ppc/cpu.h>
#include <ppc/disassembler.h>
#include <ppc/instructions.h>

#include <set>
#include <fstream>
#include <algorithm>

/* Returns true if instruction is a BasicBlock terminator */
bool isTerminator(ppc::Instruction instr)
{
   return
      instr.lk == 0 &&
      (
         ppc::isa<ppc::InstructionID::b>(instr) ||
         ppc::isa<ppc::InstructionID::bc>(instr) ||
         ppc::isa<ppc::InstructionID::bcctr>(instr) ||
         ppc::isa<ppc::InstructionID::bclr>(instr)
      );
}

/* Returns true if instruction is a conditional branch */
bool isConditionalBranch(ppc::Instruction instr)
{
   bool cond_ok = (instr.bo & (1 << 4)) != 0;
   bool ctr_ok  = (instr.bo & (1 << 2)) != 0;

   if (ppc::isa<ppc::InstructionID::bc>(instr)) {
      return !cond_ok || !ctr_ok;
   }

   if (ppc::isa<ppc::InstructionID::bcctr>(instr)) {
      return !cond_ok;
   }

   if (ppc::isa<ppc::InstructionID::bclr>(instr)) {
      return !cond_ok || !ctr_ok;
   }

   return false;
}

/* Return the branch target address */
uint64_t getBranchAddress(uint64_t cia, ppc::Instruction instr)
{
   if (ppc::isa<ppc::InstructionID::b>(instr)) {
      if (instr.aa) {
         return bits::signExtend<26>(static_cast<uint64_t>(instr.li) << 2);
      } else {
         return cia + bits::signExtend<26>(static_cast<uint64_t>(instr.li) << 2);
      }
   }

   if (ppc::isa<ppc::InstructionID::bc>(instr)) {
      if (instr.aa) {
         return bits::signExtend<16>(static_cast<uint64_t>(instr.bd) << 2);
      } else {
         return cia + bits::signExtend<16>(static_cast<uint64_t>(instr.bd) << 2);
      }
   }

   if (ppc::isa<ppc::InstructionID::bcctr>(instr)) {
      /* UNKNOWN */
   }

   if (ppc::isa<ppc::InstructionID::bclr>(instr)) {
      /* UNKNOWN */
   }

   return 0;
}

struct BasicBlock
{
   uint64_t start;
   uint64_t end;
   std::vector<uint64_t> successors;
};

BasicBlock getBasicBlock(uint64_t start, uint64_t end = 0)
{
   BasicBlock bb;
   uint64_t pos, target;

   pos = start;
   bb.start = start;

   while (true) {
      ppc::Instruction instr;

      instr.value = Memory::read<uint32_t>(pos);

      if (isTerminator(instr) || pos == end) {
         bb.end = pos;

         if (target = getBranchAddress(pos, instr)) {
            bb.successors.push_back(target);
         }

         if (isConditionalBranch(instr)) {
            bb.successors.push_back(pos + 4);
         }
         
         if (pos == end) {
            bb.successors.push_back(pos + 4);
         }

         if (isConditionalBranch(instr) && bb.successors.size() == 1) {
            //fuck this hype
            bb.successors.clear();
         } else {
            break;
         }
      }

      pos += 4;
   }

   return bb;
}

struct Function
{
   uint64_t start;
   uint64_t end;
   std::vector<BasicBlock> blocks;
};

bool System::analyse(uint64_t start)
{
   std::vector<uint64_t> visited;
   std::set<uint64_t> targets;
   Function func;

   func.start = start;
   func.end = start;

   targets.insert(func.start);

   while (targets.size()) {
      auto target = *targets.begin();

      if (target > func.end + 4) {
         //FUK U?
      }

      targets.erase(targets.begin());
      
      if (std::find(visited.begin(), visited.end(), target) != visited.end()) {
         continue;
      }

      BasicBlock bb = getBasicBlock(target);

      if (bb.end > func.end) {
         func.end = bb.end;
      }
      
      for (auto succ : bb.successors) {
         /* If before function then its a call */
         if (succ < start) {
            continue;
         }

         /* If after function ... how do we find end? */
         /* If there is a gap in the code! */
         /* Unfortunately we cannot do that until we have "discovered" ALL basic blocks */
         /* Also dynamic switches might fuck it up? */

         targets.insert(succ);
      }

      visited.push_back(target);
      func.blocks.push_back(bb);
   }

   for (auto &block : func.blocks) {
      for (auto &other : func.blocks) {
         if (&block == &other) {
            continue;
         }

         if (block.start >= other.start && block.start <= other.end) {
            other.end = block.start - 4;
            other.successors.clear();
            other.successors.push_back(block.start);
         }
      }
   }

   xDebug() << "FUNCTION " << Log::hex(func.start);
   
   for (auto &block : func.blocks) {
      xDebug() << "BLOCK START " << Log::hex(block.start);

      for (auto cia = block.start; cia <= block.end; cia += 4) {
         ppc::Disassembler::State dis;
         dis.cia = cia;
         ppc::Disassembler::decode(&dis, { Memory::read<uint32_t>(cia) });
         xDebug() << "  " << dis.result.disasm;
      }

      xDebug() << "BLOCK END " << Log::hex(block.end);
   }

   std::fstream out;

   out.open("test.dot", std::fstream::out);

   if (!out.is_open()) {
      std::cout << "Could not open dot graph for writing" << std::endl;
      return false;
   }

   for (auto &block : func.blocks) {
      out << '"';

      if (block.start == func.start) {
         out << "sub_";
      } else {
         out << "loc_";
      }

      out << std::hex << block.start;
      out << '"';

      out << " [ ";
      out << "label = \"";

      for (auto cia = block.start; cia <= block.end; cia += 4) {
         ppc::Disassembler::State dis;
         dis.cia = cia;
         ppc::Disassembler::decode(&dis, { Memory::read<uint32_t>(cia) });

         if (cia != block.start) {
            out << "\\n";
         }

         out << dis.result.disasm;
      }

      out << "\"";
      out << " ];" << std::endl;
   }

   for (auto &block : func.blocks) {
      for (auto &succ : block.successors) {
         if (std::find_if(func.blocks.begin(), func.blocks.end(), [&succ](BasicBlock &bb){ return bb.start == succ; }) == func.blocks.end()) {
            continue;
         }

         out << '"';
         if (block.start == func.start) {
            out << "sub_";
         } else {
            out << "loc_";
         }

         out << std::hex << block.start;
         out << '"';

         out << " -> ";

         out << '"';
         if (succ == func.start) {
            out << "sub_";
         } else {
            out << "loc_";
         }

         out << std::hex << succ;
         out << "\";" << std::endl;
      }
   }

   out.close();

   return true;
}
