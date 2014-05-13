#include "system.h"

#include "kernel/xboxkrnl/process.h"
#include "powerpc/instructions.h"
#include "kernel/module.h"
#include "common/memory.h"

#include "kernel/connectx/connectx.h"
#include "kernel/createprofile/createprofile.h"
#include "kernel/vk/vk.h"
#include "kernel/xam/xam.h"
#include "kernel/xapi/xapi.h"
#include "kernel/xbdm/xbdm.h"
#include "kernel/xboxkrnl/xboxkrnl.h"

bool System::initKernel()
{
   // Initialize process
   KeInitializeProcess();

   // Init each kernel module
   addModule(new connectx);
   addModule(new createprofile);
   addModule(new vk);
   addModule(new xam);
   addModule(new xapi);
   addModule(new xbdm);
   addModule(new xboxkrnl);

   // Linkup all imports to kernel exports
   if (!resolveImports(mBinary.header.importLibraries)) {
      xDebug() << "Could not resolve imports";
      return false;
   }
   
   return true;
}

void System::addModule(Module *module)
{
   mModules.emplace(module->getName(), module);
}

Module *System::getModule(std::string name)
{
   auto itr = mModules.find(name);

   if (itr == mModules.end()) {
      return nullptr;
   } else {
      return itr->second;
   }
}

// Link xex imports to module exports
bool System::resolveImports(xex::ImportLibraries &imports)
{
   for (auto &library : imports.libraries) {
      auto module = getModule(library.name);

      if (!module) {
         continue;
      }

      for (auto &import : library.imports) {
         auto export_ = module->getExport(import.ordinal);

         if (export_) {
            if (import.thunk) {
               // Write kernel call instruction at import.thunk
               auto instr = ppc::createInstruction(ppc::InstructionID::krncall);
               instr.uimm = export_->ordinal;
               instr.rA = export_->args;
               instr.rD = 1;

               Memory::write<uint32_t>(import.thunk,
                                       instr.value);

               Memory::write<uint64_t>(import.thunk + 4,
                                       reinterpret_cast<uint64_t>(export_));
            } else {
               // Write pointer to exported variable at import.address
               Memory::write<uint32_t>(import.address,
                                       reinterpret_cast<uint32_t>(export_->ptr));
            }
         } else {
            if (import.thunk) {
               // Write debug "missing import" kernel call
               auto instr = ppc::createInstruction(ppc::InstructionID::krncall);
               instr.uimm = import.ordinal;
               instr.rA = instr.rD = 0;

               Memory::write<uint32_t>(import.thunk, instr.value);

               Memory::write<uint64_t>(import.thunk + 4,
                                       reinterpret_cast<uint64_t>(&library));

               xDebug() << "Missing function import "
                        << library.name
                        << "[" << import.ordinal << "]";
            } else {
               // Write ordinal as pointer, will cause a crash at reading the ordinal number
               Memory::write<uint32_t>(import.address,
                                       import.ordinal);

               xDebug() << "Missing variable import "
                        << library.name
                        << "[" << import.ordinal << "]";
            }
         }
      }
   }

   return true;
}
