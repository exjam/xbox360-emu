#ifndef INTERPRETER_ENGINE_H
#define INTERPRETER_ENGINE_H

#include <stdint.h>

class InterpreterEngine
{
public:
   InterpreterEngine();
   bool run(uint32_t address);

private:
};

#endif // INTERPRETER_ENGINE_H
