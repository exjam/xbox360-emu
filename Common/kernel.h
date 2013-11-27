#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>
#include <assert.h>
#include <util/bits.h>

namespace xex { struct Binary; };

using XVOID       = void;
using XCHAR       = char;
using XSHORT      = int16_t;
using XINT        = int32_t;
using XBOOL       = int32_t;
using XBYTE       = uint8_t;
using XWORD       = uint16_t;
using XDWORD      = uint32_t;

#pragma pack(push, 1)
template<typename Type, bool BigEndian = false>
struct XLP
{
   XLP() :
      value(0)
   {
   }

   XLP(Type *addr)
   {
      *this = addr;
   }

   template<typename OtherType, bool OtherEndian>
   XLP(const XLP<OtherType, OtherEndian>& other)
   {
      *this = other;
   }

   XLP<Type, BigEndian> & operator =(const Type* ptr)
   {
      uint64_t lptr = reinterpret_cast<uint64_t>(ptr);
      assert(((lptr & 0xFFFFFFFF) == 0) || ((lptr & 0xFFFFFFFF) == lptr));
      value = static_cast<XDWORD>(lptr);

      if (BigEndian) {
         value = bits::swap(value);
      }

      return *this;
   }

   XLP<Type, BigEndian> & operator =(uint64_t lptr)
   {
      assert(((lptr & 0xFFFFFFFF) == 0) || ((lptr & 0xFFFFFFFF) == lptr));
      value = static_cast<XDWORD>(lptr);

      if (BigEndian) {
         value = bits::swap(value);
      }

      return *this;
   }

   template<typename OtherType, bool OtherEndian>
   XLP<Type, BigEndian> & operator =(const XLP<OtherType, OtherEndian>& o)
   {
      value = static_cast<XDWORD>(static_cast<uint64_t>(o));
      return *this;
   }

   template<typename DstType>
   DstType *as() const
   {
      if (BigEndian) {
         return reinterpret_cast<DstType *>(bits::swap(value));
      } else {
         return reinterpret_cast<DstType *>(value);
      }
   }

   template<typename DstType>
   const DstType *as() const
   {
      if (BigEndian) {
         return reinterpret_cast<DstType *>(bits::swap(value));
      } else {
         return reinterpret_cast<DstType *>(value);
      }
   }

   operator bool() const
   {
      return value != 0;
   }

   operator uint64_t() const
   {
      if (BigEndian) {
         return bits::swap(value);
      } else {
         return value;
      }
   }

   operator Type *()
   {
      if (BigEndian) {
         return reinterpret_cast<Type *>(bits::swap(value));
      } else {
         return reinterpret_cast<Type *>(value);
      }
   }

   operator const Type *() const
   {
      if (BigEndian) {
         return reinterpret_cast<Type *>(bits::swap(value));
      } else {
         return reinterpret_cast<Type *>(value);
      }
   }

   Type *operator->() const
   {
      if (BigEndian) {
         return reinterpret_cast<Type *>(bits::swap(value));
      } else {
         return reinterpret_cast<Type *>(value);
      }
   }
   
   XDWORD value;
};
#pragma pack(pop)

using XPVOID      = XVOID *;

using XLPVOID     = XLP<XVOID>;
using XLPCHAR     = XLP<XCHAR>;
using XLPBYTE     = XLP<XBYTE>;
using XLPSHORT    = XLP<XSHORT>;
using XLPINT      = XLP<XINT>;
using XLPBOOL     = XLP<XBOOL>;
using XLPWORD     = XLP<XWORD>;
using XLPDWORD    = XLP<XDWORD>;

using XNCALLER_TYPE = XDWORD;
using XHANDLE       = XLPVOID;
using XLPHANDLE     = XLP<XHANDLE>;

struct XThreadLocalStorage
{
   //KfAcquireSpinLock(+0x0)
   XBYTE _unk1[0x2c];     //0x00
   XDWORD tlsSize;        //0x2c
   XDWORD tlsVarBitset[8];//0x30 //0x7fffffff ...
};

struct XThreadState /* probably struct _PKTHREAD */
{
   XBYTE _unk1[0x84];            //0x00
   //0x58 kernel time
   XLP<XThreadLocalStorage, true> tls; //0x84
   XBYTE _unk2[0xc4];            //0x88
   XDWORD threadId;              //0x14c
   XDWORD enableSetError;        //0x150
   XBYTE _unk3[0xc];             //0x154
   XDWORD lastError;             //0x160
   XBYTE _unkSize[0x500];
};

struct XThread
{
   XLPBYTE tlsVars;         //0x0 -> 256*DWORD tls vars
   XBYTE _unk1[0xfc];       //0x4
   XLP<XThreadState, true> state; //0x100
   XBYTE _unk2[0x8];        //0x104
   XBYTE cpuId;            //0x10c
   //XWORD KeAcquireSpinkLockAtRaisedIrql(0x154)
   //0x158 if (WORD 0x158 == 0) { FindReadyThread }
   XBYTE _unkSize[0x500];
};

/*
_KTHREAD
_KDPC
http://doxygen.reactos.org/dc/d1d/wdm_8h_source.html
*/
/* struct _KTHREAD / r13+0x100
lbz 0x73 // Current Process Type : KeGetCurrentProcessType
*/

