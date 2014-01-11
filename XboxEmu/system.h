#ifndef SYSTEM_H
#define SYSTEM_H

#include "xex/xex.h"
#include <kernel.h>

class System : protected IKernel
{
public:
   System();

   bool load(const std::string &path);

   bool start();

private:
   bool initKernel();
   bool resolveImports(xex::ImportLibraries &imports);
   bool loadLibrary(xex::ImportLibrary &library);
   bool loadImport(xex::ImportLibrary &library, xex::ImportLibrary::Import &import);

   bool analyse(uint64_t start);

protected: /* IKernel */
   void resumeThread(Thread *thread);
   xex::Binary *getXexBinary();

private:
   xex::Binary mBinary;
   std::vector<Thread*> mThreads;
};

#endif // System
