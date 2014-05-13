#include "system.h"
#include "common/log.h"

System gSystem;

int __cdecl main(int argc, char** argv)
{
   if (argc < 2) {
      xDebug() << "Usage: " << argv[0] << " <.xex file>";
      return -1;
   }

   /* Totally legal copy of the game. */
   if (!gSystem.load(argv[1])) {
      return 0;
   }

   gSystem.start();

   return 0;
}