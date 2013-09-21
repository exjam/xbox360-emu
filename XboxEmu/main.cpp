#include <stdint.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <iomanip>
#include <assert.h>
#include <windows.h>

#include "xex/xex.h"

#include "interpreterengine.h"

int main(int argc, char** argv)
{
   xex::Loader test;
   std::ifstream file;

   /* Totally legal copy of the game. */
   file.open(L"D:\\Downloads\\Ultimate_Marvel_vs_Capcom_3_RF_XBOX360\\Default.xex", std::ifstream::in | std::ifstream::binary);

   if (!file.is_open()) {
      return false;
   }

   test.load(file);

   file.close();

   InterpreterEngine engine;
   engine.run(0x82F23520);
   //engine.run(0x82F23470);

   /*
   _byteswap_ushort()
   _byteswap_ulong()
   _byteswap_uint64()
   */
   return 0;
}