#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>
#include <assert.h>

#define XBXKRNL extern "C"
#define XBXKRNL_UNIMPLEMENTED assert("Unimplemented " ## __FUNCTION__);

#endif
