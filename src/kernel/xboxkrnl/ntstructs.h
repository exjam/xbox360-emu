#ifndef XBOXKRNL_NTSTRUCTS_H
#define XBOXKRNL_NTSTRUCTS_H

#include "kernel/xboxkrnl/xboxkrnl.h"
#include "common/bigendian.h"

#pragma pack(push, 1)

// Forward declare all NT structs
struct KListEntry;
struct KDispatcherHeader;
struct KCriticalSection;
struct KMutant;
struct KSemaphore;
struct KSpinLock;
struct KQueue;
struct KDpc;
struct KTimer;
struct KWaitBlock;
struct KApcState;
struct KApc;
struct KThread;
struct KProcess;
struct KPrcb;
struct KPcr;
struct KAnsiString;
struct KUnicodeString;
struct KLdrDataTableEntry;

// KPROCESSOR_MODE
using KProcessorMode = uint8_t;
CHECK_SIZE(KProcessorMode, 0x1);

// KIRQL
using KIrql = uint8_t;
CHECK_SIZE(KIrql, 0x1);

// LIST_ENTRY
struct KListEntry
{
   be_ptr32_t<KListEntry> Front;
   be_ptr32_t<KListEntry> Back;
};

CHECK_OFFSET(KListEntry, 0x0, Front);
CHECK_OFFSET(KListEntry, 0x4, Back);
CHECK_SIZE(KListEntry, 0x8);

// DISPATCHER_HEADER
struct KDispatcherHeader
{
   be_uint8_t Type;
   be_uint8_t Absolute;
   be_uint8_t NpxIrql;
   be_uint8_t Signalling;
   be_int32_t SignalState;
   KListEntry WaitList;
};

CHECK_OFFSET(KDispatcherHeader, 0x0, Type);
CHECK_OFFSET(KDispatcherHeader, 0x1, Absolute);
CHECK_OFFSET(KDispatcherHeader, 0x2, NpxIrql);
CHECK_OFFSET(KDispatcherHeader, 0x3, Signalling);
CHECK_OFFSET(KDispatcherHeader, 0x4, SignalState);
CHECK_OFFSET(KDispatcherHeader, 0x8, WaitList);
CHECK_SIZE(KDispatcherHeader, 0x10);

// CRITICAL_SECTION
struct KCriticalSection
{
   KDispatcherHeader Header;
   // using SpinCount = Header.Absolute;
   // using EntryCount = Header.SignalState;

   be_int32_t LockCount;
   be_uint32_t RecursionCount;
   be_uint32_t OwningThread;
};

CHECK_OFFSET(KCriticalSection, 0x0, Header);
CHECK_OFFSET(KCriticalSection, 0x10, LockCount);
CHECK_OFFSET(KCriticalSection, 0x14, RecursionCount);
CHECK_OFFSET(KCriticalSection, 0x18, OwningThread);
CHECK_SIZE(KCriticalSection, 0x1c);

// KMUTANT
struct KMutant
{
   KDispatcherHeader Header;
   KListEntry MutantListEntry;
   be_ptr32_t<KThread> OwnerThread;
};

CHECK_OFFSET(KMutant, 0x0, Header);
CHECK_OFFSET(KMutant, 0x10, MutantListEntry);
CHECK_OFFSET(KMutant, 0x18, OwnerThread);
CHECK_SIZE(KMutant, 0x1c);

// KSEMAPHORE
struct KSemaphore
{
   KDispatcherHeader Header;
   be_int32_t Limit;
};

CHECK_OFFSET(KSemaphore, 0x0, Header);
CHECK_OFFSET(KSemaphore, 0x10, Limit);
CHECK_SIZE(KSemaphore, 0x14);

// KSPIN_LOCK
struct KSpinLock
{
   be_ptr32_t<be_uint32_t> Lock;
};

CHECK_OFFSET(KSpinLock, 0x0, Lock);
CHECK_SIZE(KSpinLock, 0x4);

