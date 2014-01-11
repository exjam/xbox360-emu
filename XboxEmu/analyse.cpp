#include "system.h"

#include <util/memory.h>
#include <util/bits.h>

#include <ppc/cpu.h>
#include <ppc/instructions.h>

#include <queue>

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

         break;
      }

      pos += 4;
   }

   return bb;
}

struct Function
{
   uint64_t start;
   std::vector<BasicBlock> blocks;
};

bool System::analyse(uint64_t start)
{
   std::vector<uint64_t> visited;
   std::queue<uint64_t> targets;
   Function func;

   func.start = 0x82b1dde0;

   targets.push(func.start);

   while (targets.size()) {
      auto target = targets.front();

      targets.pop();
      
      if (std::find(visited.begin(), visited.end(), target) != visited.end()) {
         continue;
      }

      BasicBlock bb = getBasicBlock(target);
      
      for (auto succ : bb.successors) {
         targets.push(succ);
      }

      visited.push_back(target);
      func.blocks.push_back(bb);
   }

   for (auto &block : func.blocks) {
      for (auto &other : func.blocks) {
         if (block.start < other.start && block.end > other.end) {
            block = getBasicBlock(block.start, other.start);
         }
      }
   }

   return true;
}
