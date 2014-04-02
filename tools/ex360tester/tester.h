#ifndef TESTER_H
#define TESTER_H

#include <string>
#include <stdint.h>

class Tester
{
public:
   Tester();
   ~Tester();

   bool run(const std::string &root);

   std::string getTestRoot();
   uint64_t getTestMemoryAddress();

private:
   bool runTest(const std::string &test);

private:
   std::string m_testRoot;
   uint8_t *m_testMemory;
};

extern Tester g_tester;

#endif // ifndef TESTER_H
