#include "tester.h"
#include "xenon.h"

#include <iostream>

int main(int argc, char** argv)
{
   if (argc < 3) {
      std::cout << argv[0] << "<xenon\bin> <in dir>" << std::endl;
      return -1;
   }

   xenon::setPath(argv[1]);
   g_tester.run(argv[2]);

   return 0;
}
