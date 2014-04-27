#ifndef XBOXKRNL_H
#define XBOXKRNL_H

#include <stdint.h>
#include "common/structmacros.h"
#include "kernel/module.h"

class xboxkrnl : public Module
{
public:
   xboxkrnl();

private:
   void addExports();
};

#endif // XBOXKRNL_H
