#ifndef NETDLL_XNET_H
#define NETDLL_XNET_H

#include <kernel.h>

struct Xin_addr
{
   union
   {
      struct
      {
         XBYTE s_b1, s_b2, s_b3, s_b4;
      } S_un_b;

      struct
      {
         XWORD s_w1, s_w2;
      } S_un_w;

      XDWORD S_addr;
   } S_un;
};

using XIN_ADDR = Xin_addr;
using XLPIN_ADDR = XLP<XIN_ADDR>;

/* XNet */
XBXKRNL XINT XNetConnect(XNCALLER_TYPE xnc,
                         const XIN_ADDR ina);

/*
51 : NetDll_XNetStartup
52 : NetDll_XNetCleanup
53 : NetDll_XNetRandom
54 : NetDll_XNetCreateKey
55 : NetDll_XNetRegisterKey
56 : NetDll_XNetUnregisterKey
57 : NetDll_XNetXnAddrToInAddr
58 : NetDll_XNetServerToInAddr
59 : NetDll_XNetTsAddrToInAddr
60 : NetDll_XNetInAddrToXnAddr
61 : NetDll_XNetInAddrToServer
62 : NetDll_XNetInAddrToString
63 : NetDll_XNetUnregisterInAddr
64 : NetDll_XNetXnAddrToMachineId
65 : NetDll_XNetConnect
66 : NetDll_XNetGetConnectStatus
67 : NetDll_XNetDnsLookup
68 : NetDll_XNetDnsRelease
69 : NetDll_XNetQosListen
70 : NetDll_XNetQosLookup
71 : NetDll_XNetQosServiceLookup
72 : NetDll_XNetQosRelease
73 : NetDll_XNetGetTitleXnAddr
74 : NetDll_XNetGetDebugXnAddr
75 : NetDll_XNetGetEthernetLinkStatus
76 : NetDll_XNetGetBroadcastVersionStatus
77 : NetDll_XNetQosGetListenStats
78 : NetDll_XNetGetOpt
79 : NetDll_XNetSetOpt
80 : NetDll_XNetStartupEx
81 : NetDll_XNetReplaceKey
82 : NetDll_XNetGetXnAddrPlatform
83 : NetDll_XNetGetSystemLinkPort
84 : NetDll_XNetSetSystemLinkPort
85 : NetDll_XNetDnsReverseLookup
86 : NetDll_XNetDnsReverseRelease
*/

#endif // ifndef NETDLL_XNET_H
