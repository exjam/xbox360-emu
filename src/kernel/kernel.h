#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>
#include <assert.h>
#include <type_traits>
#include "common/bits.h"

namespace xex
{
   struct Binary;
};

#pragma pack(push, 1)

#define CHECK_SIZE(Type, Size) \
   static_assert(sizeof(Type) == Size, \
                 #Type " must be " #Size " bytes");

#define CHECK_OFFSET(Type, Offset, Field) \
   static_assert((uint64_t)&((Type*)0)->Field == Offset, \
                 #Type "::" #Field " must be at offset " #Offset);

/* Workaround weird ## behaviour */
#define PP_CAT(a, b) PP_CAT_I(a, b)
#define PP_CAT_I(a, b) PP_CAT_II(~, a ## b)
#define PP_CAT_II(p, res) res

#define UNKNOWN(Size) uint8_t PP_CAT(__unk, __COUNTER__) [Size];
#define PADDING(Size) UNKNOWN(Size)

namespace std
{
   template <bool B, typename T = void>
   struct disable_if
   {
      typedef T type;
   };

   template <typename T>
   struct disable_if<true, T>
   {
   };
};

template<typename Type>
struct ptr32
{
   ptr32()
   {
   }
   
   ptr32(uint32_t value) :
      value(value)
   {
   }

   ptr32 &operator=(Type *src)
   {
      auto uval = reinterpret_cast<uint64_t>(src);
      assert(uval < 0xffffffff);
      value = static_cast<uint32_t>(uval);
      return *this;
   }

   template<typename OtherType>
   ptr32 &operator=(const ptr32<OtherType> &src)
   {
      value = src.value;
      return *this;
   }

   operator bool() const
   {
      return value != 0;
   }

   Type *operator->() const
   {
      return reinterpret_cast<Type*>(value);
   }

   Type &operator*()
   {
      return *reinterpret_cast<Type*>(value);
   }

   uint32_t value;
};

// Specialize with no operator-> or operator*
template<>
struct ptr32<void>
{
   ptr32()
   {
   }

   ptr32(uint32_t value) :
      value(value)
   {
   }

   ptr32 &operator=(void *src)
   {
      auto uval = reinterpret_cast<uint64_t>(src);
      assert(uval < 0xffffffff);
      value = static_cast<uint32_t>(uval);
      return *this;
   }

   template<typename OtherType>
   ptr32 &operator=(const ptr32<OtherType> &src)
   {
      value = src.value;
      return *this;
   }

   operator bool() const
   {
      return value != 0;
   }

   uint32_t value;
};

CHECK_SIZE(ptr32<void>, 0x4);

template<typename VarType, typename ValType>
static inline void beSetVariable(VarType *variable, ValType val)
{
   *variable = bits::swap<VarType>(static_cast<VarType>(val));
}

template<typename VarType>
static inline VarType beGetVariable(const VarType *variable)
{
   return bits::swap<VarType>(*variable);
}

template<typename VarType>
static inline VarType beGetVariable(const ptr32<VarType *> *variable)
{
   return bits::swap<VarType>(**variable);
}

#define beSet(Variable, Value) \
   beSetVariable(&Variable, Value)

#define beGet(Variable) \
   beGetVariable(&Variable)

struct KThread;
struct KProcess;
struct KQueue;
struct KPrcb;
struct KTrapFrame;

using KProcessorMode = uint8_t; // KPROCESSOR_MODE
CHECK_SIZE(KProcessorMode, 0x1);

using KIrql = uint8_t; // KIRQL
CHECK_SIZE(KIrql, 0x1);

struct LinkedListEntry // LIST_ENTRY
{
   ptr32<LinkedListEntry> Front;
   ptr32<LinkedListEntry> Back;
};

CHECK_OFFSET(LinkedListEntry, 0x0, Front);
CHECK_OFFSET(LinkedListEntry, 0x4, Back);
CHECK_SIZE(LinkedListEntry, 0x8);

struct DispatcherHeader // DISPATCHER_HEADER
{
   uint8_t Type;
   uint8_t Absolute;
   uint8_t NpxIrql;
   uint8_t Signalling;
   int32_t SignalState;
   LinkedListEntry WaitList;
};

CHECK_OFFSET(DispatcherHeader, 0x0, Type);
CHECK_OFFSET(DispatcherHeader, 0x1, Absolute);
CHECK_OFFSET(DispatcherHeader, 0x2, NpxIrql);
CHECK_OFFSET(DispatcherHeader, 0x3, Signalling);
CHECK_OFFSET(DispatcherHeader, 0x4, SignalState);
CHECK_OFFSET(DispatcherHeader, 0x8, WaitList);
CHECK_SIZE(DispatcherHeader, 0x10);

struct KCriticalSection // CRITICAL_SECTION
{
   DispatcherHeader Header;
   // using SpinCount = Header.Absolute;
   // using EntryCount = Header.SignalState;
   
   int32_t LockCount;
   uint32_t RecursionCount;
   uint32_t OwningThread;
};

CHECK_OFFSET(KCriticalSection, 0x0, Header);
CHECK_OFFSET(KCriticalSection, 0x10, LockCount);
CHECK_OFFSET(KCriticalSection, 0x14, RecursionCount);
CHECK_OFFSET(KCriticalSection, 0x18, OwningThread);
CHECK_SIZE(KCriticalSection, 0x1c);

struct KMutant // KMUTANT
{
   DispatcherHeader Header;
   LinkedListEntry MutantListEntry;
   ptr32<KThread> OwnerThread;
};

CHECK_OFFSET(KMutant, 0x0, Header);
CHECK_OFFSET(KMutant, 0x10, MutantListEntry);
CHECK_OFFSET(KMutant, 0x18, OwnerThread);
CHECK_SIZE(KMutant, 0x1c);

struct KDpc // KDPC
{
   uint16_t Type;
   uint8_t InsertedNumber;
   uint8_t TargetNumber;
   LinkedListEntry DpcListEntry;
   ptr32<void> DeferredRoutine;
   ptr32<void> DeferredCintext;
   ptr32<void> SystemArgument1;
   ptr32<void> SystemArgument2;
};

CHECK_OFFSET(KDpc, 0x00, Type);
CHECK_OFFSET(KDpc, 0x02, InsertedNumber);
CHECK_OFFSET(KDpc, 0x03, TargetNumber);
CHECK_OFFSET(KDpc, 0x04, DpcListEntry);
CHECK_OFFSET(KDpc, 0x0C, DeferredRoutine);
CHECK_OFFSET(KDpc, 0x10, DeferredCintext);
CHECK_OFFSET(KDpc, 0x14, SystemArgument1);
CHECK_OFFSET(KDpc, 0x18, SystemArgument2);
CHECK_SIZE(KDpc, 0x1c);

struct KTimer // KTIMER
{
   DispatcherHeader Header;
   int64_t DueTime;
   LinkedListEntry TimerListEntry;
   ptr32<KDpc> Dpc;
   int32_t Period;
};

CHECK_OFFSET(KTimer, 0x0, Header);
CHECK_OFFSET(KTimer, 0x10, DueTime);
CHECK_OFFSET(KTimer, 0x18, TimerListEntry);
CHECK_OFFSET(KTimer, 0x20, Dpc);
CHECK_OFFSET(KTimer, 0x24, Period);
CHECK_SIZE(KTimer, 0x28);

struct KWaitBlock // KWAIT_BLOCK
{
   LinkedListEntry WaitListEntry;
   ptr32<KThread> Thread;
   ptr32<void> Object;
   ptr32<KWaitBlock> NextWaitBlock;
   int16_t WaitKey;
   int16_t WaitType;
};

CHECK_OFFSET(KWaitBlock, 0x0, WaitListEntry);
CHECK_OFFSET(KWaitBlock, 0x8, Thread);
CHECK_OFFSET(KWaitBlock, 0xc, Object);
CHECK_OFFSET(KWaitBlock, 0x10, NextWaitBlock);
CHECK_OFFSET(KWaitBlock, 0x14, WaitKey);
CHECK_OFFSET(KWaitBlock, 0x16, WaitType);
CHECK_SIZE(KWaitBlock, 0x18);

struct KSemaphore // KSEMAPHORE
{
   DispatcherHeader Header;
   int32_t Limit;
};

CHECK_OFFSET(KSemaphore, 0x0, Header);
CHECK_OFFSET(KSemaphore, 0x10, Limit);
CHECK_SIZE(KSemaphore, 0x14);

struct KApcState // KAPC_STATE
{
   LinkedListEntry ApcListHead[2];
   ptr32<KProcess> Process;
   uint8_t KernelApcInProgress;
   uint8_t KernelApcPending;
   uint8_t UserApcPending;
};

CHECK_OFFSET(KApcState, 0x0, ApcListHead);
CHECK_OFFSET(KApcState, 0x10, Process);
CHECK_OFFSET(KApcState, 0x14, KernelApcInProgress);
CHECK_OFFSET(KApcState, 0x15, KernelApcPending);
CHECK_OFFSET(KApcState, 0x16, UserApcPending);
CHECK_SIZE(KApcState, 0x17);

struct KApc // KAPC
{
   uint16_t Type;
   KProcessorMode ApcMode;
   UNKNOWN(0x1);
   ptr32<KThread> Thread;
   LinkedListEntry ApcListEntry;
   ptr32<void> KernelRoutine;
   ptr32<void> RundownRoutine;
   ptr32<void> NormalRoutine;
   ptr32<void> NormalContext;
   ptr32<void> SystemArgument1;
   ptr32<void> SystemArgument2;
};

CHECK_OFFSET(KApc, 0x0, Type);
CHECK_OFFSET(KApc, 0x2, ApcMode);
CHECK_OFFSET(KApc, 0x4, Thread);
CHECK_OFFSET(KApc, 0x8, ApcListEntry);
CHECK_OFFSET(KApc, 0x10, KernelRoutine);
CHECK_OFFSET(KApc, 0x14, RundownRoutine);
CHECK_OFFSET(KApc, 0x18, NormalRoutine);
CHECK_OFFSET(KApc, 0x1c, NormalContext);
CHECK_OFFSET(KApc, 0x20, SystemArgument1);
CHECK_OFFSET(KApc, 0x24, SystemArgument2);
CHECK_SIZE(KApc, 0x28);

struct KSpinLock // KSPIN_LOCK
{
   ptr32<uint32_t> Lock;
};

CHECK_OFFSET(KSpinLock, 0x0, Lock);
CHECK_SIZE(KSpinLock, 0x4);

struct KThread // KTHREAD
{
   DispatcherHeader Header;
   LinkedListEntry MutantListHead;
   KTimer Timer;
   KWaitBlock TimerWaitBlock;
   uint32_t KernelTime;
   ptr32<void> InitialStack;
   ptr32<void> StackLimit;
   ptr32<void> KernelStack;
   ptr32<void> TlsData;
   uint8_t State;
   uint8_t Alerted[2];
   uint8_t Alertable;
   uint8_t Priority;
   uint8_t FpuExceptionEnable;
   uint8_t CreateProcessType;
   uint8_t CurrentProcessType;
   KApcState ApcState;
   uint8_t ApcQueueable;
   KSpinLock ApcQueueLock;
   uint32_t ContextSwitches;
   LinkedListEntry ReadyListEntry;
   uint32_t MsrEnableMask;
   ptr32<int32_t> WaitStatus;
   KIrql WaitIrql;
   KProcessorMode WaitMode;
   uint8_t WaitNext;
   uint8_t WaitReason;
   ptr32<KWaitBlock> WaitBlockList;
   UNKNOWN(0x4);
   uint32_t KernelApcDisable;
   uint32_t Quantum;
   int8_t Saturation;
   uint8_t BasePriority;
   uint8_t PriorityDecrement;
   uint8_t DisableBoost;
   int8_t SuspendCount;
   uint8_t Preempted;
   uint8_t HasTerminated;
   uint8_t CurrentProcessor;
   ptr32<struct _KPRCB> CurrentPrcb;
   ptr32<struct _KPRCB> AffinityPrcb;
   uint8_t IdlePriorityClass;
   uint8_t NormalPriorityClass;
   uint8_t TimeCriticalPriorityClass;
   uint8_t HasAsyncTerminated;
   uint32_t ActiveTimerListLock;
   ptr32<void> StackAllocatedBase;
   KApc SuspendApc;
   KSemaphore SuspendSemaphore;
   LinkedListEntry ThreadListEntry;
   ptr32<KQueue> Queue;
   LinkedListEntry QueueListEntry;
   ptr32<void> UserModeDispatcher;
   ptr32<KTrapFrame> UserModeTrapFrame;
   PADDING(0x4);
   uint64_t CreateTime;
   uint64_t ExitTime;
   uint32_t ExitStatus;
   LinkedListEntry ActiveTimerListHead;
   uint32_t ThreadId;
   ptr32<void> StartAddress;
   LinkedListEntry IrpList;
   ptr32<void> DebugMonitorData;
   uint32_t LastWin32ErrorCode;
   ptr32<void> Win32CurrentFiber;
   UNKNOWN(0x4);
   uint32_t CreateOptions;
   UNKNOWN(0x100); // Unknown max size
};

CHECK_OFFSET(KThread, 0x00, Header);
CHECK_OFFSET(KThread, 0x10, MutantListHead);
CHECK_OFFSET(KThread, 0x18, Timer);
CHECK_OFFSET(KThread, 0x40, TimerWaitBlock);
CHECK_OFFSET(KThread, 0x58, KernelTime);
CHECK_OFFSET(KThread, 0x5c, InitialStack);
CHECK_OFFSET(KThread, 0x60, StackLimit);
CHECK_OFFSET(KThread, 0x64, KernelStack);
CHECK_OFFSET(KThread, 0x68, TlsData);
CHECK_OFFSET(KThread, 0x6c, State);
CHECK_OFFSET(KThread, 0x6d, Alerted);
CHECK_OFFSET(KThread, 0x6f, Alertable);
CHECK_OFFSET(KThread, 0x70, Priority);
CHECK_OFFSET(KThread, 0x71, FpuExceptionEnable);
CHECK_OFFSET(KThread, 0x72, CreateProcessType);
CHECK_OFFSET(KThread, 0x73, CurrentProcessType);
CHECK_OFFSET(KThread, 0x74, ApcState);
CHECK_OFFSET(KThread, 0x8b, ApcQueueable);
CHECK_OFFSET(KThread, 0x8c, ApcQueueLock);
CHECK_OFFSET(KThread, 0x90, ContextSwitches);
CHECK_OFFSET(KThread, 0x94, ReadyListEntry);
CHECK_OFFSET(KThread, 0x9c, MsrEnableMask);
CHECK_OFFSET(KThread, 0xa0, WaitStatus);
CHECK_OFFSET(KThread, 0xa4, WaitIrql);
CHECK_OFFSET(KThread, 0xa5, WaitMode);
CHECK_OFFSET(KThread, 0xa6, WaitNext);
CHECK_OFFSET(KThread, 0xa7, WaitReason);
CHECK_OFFSET(KThread, 0xa8, WaitBlockList);
CHECK_OFFSET(KThread, 0xb0, KernelApcDisable);
CHECK_OFFSET(KThread, 0xb4, Quantum);
CHECK_OFFSET(KThread, 0xb8, Saturation);
CHECK_OFFSET(KThread, 0xb9, BasePriority);
CHECK_OFFSET(KThread, 0xba, PriorityDecrement);
CHECK_OFFSET(KThread, 0xbb, DisableBoost);
CHECK_OFFSET(KThread, 0xbc, SuspendCount);
CHECK_OFFSET(KThread, 0xbd, Preempted);
CHECK_OFFSET(KThread, 0xbe, HasTerminated);
CHECK_OFFSET(KThread, 0xbf, CurrentProcessor);
CHECK_OFFSET(KThread, 0xc0, CurrentPrcb);
CHECK_OFFSET(KThread, 0xc4, AffinityPrcb);
CHECK_OFFSET(KThread, 0xc8, IdlePriorityClass);
CHECK_OFFSET(KThread, 0xc9, NormalPriorityClass);
CHECK_OFFSET(KThread, 0xca, TimeCriticalPriorityClass);
CHECK_OFFSET(KThread, 0xcb, HasAsyncTerminated);
CHECK_OFFSET(KThread, 0xcc, ActiveTimerListLock);
CHECK_OFFSET(KThread, 0xd0, StackAllocatedBase);
CHECK_OFFSET(KThread, 0xd4, SuspendApc);
CHECK_OFFSET(KThread, 0xfc, SuspendSemaphore);
CHECK_OFFSET(KThread, 0x110, ThreadListEntry);
CHECK_OFFSET(KThread, 0x118, Queue);
CHECK_OFFSET(KThread, 0x11c, QueueListEntry);
CHECK_OFFSET(KThread, 0x124, UserModeDispatcher);
CHECK_OFFSET(KThread, 0x128, UserModeTrapFrame);
CHECK_OFFSET(KThread, 0x130, CreateTime);
CHECK_OFFSET(KThread, 0x138, ExitTime);
CHECK_OFFSET(KThread, 0x140, ExitStatus);
CHECK_OFFSET(KThread, 0x144, ActiveTimerListHead);
CHECK_OFFSET(KThread, 0x14c, ThreadId);
CHECK_OFFSET(KThread, 0x150, StartAddress);
CHECK_OFFSET(KThread, 0x154, IrpList);
CHECK_OFFSET(KThread, 0x15c, DebugMonitorData);
CHECK_OFFSET(KThread, 0x160, LastWin32ErrorCode);
CHECK_OFFSET(KThread, 0x164, Win32CurrentFiber);
CHECK_OFFSET(KThread, 0x16c, CreateOptions);

struct KProcess // KPROCESS
{
   uint32_t ThreadListLock;
   LinkedListEntry ThreadListHead;
   int32_t ThreadQuantum;
   uint32_t DirectoryTableBase;
   uint32_t ThreadCount;
   uint8_t IdlePriorityClass;
   uint8_t NormalPriorityClass;
   uint8_t TimeCriticalPriorityClass;
   uint8_t DisableQuantum;
   uint32_t DefaultKernelStackSize;
   ptr32<void> TlsStaticDataImage;
   uint32_t SizeOfTlsStaticData;
   uint32_t SizeOfTlsStaticDataImage;
   uint16_t SizeOfTlsSlots;
   uint8_t Terminating;
   uint8_t ProcessType;
   uint32_t TlsSlotBitmap[8];
   uint32_t FileObjectListLock;
   LinkedListEntry FileObjectListHead;
   ptr32<void> Win32DefaultHeapHandle;
   UNKNOWN(0x100); // Unknown max size
};

CHECK_OFFSET(KProcess, 0x00, ThreadListLock);
CHECK_OFFSET(KProcess, 0x04, ThreadListHead);
CHECK_OFFSET(KProcess, 0x0C, ThreadQuantum);
CHECK_OFFSET(KProcess, 0x10, DirectoryTableBase);
CHECK_OFFSET(KProcess, 0x14, ThreadCount);
CHECK_OFFSET(KProcess, 0x18, IdlePriorityClass);
CHECK_OFFSET(KProcess, 0x19, NormalPriorityClass);
CHECK_OFFSET(KProcess, 0x1a, TimeCriticalPriorityClass);
CHECK_OFFSET(KProcess, 0x1b, DisableQuantum);
CHECK_OFFSET(KProcess, 0x1c, DefaultKernelStackSize);
CHECK_OFFSET(KProcess, 0x20, TlsStaticDataImage);
CHECK_OFFSET(KProcess, 0x24, SizeOfTlsStaticData);
CHECK_OFFSET(KProcess, 0x28, SizeOfTlsStaticDataImage);
CHECK_OFFSET(KProcess, 0x2c, SizeOfTlsSlots);
CHECK_OFFSET(KProcess, 0x2e, Terminating);
CHECK_OFFSET(KProcess, 0x2f, ProcessType);
CHECK_OFFSET(KProcess, 0x30, TlsSlotBitmap);
CHECK_OFFSET(KProcess, 0x50, FileObjectListLock);
CHECK_OFFSET(KProcess, 0x54, FileObjectListHead);
CHECK_OFFSET(KProcess, 0x5C, Win32DefaultHeapHandle);

struct KPrcb // KPrcb
{
   ptr32<KThread> CurrentThread;
   ptr32<KThread> NextThread;
   ptr32<KThread> IdleThread;
   uint8_t Number;
   PADDING(0x3);
   uint32_t SetMember;
   uint32_t DpcTime;
   uint32_t InterruptTime;
   uint32_t InterruptCount;
   uint32_t IpiFrozen;
   ptr32<void> CurrentPacket[3];
   uint32_t TargetSet;
   ptr32<void> WorkerRoutine;
   ptr32<KPrcb> SignalDone;
   uint32_t RequestSummary;
   uint32_t DpcInterruptRequested;
   uint32_t DpcLock;
   LinkedListEntry DpcListHead;
   uint32_t DpcRoutineActive;
   uint32_t ReadyListLock;
   uint32_t IdleThreadActive;
   ptr32<void> DeferredReadyListHead;
   uint32_t ReadySummary;
   uint32_t ReadSummaryMask;
   LinkedListEntry DispatcherReadyListHead[32];
   KDpc ThreadReaperDpc;
   LinkedListEntry ThreadReaperListHead;
   KDpc SwitchProcessorThreadDpc;
};

CHECK_OFFSET(KPrcb, 0x00, CurrentThread);
CHECK_OFFSET(KPrcb, 0x04, NextThread);
CHECK_OFFSET(KPrcb, 0x08, IdleThread);
CHECK_OFFSET(KPrcb, 0x0C, Number);
CHECK_OFFSET(KPrcb, 0x10, SetMember);
CHECK_OFFSET(KPrcb, 0x14, DpcTime);
CHECK_OFFSET(KPrcb, 0x18, InterruptTime);
CHECK_OFFSET(KPrcb, 0x1C, InterruptCount);
CHECK_OFFSET(KPrcb, 0x20, IpiFrozen);
CHECK_OFFSET(KPrcb, 0x24, CurrentPacket);
CHECK_OFFSET(KPrcb, 0x30, TargetSet);
CHECK_OFFSET(KPrcb, 0x34, WorkerRoutine);
CHECK_OFFSET(KPrcb, 0x38, SignalDone);
CHECK_OFFSET(KPrcb, 0x3C, RequestSummary);
CHECK_OFFSET(KPrcb, 0x40, DpcInterruptRequested);
CHECK_OFFSET(KPrcb, 0x44, DpcLock);
CHECK_OFFSET(KPrcb, 0x48, DpcListHead);
CHECK_OFFSET(KPrcb, 0x50, DpcRoutineActive);
CHECK_OFFSET(KPrcb, 0x54, ReadyListLock);
CHECK_OFFSET(KPrcb, 0x58, IdleThreadActive);
CHECK_OFFSET(KPrcb, 0x5C, DeferredReadyListHead);
CHECK_OFFSET(KPrcb, 0x60, ReadySummary);
CHECK_OFFSET(KPrcb, 0x64, ReadSummaryMask);
CHECK_OFFSET(KPrcb, 0x68, DispatcherReadyListHead);
CHECK_OFFSET(KPrcb, 0x168, ThreadReaperDpc);
CHECK_OFFSET(KPrcb, 0x184, ThreadReaperListHead);
CHECK_OFFSET(KPrcb, 0x18c, SwitchProcessorThreadDpc);
CHECK_SIZE(KPrcb, 0x1a8);

// This is the structure on r13
struct KPcr // KPCR
{
   ptr32<void> TlsData;
   uint32_t MsrEnableMask;
   uint16_t SoftwareInterrupt;
   uint16_t DpcFpuVpuStateSaved;
   uint8_t DpcCurrentProcessType;
   uint8_t QuantumEnd;
   uint8_t TimerRequest;
   uint8_t HvCr0Save;
   ptr32<KThread> FpuOwnerThread;
   ptr32<KThread> VpuOwnerThread;
   uint8_t CurrentIrql;
   uint8_t BackgroundSchedulingActive;
   uint16_t StartStopBackgroundScheduling;
   uint32_t TimerHand;
   uint64_t LrIarSave;
   uint64_t CrMsrSave;
   uint64_t Gpr13Save;
   uint64_t HvGpr1Save;
   uint64_t HvGpr3Save;
   uint64_t HvGpr4Save;
   uint64_t HvGpr5Save;
   uint32_t UserModeControl;
   ptr32<void> PanicStack;
   uint32_t DarSave;
   uint32_t DsisrSave;
   ptr32<void> DbgLastDpcRoutine;
   uint32_t OnInterruptStack;
   ptr32<void> StackBase;
   ptr32<void> StackLimit;
   ptr32<void> InterruptStackBase;
   ptr32<void> InterruptStackLimit;
   ptr32<void> InterruptRoutine[32];
   KPrcb PrcbData;
   ptr32<KPrcb> Prcb;
   UNKNOWN(0x4);
   ptr32<uint32_t> PixCurrent;
   ptr32<uint32_t> PixLimit;
   ptr32<uint32_t> ProfilerCurrent;
   ptr32<uint32_t> ProfilerLimit;
   uint32_t ProfilerFlags;
   PADDING(0x4);
   uint64_t Contention;
   ptr32<uint32_t> MonitorProfileData;
   UNKNOWN(0x100); // Unknown max size
};

CHECK_OFFSET(KPcr, 0x00, TlsData);
CHECK_OFFSET(KPcr, 0x04, MsrEnableMask);
CHECK_OFFSET(KPcr, 0x08, SoftwareInterrupt);
CHECK_OFFSET(KPcr, 0x0a, DpcFpuVpuStateSaved);
CHECK_OFFSET(KPcr, 0x0c, DpcCurrentProcessType);
CHECK_OFFSET(KPcr, 0x0d, QuantumEnd);
CHECK_OFFSET(KPcr, 0x0e, TimerRequest);
CHECK_OFFSET(KPcr, 0x0f, HvCr0Save);
CHECK_OFFSET(KPcr, 0x10, FpuOwnerThread);
CHECK_OFFSET(KPcr, 0x14, VpuOwnerThread);
CHECK_OFFSET(KPcr, 0x18, CurrentIrql);
CHECK_OFFSET(KPcr, 0x19, BackgroundSchedulingActive);
CHECK_OFFSET(KPcr, 0x1a, StartStopBackgroundScheduling);
CHECK_OFFSET(KPcr, 0x1c, TimerHand);
CHECK_OFFSET(KPcr, 0x20, LrIarSave);
CHECK_OFFSET(KPcr, 0x28, CrMsrSave);
CHECK_OFFSET(KPcr, 0x30, Gpr13Save);
CHECK_OFFSET(KPcr, 0x38, HvGpr1Save);
CHECK_OFFSET(KPcr, 0x40, HvGpr3Save);
CHECK_OFFSET(KPcr, 0x48, HvGpr4Save);
CHECK_OFFSET(KPcr, 0x50, HvGpr5Save);
CHECK_OFFSET(KPcr, 0x58, UserModeControl);
CHECK_OFFSET(KPcr, 0x5c, PanicStack);
CHECK_OFFSET(KPcr, 0x60, DarSave);
CHECK_OFFSET(KPcr, 0x64, DsisrSave);
CHECK_OFFSET(KPcr, 0x68, DbgLastDpcRoutine);
CHECK_OFFSET(KPcr, 0x6c, OnInterruptStack);
CHECK_OFFSET(KPcr, 0x70, StackBase);
CHECK_OFFSET(KPcr, 0x74, StackLimit);
CHECK_OFFSET(KPcr, 0x78, InterruptStackBase);
CHECK_OFFSET(KPcr, 0x7c, InterruptStackLimit);
CHECK_OFFSET(KPcr, 0x80, InterruptRoutine);
CHECK_OFFSET(KPcr, 0x100, PrcbData);
CHECK_OFFSET(KPcr, 0x2a8, Prcb);
CHECK_OFFSET(KPcr, 0x2b0, PixCurrent);
CHECK_OFFSET(KPcr, 0x2b4, PixLimit);
CHECK_OFFSET(KPcr, 0x2b8, ProfilerCurrent);
CHECK_OFFSET(KPcr, 0x2bc, ProfilerLimit);
CHECK_OFFSET(KPcr, 0x2c0, ProfilerFlags);
CHECK_OFFSET(KPcr, 0x2c8, Contention);
CHECK_OFFSET(KPcr, 0x2d0, MonitorProfileData);

#pragma pack(pop)

struct Thread
{
   uint32_t id;
   uint32_t handle;
   uint32_t stackSize;
   uint8_t *stack;
   uint32_t apiThreadStartup;
   uint32_t startAddress;
   uint32_t startParameter;
   uint32_t creationFlags;
   void *state;
   KPcr *pcr;
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
