#include "system.h"
#include <util/log.h>

int main(int argc, char** argv)
{
   if (argc < 2) {
      xDebug() << "Usage: " << argv[0] << " <.xex file>";
      return -1;
   }

   System emu;

   /* Totally legal copy of the game. */
   if (!emu.load(argv[1])) {
      return 0;
   }

   emu.start();
   return 0;
}