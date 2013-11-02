#ifndef KE_H
#define KE_H

#include "kernel.h"

/*
KeAcquireSpinLockAtRaisedIrql @ 77
KeAlertResumeThread @ 78
KeAlertThread @ 79
KeBlowFuses @ 80
KeBoostPriorityThread @ 81
KeBugCheck @ 82
KeBugCheckEx @ 83
KeCancelTimer @ 84
KeConnectInterrupt @ 85
KeContextFromKframes @ 86
KeContextToKframes @ 87
KeCreateUserMode @ 88
KeDebugMonitorData @ 89
KeDelayExecutionThread @ 90
KeDeleteUserMode @ 91
KeDisconnectInterrupt @ 92
KeEnableFpuExceptions @ 93
KeEnablePPUPerformanceMonitor @ 94
KeEnterCriticalRegion @ 95
KeEnterUserMode @ 96
KeFlushCacheRange @ 97
KeFlushCurrentEntireTb @ 98
KeFlushEntireTb @ 99
KeFlushUserModeCurrentTb @ 100
KeFlushUserModeTb @ 101
*/

XBXKRNL uint8_t KeGetCurrentProcessType();

/*
KeGetPMWRegister @ 103
KeGetPRVRegister @ 104
KeGetSocRegister @ 105
KeGetSpecialPurposeRegister @ 106
KeLockL2 @ 107
KeUnlockL2 @ 108
KeInitializeApc @ 109
KeInitializeDeviceQueue @ 110
KeInitializeDpc @ 111
KeInitializeEvent @ 112
KeInitializeInterrupt @ 113
KeInitializeMutant @ 114
KeInitializeQueue @ 115
KeInitializeSemaphore @ 116
KeInitializeTimerEx @ 117
KeInsertByKeyDeviceQueue @ 118
KeInsertDeviceQueue @ 119
KeInsertHeadQueue @ 120
KeInsertQueue @ 121
KeInsertQueueApc @ 122
KeInsertQueueDpc @ 123
KeIpiGenericCall @ 124
KeLeaveCriticalRegion @ 125
KeLeaveUserMode @ 126
KePulseEvent @ 127
KeQueryBackgroundProcessors @ 128
KeQueryBasePriorityThread @ 129
KeQueryInterruptTime @ 130
KeQueryPerformanceFrequency @ 131
KeQuerySystemTime @ 132
KeRaiseIrqlToDpcLevel @ 133
KeRegisterDriverNotification @ 134
KeReleaseMutant @ 135
KeReleaseSemaphore @ 136
KeReleaseSpinLockFromRaisedIrql @ 137
KeRemoveByKeyDeviceQueue @ 138
KeRemoveDeviceQueue @ 139
KeRemoveEntryDeviceQueue @ 140
KeRemoveQueue @ 141
KeRemoveQueueDpc @ 142
KeResetEvent @ 143
KeRestoreFloatingPointState @ 144
KeRestoreVectorUnitState @ 145
KeResumeThread @ 146
KeRetireDpcList @ 147
KeRundownQueue @ 148
KeSaveFloatingPointState @ 149
KeSaveVectorUnitState @ 150
KeSetAffinityThread @ 151
KeSetBackgroundProcessors @ 152
KeSetBasePriorityThread @ 153
KeSetCurrentProcessType @ 154
KeSetCurrentStackPointers @ 155
KeSetDisableBoostThread @ 156
KeSetEvent @ 157
KeSetEventBoostPriority @ 158
KeSetPMWRegister @ 159
KeSetPowerMode @ 160
KeSetPRVRegister @ 161
KeSetPriorityClassThread @ 162
KeSetPriorityThread @ 163
KeSetSocRegister @ 164
KeSetSpecialPurposeRegister @ 165
KeSetTimer @ 166
KeSetTimerEx @ 167
KeStallExecutionProcessor @ 168
KeSuspendThread @ 169
KeSweepDcacheRange @ 170
KeSweepIcacheRange @ 171
KeTestAlertThread @ 172
KeTimeStampBundle @ 173
KeTryToAcquireSpinLockAtRaisedIrql @ 174
KeWaitForMultipleObjects @ 175
KeWaitForSingleObject @ 176
*/

#endif
