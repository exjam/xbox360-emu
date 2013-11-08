#include "emugen.h"
#include <iostream>

int main(int argc, char **argv)
{
   if (argc < 3) {
      std::cout << "Usage: " << argv[0] << " <definition file> <output directory>" << std::endl;
      return 0;
   }

   EmuGen app;
   app.run(argv[1], argv[2]);

   return 0;
}
