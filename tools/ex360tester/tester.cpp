#include "tester.h"

#include "test.h"

#include <iostream>
#include <filesystem>
#include "powerpc/interpreter/interpreter.h"

Tester g_tester;

Tester::Tester()
{
   m_testMemory = new uint8_t[0x100];
}

Tester::~Tester()
{
   delete[] m_testMemory;
}

bool Tester::run(const std::string &root)
{
   m_testRoot = root;

   for (auto itr = std::tr2::sys::recursive_directory_iterator(root); itr != std::tr2::sys::recursive_directory_iterator(); ++itr) {
      if (std::tr2::sys::is_regular_file(itr->status()) && itr->path().extension().compare(".s") == 0) {
         try {
            if (runTest(itr->path())) {
               std::cout << "[PASS] " << itr->path().filename() << std::endl;
            } else {
               std::cout << "[FAIL] Unknown error in " << itr->path().filename() << std::endl;
            }
         } catch (TestError *error) {
            std::cout << "[FAIL] " << itr->path().filename() << std::endl;
            std::cout << error->what() << std::endl;
            std::cout << error->dumpState() << std::endl;
            delete error;
         }
      }
   }

   return true;
}

bool Tester::runTest(const std::string &path)
{
   Test test;

   if (!test.load(path)) {
      return false;
   }

   return test.run();
}

std::string Tester::getTestRoot()
{
   return m_testRoot;
}

uint64_t Tester::getTestMemoryAddress()
{
   return reinterpret_cast<uint64_t>(m_testMemory);
}
