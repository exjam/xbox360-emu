#ifndef NETDLL_WINSOCK_H
#define NETDLL_WINSOCK_H

#include <kernel.h>

using XSOCKET         = XLPVOID;
using XWSAEVENT       = XHANDLE;
using XLPWSAEVENT     = XLP<XWSAEVENT>;
using XADDRESS_FAMILY = XWORD;

struct XWSAData
{
   XWORD wVersion;
   XWORD wHighVersion;
   XCHAR szDescription[257];
   XCHAR szSystemStatus[257];
   XWORD iMaxSockets;
   XWORD iMaxUdpDg;
   XLPCHAR lpVendorInfo;
};

struct XWSABuf
{
   XDWORD len;
   XLPCHAR buf;
};

struct XOVERLAPPED
{
   XLPDWORD Internal;
   XLPDWORD InternalHigh;

   union
   {
      struct
      {
         XDWORD Offset;
         XDWORD OffsetHigh;
      };

      XPVOID Pointer;
   };

   XHANDLE hEvent;
};

struct Xsockaddr
{
   XADDRESS_FAMILY sa_family;
   XCHAR sa_data[14];
};

struct Xfd_set
{
   XDWORD fd_count;
   XSOCKET fd_array[64];
};

struct Xtimeval
{
   XINT tv_sec;
   XINT tv_usec;
};

using XWSADATA    = XWSAData;
using XLPWSADATA  = XLP<XWSADATA>;

using XWSABUF     = XWSABuf;
using XLPWSABUF   = XLP<XWSABUF>;

using XSOCKADDR   = Xsockaddr;
using XLPSOCKADDR = XLP<XSOCKADDR>;

using XFD_SET     = Xfd_set;
using XLPFD_SET   = XLP<XFD_SET>;

using XTIMEVAL    = Xtimeval;
using XLPTIMEVAL  = XLP<XTIMEVAL>;

using XWSAOVERLAPPED   = XOVERLAPPED;
using XLPWSAOVERLAPPED = XLP<XOVERLAPPED>;

/* WinSock */
XBXKRNL XINT
NetDll_WSAStartup(XNCALLER_TYPE xnc,
                  XWORD wVersionRequested,
                  XLPWSADATA lpWSAData);

XBXKRNL XINT
NetDll_WSACleanup(XNCALLER_TYPE xnc);

XBXKRNL XPVOID
NetDll_socket(XNCALLER_TYPE xnc,
              XINT af,
              XINT type,
              XINT protocol);

XBXKRNL XINT
NetDll_closesocket(XNCALLER_TYPE xnc,
                   XSOCKET s);

XBXKRNL XINT
NetDll_shutdown(XNCALLER_TYPE xnc,
                XSOCKET s,
                XINT how);

XBXKRNL XINT
NetDll_ioctlsocket(XNCALLER_TYPE xnc,
                   XSOCKET s,
                   XINT cmd,
                   XLPDWORD argp);

XBXKRNL XINT
NetDll_setsockopt(XNCALLER_TYPE xnc,
                  XSOCKET s,
                  XINT level,
                  XINT optname,
                  XLPCHAR optval,
                  XINT optlen);

XBXKRNL XINT
NetDll_getsockopt(XNCALLER_TYPE xnc,
                  XSOCKET s,
                  XINT level,
                  XINT optname,
                  XLPCHAR optval,
                  XLPINT optlen);

XBXKRNL XINT
NetDll_getsockname(XNCALLER_TYPE xnc,
                   XSOCKET s,
                   XLPSOCKADDR name,
                   XLPINT namelen);

XBXKRNL XINT
NetDll_getpeername(XNCALLER_TYPE xnc,
                   XSOCKET s,
                   XLPSOCKADDR name,
                   XLPINT namelen);

XBXKRNL XINT
NetDll_bind(XNCALLER_TYPE xnc,
            XSOCKET s,
            const XLPSOCKADDR name,
            XINT namelen);

XBXKRNL XINT
NetDll_connect(XNCALLER_TYPE xnc,
               XSOCKET s,
               const XLPSOCKADDR name,
               XINT namelen);

XBXKRNL XINT
NetDll_listen(XNCALLER_TYPE xnc,
              XSOCKET s,
              XINT backlog);

XBXKRNL XPVOID
NetDll_accept(XNCALLER_TYPE xnc,
              XSOCKET s,
              XLPSOCKADDR addr,
              XLPINT addrlen);

XBXKRNL XINT
NetDll_select(XNCALLER_TYPE xnc,
              XINT nfds,
              XLPFD_SET readfds,
              XLPFD_SET writefds,
              XLPFD_SET exceptfds,
              XLPTIMEVAL timeout);