/*
struct r13 {
lwz 0x4   // KiCommonInterruptExit KiDiscardVpuContextThread KiDiscardFpuContextThread KeSetCurrentStackPointers
sth 0x4   // KiFlushVpuContextThread KiDiscardVpuContextThread
sth 0x6   // KiDiscardFpuContextThread KiFlushFpuContextThread KeRestoreFloatingPointState
stb 0x8   // KiDispatchInterrupt KiSwitchProcessorThread KiExitDispatcher
lhz 0x8   // KiDeliverSoftwareInterrupt KiRequestSoftwareInterrupt KfLowerIrql KfReleaseSpinLock KiExitDispatcher
stb 0x9   // KiDeliverSoftwareInterrupt
stb 0xa   // KeRestoreFloatingPointState
lhz 0xa   // KeRetireDpcList
lbz 0xc   // Current Process Type : KeRetireDpcList KeSetCurrentProcessType
stb 0xd   // KiDispatchInterrupt KiIdleLoop
lbz 0xe   // KiDispatchInterrupt
stw 0x10  // KiDiscardFpuContextThread KiFlushFpuContextThread KeRestoreFloatingPointState
stw 0x14  // KiFlushVpuContextThread KiDiscardVpuContextThread
lbz 0x18  // RtlGetLastError
stb 0x19  // KiQuantumEnd
sth 0x1a  // KiQuantumEnd
lbz 0x1b  // KiQuantumEnd
lwz 0x1c  // struct PKDPC* KiRetireDpcList
ld  0x20  // KiCommonInterruptHandler
ld  0x28  // KiCommonInterruptHandler
ld  0x30  // KiCommonInterruptHandler
lwz 0x68  // "The stack address of the original thread" "The total number of dirty pages" sub_8008A8E8 KeGetCurrentProcessType .. (Sort sort of error code store?)
lwz 0x6c  // RtlGetStackLimits KiDispatchInterrupt
lwz 0x70  // Current Stack Low : RtlGetStackLimits KeSetCurrentStackPointers
lwz 0x74  // Current Stack High : RtlGetStackLimits KeSetCurrentStackPointers
lwz 0x78  // RtlGetStackLimits KiDispatchInterrupt
lwz 0x7c  // RtlGetStackLimits
lwz 0x100 // struct _KTHREAD* -> CUR_THREAD PTR
lwz 0x104 // struct _KTHREAD* -> NEXT_THREAD PTR KiDispatchInterrupt KiExitDispatcher KiExitDispatcherSwapThread KiSwapThread
lbz 0x10c // Processor ID KeSetAffinityThread
lwz 0x148 // KiDispatchInterrupt KeRetireDpcList KiIdleLoop
lwz 0x150 // RtlGetLastError KiExitDispatcher KeRetireDpcList KeRestoreFloatingPointState
lwz 0x15c // struct  KiDispatchInterrupt KiExitDispatcher KiSwapWaitingThread
spinlock 0x154 // KiSwapWaitingThread
lwz 0x160 // RtlGetLastError KiSwapThread
lwz 0x164 // KiSwapThread
lwz 0x2c0 // KeAcquireSpinLockAtRaisedIrql KiExitDispatcher 
ld  0x2c8 // KeAcquireSpinLockAtRaisedIrql KiExitDispatcher KiSwapWaitingThreadSpinOnSpinLock
lwz 0x300 // ptr to a struct KiIdleLoop
}
*/

/* KiDecodeFpuException
if (lbz 0xa(r13)) {
   lwz r10, 0x100(r13)
} else {
   lwz r10, 0x108(r13)
}

lwz 0x980(r10)
*/
/* KiDeliverUserApc
KfAcquireSpinLock(addi 0x8c(lwz 0x100(r13)))
*/

/* KiQueueReadyThread
ALL for XThreadState / _PKTHREAD / lwz 0x100(r13)
if (lwz 0xc4(r3) == 0x100(r13)) {
lbz 0xbd(r3)
} else {
stw 0x94(r3)
stw 0x94 + r3, 0x15c(r13)
}

*/

/* KiCommonInterruptHandler
lwz 0x58(lwz 0x100(r13))
*/

/* KiQuantumEnd
spinlock lwz 0x54(lwz 0x100(r13))
*/

/* 
*/

/* KiClockInterrupt
lwz 0x58(lwz 0x100(r13))
lwz 0xb4(lwz 0x100(r13))
*/

/* KiCommonDispatchException
lwz 0x124(lwz 0x100(r13))
*/

/*
RtlGetLastError

if (lbz 0x18(r13) <= 2) {
   lwz r11, 0x150(r13)

   if (r11 != 0) {
      return 0;
   } else {
      lwz r11, 0x100(r13)
      lwz r3, 0x160(r13)
      return r3;
   }
} else {
   //kaboompowtraperror
}
*/

/*
RtlGetStackLimits(XLPDWORD lo, XLPDWORD hi)

if (lwz 0x6c(13) == 0) {
   *lo = 0x70(13)
   *hi = 0x74(13)
} else {
   *lo = 0x78(13)
   *hi = 0x7c(13)
}
*/

struct Thread
{
   XDWORD id;
   XHANDLE handle;
   XDWORD stackSize;
   XBYTE *stack;
   XLPVOID apiThreadStartup;
   XLPVOID startAddress;
   XLPVOID startParameter;
   XDWORD creationFlags;
   XVOID *state;
   XThread xthread;
};

class IKernel
{
public:
   virtual xex::Binary *getXexBinary() = 0;
   virtual void resumeThread(Thread *thread) = 0;
};

template<typename SrcType, typename DstType>
void swapField(const SrcType src, DstType dst)
{
   if (dst && src) {
      *dst = bits::swap(*src);
   }
}

#define XBXKRNL extern "C"
#define XBXKRNL_UNIMPLEMENTED assert("Unimplemented " ## __FUNCTION__);

#endif
