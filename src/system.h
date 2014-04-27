#ifndef SYSTEM_H
#define SYSTEM_H

#include "xex/xex.h"
#include "kernel/kernel.h"

#include <map>

class Thread;
class Module;

class System
{
public:
   System();

   bool load(const std::string &path);

   bool start();

   bool analyse(uint64_t start);
   void resumeThread(Thread *thread);

   void addModule(Module *module);
   Module *getModule(std::string name);

   xex::Binary *getBinary();

private:
   bool initKernel();
   bool resolveImports(xex::ImportLibraries &imports);

private:
   xex::Binary mBinary;
   std::map<std::string, Module*> mModules;
};

extern System gSystem;

#endif // System
