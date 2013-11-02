#ifndef INTERPRETER_ENGINE_H
#define INTERPRETER_ENGINE_H

#include <stdint.h>
#include "xex/xex.h"

class InterpreterEngine
{
public:
   InterpreterEngine();
   bool run(xex::Binary &xex);

private:
};

#endif // INTERPRETER_ENGINE_H