XBXKRNL XBOOL
NetDll_WSAGetOverlappedResult(XNCALLER_TYPE xnc,
                              XSOCKET s,
                              XLPWSAOVERLAPPED lpOverlapped,
                              XLPDWORD lpcbTransfer,
                              XBOOL fWait,
                              XLPDWORD lpdwFlags);

XBXKRNL XINT
NetDll_WSACancelOverlappedIO(XNCALLER_TYPE xnc,
                             XSOCKET s);

XBXKRNL XINT
NetDll_recv(XNCALLER_TYPE xnc,
            XSOCKET s,
            XLPCHAR buf,
            XINT len,
            XINT flags);

XBXKRNL XINT
NetDll_WSARecv(XNCALLER_TYPE xnc,
               XSOCKET s,
               XLPWSABUF lpBuffers,
               XDWORD dwBufferCount,
               XLPDWORD lpNumberOfBytesRecvd,
               XLPDWORD lpFlags,
               XLPWSAOVERLAPPED lpOverlapped,
               XLPVOID lpCompletionRoutine);

XBXKRNL XINT
NetDll_recvfrom(XNCALLER_TYPE xnc,
                XSOCKET s,
                XLPCHAR buf,
                XINT len,
                XINT flags,
                XLPSOCKADDR from,
                XLPINT fromlen);

XBXKRNL XINT
NetDll_WSARecvFrom(XNCALLER_TYPE xnc,
                   XSOCKET s,
                   XLPWSABUF lpBuffers,
                   XDWORD dwBufferCount,
                   XLPDWORD lpNumberOfBytesRecvd,
                   XLPDWORD lpFlags,
                   XLPSOCKADDR lpFrom,
                   XLPINT lpFromlen,
                   XLPWSAOVERLAPPED lpOverlapped,
                   XLPVOID lpCompletionRoutine);

XBXKRNL XINT
NetDll_send(XNCALLER_TYPE xnc,
            XSOCKET s,
            const XLPCHAR buf,
            XINT len,
            XINT flags);

XBXKRNL XINT
NetDll_WSASend(XNCALLER_TYPE xnc,
               XSOCKET s,
               XLPWSABUF lpBuffers,
               XDWORD dwBufferCount,
               XLPDWORD lpNumberOfBytesSent,
               XDWORD dwFlags,
               XLPWSAOVERLAPPED lpOverlapped,
               XLPVOID lpCompletionRoutine);

XBXKRNL XINT
NetDll_sendto(XNCALLER_TYPE xnc,
              XSOCKET s,
              const XLPCHAR buf,
              XINT len,
              XINT flags,
              const XLPSOCKADDR to,
              XINT tolen);

XBXKRNL XINT
NetDll_WSASendTo(XNCALLER_TYPE xnc,
                 XSOCKET s,
                 XLPWSABUF lpBuffers,
                 XDWORD dwBufferCount,
                 XLPDWORD lpNumberOfBytesSent,
                 XDWORD dwFlags,
                 const XLPSOCKADDR lpTo,
                 XINT iToLen,
                 XLPWSAOVERLAPPED lpOverlapped,
                 XLPVOID lpCompletionRoutine);

XBXKRNL XINT
NetDll_inet_addr(XNCALLER_TYPE xnc,
                 XLPCHAR cp);

XBXKRNL XINT
NetDll_WSAGetLastError(XNCALLER_TYPE xnc);

XBXKRNL XVOID
NetDll_WSASetLastError(XNCALLER_TYPE xnc,
                       XINT iError);

XBXKRNL XPVOID
NetDll_WSACreateEvent(XNCALLER_TYPE xnc);

XBXKRNL XBOOL
NetDll_WSACloseEvent(XNCALLER_TYPE xnc,
                     XWSAEVENT hEvent);

XBXKRNL XBOOL
NetDll_WSASetEvent(XNCALLER_TYPE xnc,
                   XWSAEVENT hEvent);

XBXKRNL XBOOL
NetDll_WSAResetEvent(XNCALLER_TYPE xnc,
                     XWSAEVENT hEvent);

XBXKRNL XDWORD
NetDll_WSAWaitForMultipleEvents(XNCALLER_TYPE xnc,
                                XDWORD cEvents,
                                const XLPWSAEVENT lphEvents,
                                XBOOL fWaitAll,
                                XDWORD dwTimeout,
                                XBOOL fAlertable);

XBXKRNL XINT
NetDll___WSAFDIsSet(XNCALLER_TYPE xnc,
                    XSOCKET fd,
                    XLPFD_SET set);

XBXKRNL XINT
NetDll_WSAEventSelect(XNCALLER_TYPE xnc,
                      XSOCKET s,
                      XWSAEVENT hEventObject,
                      XINT lNetworkEvents);

/*
36 : NetDll_WSAStartupEx 
*/

#endif
