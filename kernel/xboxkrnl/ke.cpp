#include "ke.h"

enum class ProcessType
{
   Idle = 0,
   User = 1,
   System = 2
};

XBXKRNL uint8_t KeGetCurrentProcessType()
{
   return static_cast<uint8_t>(ProcessType::User);
}