// KQUEUE
struct KQueue
{
   KDispatcherHeader Header;
   KListEntry EntryListHead;
   be_uint32_t CurrentCount;
   be_uint32_t MaximumCount;
   KListEntry ThreadListHead;
};

CHECK_OFFSET(KQueue, 0x0, Header);
CHECK_OFFSET(KQueue, 0x10, EntryListHead);
CHECK_OFFSET(KQueue, 0x18, CurrentCount);
CHECK_OFFSET(KQueue, 0x1C, MaximumCount);
CHECK_OFFSET(KQueue, 0x20, ThreadListHead);
CHECK_SIZE(KQueue, 0x28);

// KDPC
struct KDpc
{
   be_uint16_t Type;
   be_uint8_t InsertedNumber;
   be_uint8_t TargetNumber;
   KListEntry DpcListEntry;
   be_ptr32_t<void> DeferredRoutine;
   be_ptr32_t<void> DeferredCintext;
   be_ptr32_t<void> SystemArgument1;
   be_ptr32_t<void> SystemArgument2;
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

// KTIMER
struct KTimer
{
   KDispatcherHeader Header;
   be_int64_t DueTime;
   KListEntry TimerListEntry;
   be_ptr32_t<KDpc> Dpc;
   be_int32_t Period;
};

CHECK_OFFSET(KTimer, 0x0, Header);
CHECK_OFFSET(KTimer, 0x10, DueTime);
CHECK_OFFSET(KTimer, 0x18, TimerListEntry);
CHECK_OFFSET(KTimer, 0x20, Dpc);
CHECK_OFFSET(KTimer, 0x24, Period);
CHECK_SIZE(KTimer, 0x28);

// KWAIT_BLOCK
struct KWaitBlock
{
   KListEntry WaitListEntry;
   be_ptr32_t<KThread> Thread;
   be_ptr32_t<void> Object;
   be_ptr32_t<KWaitBlock> NextWaitBlock;
   be_int16_t WaitKey;
   be_int16_t WaitType;
};

CHECK_OFFSET(KWaitBlock, 0x0, WaitListEntry);
CHECK_OFFSET(KWaitBlock, 0x8, Thread);
CHECK_OFFSET(KWaitBlock, 0xc, Object);
CHECK_OFFSET(KWaitBlock, 0x10, NextWaitBlock);
CHECK_OFFSET(KWaitBlock, 0x14, WaitKey);
CHECK_OFFSET(KWaitBlock, 0x16, WaitType);
CHECK_SIZE(KWaitBlock, 0x18);

// KAPC_STATE
struct KApcState
{
   KListEntry ApcListHead[2];
   be_ptr32_t<KProcess> Process;
   be_uint8_t KernelApcInProgress;
   be_uint8_t KernelApcPending;
   be_uint8_t UserApcPending;
};

CHECK_OFFSET(KApcState, 0x0, ApcListHead);
CHECK_OFFSET(KApcState, 0x10, Process);
CHECK_OFFSET(KApcState, 0x14, KernelApcInProgress);
CHECK_OFFSET(KApcState, 0x15, KernelApcPending);
CHECK_OFFSET(KApcState, 0x16, UserApcPending);
CHECK_SIZE(KApcState, 0x17);

// KAPC
struct KApc
{
   be_uint16_t Type;
   KProcessorMode ApcMode;
   UNKNOWN(0x1);
   be_ptr32_t<KThread> Thread;
   KListEntry ApcListEntry;
   be_ptr32_t<void> KernelRoutine;
   be_ptr32_t<void> RundownRoutine;
   be_ptr32_t<void> NormalRoutine;
   be_ptr32_t<void> NormalContext;
   be_ptr32_t<void> SystemArgument1;
   be_ptr32_t<void> SystemArgument2;
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

// KTHREAD
struct KThread
{
   KDispatcherHeader Header;
   KListEntry MutantListHead;
   KTimer Timer;
   KWaitBlock TimerWaitBlock;
   be_uint32_t KernelTime;
   be_ptr32_t<void> InitialStack;
   be_ptr32_t<void> StackLimit;
   be_ptr32_t<void> KernelStack;
   be_ptr32_t<void> TlsData;
   be_uint8_t State;
   be_uint8_t Alerted[2];
   be_uint8_t Alertable;
   be_uint8_t Priority;
   be_uint8_t FpuExceptionEnable;
   be_uint8_t CreateProcessType;
   be_uint8_t CurrentProcessType;
   KApcState ApcState;
   be_uint8_t ApcQueueable;
   KSpinLock ApcQueueLock;
   be_uint32_t ContextSwitches;
   KListEntry ReadyListEntry;
   be_uint32_t MsrEnableMask;
   be_ptr32_t<be_int32_t> WaitStatus;
   KIrql WaitIrql;
   KProcessorMode WaitMode;
   be_uint8_t WaitNext;
   be_uint8_t WaitReason;
   be_ptr32_t<KWaitBlock> WaitBlockList;
   UNKNOWN(0x4);
   be_uint32_t KernelApcDisable;
   be_uint32_t Quantum;
   be_int8_t Saturation;
   be_uint8_t BasePriority;
   be_uint8_t PriorityDecrement;
   be_uint8_t DisableBoost;
   be_int8_t SuspendCount;
   be_uint8_t Preempted;
   be_uint8_t HasTerminated;
   be_uint8_t CurrentProcessor;
   ptr32_t<KPrcb> CurrentPrcb;
   ptr32_t<KPrcb> AffinityPrcb;
   be_uint8_t IdlePriorityClass;
   be_uint8_t NormalPriorityClass;
   be_uint8_t TimeCriticalPriorityClass;
   be_uint8_t HasAsyncTerminated;
   KSpinLock ActiveTimerListLock;
   be_ptr32_t<void> StackAllocatedBase;
   KApc SuspendApc;
   KSemaphore SuspendSemaphore;
   KListEntry ThreadListEntry;
   be_ptr32_t<KQueue> Queue;
   KListEntry QueueListEntry;
   be_ptr32_t<void> UserModeDispatcher;
   be_ptr32_t<struct KTrapFrame> UserModeTrapFrame; // Unknown struct KTRAP_FRAME
   PADDING(0x4);
   be_uint64_t CreateTime;
   be_uint64_t ExitTime;
   be_uint32_t ExitStatus;
   KListEntry ActiveTimerListHead;
   be_uint32_t ThreadId;
   be_ptr32_t<void> StartAddress;
   KListEntry IrpList;
   be_ptr32_t<void> DebugMonitorData;
   be_uint32_t LastWin32ErrorCode;
   be_ptr32_t<void> Win32CurrentFiber;
   UNKNOWN(0x4);
   be_uint32_t CreateOptions;
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

// KPROCESS
struct KProcess
{
   KSpinLock ThreadListLock;
   KListEntry ThreadListHead;
   be_int32_t ThreadQuantum;
   be_uint32_t DirectoryTableBase;
   be_uint32_t ThreadCount;
   be_uint8_t IdlePriorityClass;
   be_uint8_t NormalPriorityClass;
   be_uint8_t TimeCriticalPriorityClass;
   be_uint8_t DisableQuantum;
   be_uint32_t DefaultKernelStackSize;
   be_ptr32_t<void> TlsStaticDataImage;
   be_uint32_t SizeOfTlsStaticData;
   be_uint32_t SizeOfTlsStaticDataImage;
   be_uint16_t SizeOfTlsSlots;
   be_uint8_t Terminating;
   be_uint8_t ProcessType;
   be_uint32_t TlsSlotBitmap[8];
   KSpinLock FileObjectListLock;
   KListEntry FileObjectListHead;
   be_ptr32_t<void> Win32DefaultHeapHandle;
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

// KPRCB
struct KPrcb
{
   ptr32_t<KThread> CurrentThread;
   ptr32_t<KThread> NextThread;
   ptr32_t<KThread> IdleThread;
   be_uint8_t Number;
   PADDING(0x3);
   be_uint32_t SetMember;
   be_uint32_t DpcTime;
   be_uint32_t InterruptTime;
   be_uint32_t InterruptCount;
   be_uint32_t IpiFrozen;
   be_ptr32_t<void> CurrentPacket[3];
   be_uint32_t TargetSet;
   be_ptr32_t<void> WorkerRoutine;
   be_ptr32_t<KPrcb> SignalDone;
   be_uint32_t RequestSummary;
   be_uint32_t DpcInterruptRequested;
   KSpinLock DpcLock;
   KListEntry DpcListHead;
   be_uint32_t DpcRoutineActive;
   KSpinLock ReadyListLock;
   be_uint32_t IdleThreadActive;
   be_ptr32_t<void> DeferredReadyListHead;
   be_uint32_t ReadySummary;
   be_uint32_t ReadSummaryMask;
   KListEntry DispatcherReadyListHead[32];
   KDpc ThreadReaperDpc;
   KListEntry ThreadReaperListHead;
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

// KPCR, per thread structure living on r13
struct KPcr
{
   be_ptr32_t<void> TlsData;
   be_uint32_t MsrEnableMask;
   be_uint16_t SoftwareInterrupt;
   be_uint16_t DpcFpuVpuStateSaved;
   be_uint8_t DpcCurrentProcessType;
   be_uint8_t QuantumEnd;
   be_uint8_t TimerRequest;
   be_uint8_t HvCr0Save;
   be_ptr32_t<KThread> FpuOwnerThread;
   be_ptr32_t<KThread> VpuOwnerThread;
   be_uint8_t CurrentIrql;
   be_uint8_t BackgroundSchedulingActive;
   be_uint16_t StartStopBackgroundScheduling;
   be_uint32_t TimerHand;
   be_uint64_t LrIarSave;
   be_uint64_t CrMsrSave;
   be_uint64_t Gpr13Save;
   be_uint64_t HvGpr1Save;
   be_uint64_t HvGpr3Save;
   be_uint64_t HvGpr4Save;
   be_uint64_t HvGpr5Save;
   be_uint32_t UserModeControl;
   be_ptr32_t<void> PanicStack;
   be_uint32_t DarSave;
   be_uint32_t DsisrSave;
   be_ptr32_t<void> DbgLastDpcRoutine;
   be_uint32_t OnInterruptStack;
   be_ptr32_t<void> StackBase;
   be_ptr32_t<void> StackLimit;
   be_ptr32_t<void> InterruptStackBase;
   be_ptr32_t<void> InterruptStackLimit;
   be_ptr32_t<void> InterruptRoutine[32];
   KPrcb PrcbData;
   be_ptr32_t<KPrcb> Prcb;
   UNKNOWN(0x4);
   be_ptr32_t<be_uint32_t> PixCurrent;
   be_ptr32_t<be_uint32_t> PixLimit;
   be_ptr32_t<be_uint32_t> ProfilerCurrent;
   be_ptr32_t<be_uint32_t> ProfilerLimit;
   be_uint32_t ProfilerFlags;
   PADDING(0x4);
   be_uint64_t Contention;
   be_ptr32_t<be_uint32_t> MonitorProfileData;
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

// ANSI_STRING
struct KAnsiString
{
   be_uint16_t Length;
   be_uint16_t MaximumLength;
   be_ptr32_t<be_int8_t> Buffer;
};

CHECK_OFFSET(KAnsiString, 0x0, Length);
CHECK_OFFSET(KAnsiString, 0x2, MaximumLength);
CHECK_OFFSET(KAnsiString, 0x4, Buffer);
CHECK_SIZE(KAnsiString, 0x8);

// UNICODE_STRING
struct KUnicodeString
{
   be_uint16_t Length;
   be_uint16_t MaximumLength;
   be_ptr32_t<be_int16_t> Buffer;
};

CHECK_OFFSET(KUnicodeString, 0x0, Length);
CHECK_OFFSET(KUnicodeString, 0x2, MaximumLength);
CHECK_OFFSET(KUnicodeString, 0x4, Buffer);
CHECK_SIZE(KUnicodeString, 0x8);

// LDR_DATA_TABLE_ENTRY
struct KLdrDataTableEntry
{
   KListEntry InLoadOrderLinks;
   KListEntry InClosureOrderLinks;
   KListEntry InInitializationOrderLinks;
   be_ptr32_t<void> NtHeadersBase;
   be_ptr32_t<void> ImageBase;
   be_uint32_t SizeOfNtImage;
   KUnicodeString FullDllName;
   KUnicodeString BaseDllName;
   be_uint32_t Flags;
   be_uint32_t SizeOfFullImage;
   be_ptr32_t<void> EntryPoint;
   be_uint16_t LoadCount;
   be_uint16_t ModuleIndex;
   be_ptr32_t<void> DllBaseOriginal;
   be_uint32_t CheckSum;
   be_uint32_t ModuleLoadFlags;
   be_uint32_t TimeDateStamp;
   be_ptr32_t<void> LoadedImports;
   be_ptr32_t<void> XexHeaderBase;

