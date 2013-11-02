#ifndef XEX_H
#define XEX_H

#include "kernel.h"
#include <WinSock2.h>

using XNCALLER_TYPE = uint32_t;

/* WinSock */
XBXKRNL int
NetDll_WSAStartup(XNCALLER_TYPE xnc,
                  WORD wVersionRequested,
                  LPWSADATA lpWSAData);

XBXKRNL int
NetDll_WSACleanup(XNCALLER_TYPE xnc);

XBXKRNL SOCKET
NetDll_socket(XNCALLER_TYPE xnc,
              int af,
              int type,
              int protocol);

XBXKRNL int
NetDll_closesocket(XNCALLER_TYPE xnc,
                   SOCKET s);

XBXKRNL int
NetDll_shutdown(XNCALLER_TYPE xnc,
                SOCKET s,
                int how);

XBXKRNL int
NetDll_ioctlsocket(XNCALLER_TYPE xnc,
                   SOCKET s,
                   long cmd,
                   u_long FAR *argp);

XBXKRNL int 
NetDll_setsockopt(XNCALLER_TYPE xnc,
                  SOCKET s,
                  int level,
                  int optname,
                  char FAR *optval,
                  int optlen);

XBXKRNL int
NetDll_getsockopt(XNCALLER_TYPE xnc,
                  SOCKET s,
                  int level,
                  int optname,
                  char FAR *optval,
                  int FAR *optlen);

XBXKRNL int
NetDll_getsockname(XNCALLER_TYPE xnc,
                   SOCKET s,
                   struct sockaddr FAR *name,
                   int FAR *namelen);

XBXKRNL int
NetDll_getpeername(XNCALLER_TYPE xnc,
                   SOCKET s,
                   struct sockaddr FAR *name,
                   int FAR *namelen);

XBXKRNL int
NetDll_bind(XNCALLER_TYPE xnc,
            SOCKET s,
            const struct sockaddr FAR *name,
            int namelen);

XBXKRNL int
NetDll_connect(XNCALLER_TYPE xnc,
               SOCKET s,
               const struct sockaddr FAR *name,
               int namelen);

XBXKRNL int
NetDll_listen(XNCALLER_TYPE xnc,
              SOCKET s,
              int backlog);

XBXKRNL SOCKET
NetDll_accept(XNCALLER_TYPE xnc,
              SOCKET s,
              struct sockaddr FAR *addr,
              int FAR *addrlen);

XBXKRNL int
NetDll_select(XNCALLER_TYPE xnc,
              int nfds,
              fd_set FAR *readfds,
              fd_set FAR *writefds,
              fd_set FAR *exceptfds,
              const struct timeval FAR *timeout);

XBXKRNL BOOL
NetDll_WSAGetOverlappedResult(XNCALLER_TYPE xnc,
                              SOCKET s,
                              LPWSAOVERLAPPED lpOverlapped,
                              LPDWORD lpcbTransfer,
                              BOOL fWait,
                              LPDWORD lpdwFlags);

XBXKRNL int
NetDll_WSACancelOverlappedIO(XNCALLER_TYPE xnc,
                             SOCKET s);

XBXKRNL int
NetDll_recv(XNCALLER_TYPE xnc,
            SOCKET s,
            char FAR *buf,
            int len,
            int flags);

XBXKRNL int
NetDll_WSARecv(XNCALLER_TYPE xnc,
               SOCKET s,
               LPWSABUF lpBuffers,
               DWORD dwBufferCount,
               LPDWORD lpNumberOfBytesRecvd,
               LPDWORD lpFlags,
               LPWSAOVERLAPPED lpOverlapped,
               LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

XBXKRNL int
NetDll_recvfrom(XNCALLER_TYPE xnc,
                SOCKET s,
                char FAR *buf,
                int len,
                int flags,
                struct sockaddr FAR *from,
                int FAR *fromlen);

XBXKRNL int
NetDll_WSARecvFrom(XNCALLER_TYPE xnc,
                   SOCKET s,
                   LPWSABUF lpBuffers,
                   DWORD dwBufferCount,
                   LPDWORD lpNumberOfBytesRecvd,
                   LPDWORD lpFlags,
                   struct sockaddr FAR *lpFrom,
                   LPINT lpFromlen,
                   LPWSAOVERLAPPED lpOverlapped,
                   LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

XBXKRNL int
NetDll_send(XNCALLER_TYPE xnc,
            SOCKET s,
            const char FAR *buf,
            int len,
            int flags);

XBXKRNL int
NetDll_WSASend(XNCALLER_TYPE xnc,
               SOCKET s,
               LPWSABUF lpBuffers,
               DWORD dwBufferCount,
               LPDWORD lpNumberOfBytesSent,
               DWORD dwFlags,
               LPWSAOVERLAPPED lpOverlapped,
               LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

XBXKRNL int
NetDll_sendto(XNCALLER_TYPE xnc,
              SOCKET s,
              const char FAR *buf,
              int len,
              int flags,
              const struct sockaddr FAR *to,
              int tolen);

XBXKRNL int
NetDll_WSASendTo(XNCALLER_TYPE xnc,
                 SOCKET s,
                 LPWSABUF lpBuffers,
                 DWORD dwBufferCount,
                 LPDWORD lpNumberOfBytesSent,
                 DWORD dwFlags,
                 const struct sockaddr FAR *lpTo,
                 int iToLen,
                 LPWSAOVERLAPPED lpOverlapped,
                 LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

XBXKRNL long
NetDll_inet_addr(XNCALLER_TYPE xnc,
                 const char FAR *cp);

XBXKRNL int
NetDll_WSAGetLastError(XNCALLER_TYPE xnc);

XBXKRNL void
NetDll_WSASetLastError(XNCALLER_TYPE xnc,
                       int iError);

XBXKRNL WSAEVENT
NetDll_WSACreateEvent(XNCALLER_TYPE xnc);

XBXKRNL BOOL
NetDll_WSACloseEvent(XNCALLER_TYPE xnc,
                     WSAEVENT hEvent);

XBXKRNL BOOL
NetDll_WSASetEvent(XNCALLER_TYPE xnc,
                   WSAEVENT hEvent);

XBXKRNL BOOL
NetDll_WSAResetEvent(XNCALLER_TYPE xnc,
                     WSAEVENT hEvent);

XBXKRNL DWORD
NetDll_WSAWaitForMultipleEvents(XNCALLER_TYPE xnc,
                                DWORD cEvents,
                                const WSAEVENT FAR *lphEvents,
                                BOOL fWaitAll,
                                DWORD dwTimeout,
                                BOOL fAlertable);

XBXKRNL INT
NetDll___WSAFDIsSet(XNCALLER_TYPE xnc,
                    SOCKET fd,
                    fd_set *set);

XBXKRNL int
NetDll_WSAEventSelect(XNCALLER_TYPE xnc,
                      SOCKET s,
                      WSAEVENT hEventObject,
                      long lNetworkEvents);

/*
36 : NetDll_WSAStartupEx 
*/


/* XNet */
XBXKRNL INT XNetConnect(XNCALLER_TYPE xnc,
                        const IN_ADDR ina);

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

/* Xnp */
#endif
