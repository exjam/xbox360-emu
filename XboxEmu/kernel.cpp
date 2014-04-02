#include "system.h"

bool System::initKernel()
{
   if (!resolveImports(mBinary.header.importLibraries)) {
      xDebug() << "Could not resolve imports";
      return false;
   }
   
   return true;
}