   struct
   {
      be_ptr32_t<KLdrDataTableEntry> ClosureRoot;
      be_ptr32_t<KLdrDataTableEntry> TraversalParent;
   } asEntry;
};

CHECK_OFFSET(KLdrDataTableEntry, 0x0, InLoadOrderLinks);
CHECK_OFFSET(KLdrDataTableEntry, 0x8, InClosureOrderLinks);
CHECK_OFFSET(KLdrDataTableEntry, 0x10, InInitializationOrderLinks);
CHECK_OFFSET(KLdrDataTableEntry, 0x18, NtHeadersBase);
CHECK_OFFSET(KLdrDataTableEntry, 0x1c, ImageBase);
CHECK_OFFSET(KLdrDataTableEntry, 0x20, SizeOfNtImage);
CHECK_OFFSET(KLdrDataTableEntry, 0x24, FullDllName);
CHECK_OFFSET(KLdrDataTableEntry, 0x2c, BaseDllName);
CHECK_OFFSET(KLdrDataTableEntry, 0x34, Flags);
CHECK_OFFSET(KLdrDataTableEntry, 0x38, SizeOfFullImage);
CHECK_OFFSET(KLdrDataTableEntry, 0x3c, EntryPoint);
CHECK_OFFSET(KLdrDataTableEntry, 0x40, LoadCount);
CHECK_OFFSET(KLdrDataTableEntry, 0x42, ModuleIndex);
CHECK_OFFSET(KLdrDataTableEntry, 0x44, DllBaseOriginal);
CHECK_OFFSET(KLdrDataTableEntry, 0x48, CheckSum);
CHECK_OFFSET(KLdrDataTableEntry, 0x4c, ModuleLoadFlags);
CHECK_OFFSET(KLdrDataTableEntry, 0x50, TimeDateStamp);
CHECK_OFFSET(KLdrDataTableEntry, 0x54, LoadedImports);
CHECK_OFFSET(KLdrDataTableEntry, 0x58, XexHeaderBase);
CHECK_OFFSET(KLdrDataTableEntry, 0x5c, asEntry.ClosureRoot);
CHECK_OFFSET(KLdrDataTableEntry, 0x60, asEntry.TraversalParent);
CHECK_SIZE(KLdrDataTableEntry, 0x64);

using KNTSTATUS = int32_t;

#define KSTATUS_SUCCESS   ((KNTSTATUS)0x00000000L)
#define KSTATUS_NO_MEMORY ((KNTSTATUS)0xC0000017L)

#pragma pack(pop)

#endif // XBOXKRNL_NTSTRUCTS_H
