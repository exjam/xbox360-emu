#include "netdll.h"
#include "util/bits.h"

#pragma comment(lib, "ws2_32.lib")

XBXKRNL int
NetDll_WSAStartup(XNCALLER_TYPE xnc,
                  WORD wVersionRequested,
                  LPWSADATA lpWSAData)
{
   auto swapValues = [&lpWSAData] {
      lpWSAData->wVersion     = bits::swap(lpWSAData->wVersion);
      lpWSAData->wHighVersion = bits::swap(lpWSAData->wHighVersion);
      lpWSAData->iMaxSockets  = bits::swap(lpWSAData->iMaxSockets);
      lpWSAData->iMaxUdpDg    = bits::swap(lpWSAData->iMaxUdpDg);
      lpWSAData->lpVendorInfo = bits::swap(lpWSAData->lpVendorInfo);
   };

   swapValues();
   auto result = WSAStartup(wVersionRequested, lpWSAData);
   swapValues();

   return result;
}

XBXKRNL int
NetDll_WSACleanup(XNCALLER_TYPE xnc)
{
   return WSACleanup();
}

XBXKRNL SOCKET
NetDll_socket(XNCALLER_TYPE xnc,
              int af,
              int type,
              int protocol)
{
   return socket(af, type, protocol);
}

XBXKRNL int
NetDll_closesocket(XNCALLER_TYPE xnc,
                   SOCKET s)
{
   return closesocket(s);
}

XBXKRNL int
NetDll_shutdown(XNCALLER_TYPE xnc,
                SOCKET s,
                int how)
{
   return shutdown(s, how);
}

XBXKRNL int
NetDll_ioctlsocket(XNCALLER_TYPE xnc,
                   SOCKET s,
                   long cmd,
                   u_long FAR *argp)
{
   int result;

   if (argp) {
      *argp = bits::swap(*argp);
   }
   
   result = ioctlsocket(s, cmd, argp);

   if (argp) {
      *argp = bits::swap(*argp);
   }

   return result;
}

XBXKRNL int
NetDll_setsockopt(XNCALLER_TYPE xnc,
                  SOCKET s,
                  int level,
                  int optname,
                  char FAR *optval,
                  int optlen)
{
   auto swapValues = [&optname, &optval] {
      if (optval) {
         switch (optname) {
         case SO_GROUP_PRIORITY:
         case SO_RCVBUF:
         case SO_SNDBUF:
            *reinterpret_cast<int*>(optval) = bits::swap(*reinterpret_cast<int*>(optval));
            break;

         case SO_RCVTIMEO:
         case SO_SNDTIMEO:
            *reinterpret_cast<DWORD*>(optval) = bits::swap(*reinterpret_cast<DWORD*>(optval));
            break;

         case SO_LINGER:
            linger *value = reinterpret_cast<linger*>(optval);
            value->l_linger = bits::swap(value->l_linger);
            value->l_onoff = bits::swap(value->l_onoff);
            break;
         }
      }
   };

   swapValues();
   auto result = setsockopt(s, level, optname, optval, optlen);
   swapValues();

   return result;
}

XBXKRNL int
NetDll_getsockopt(XNCALLER_TYPE xnc,
                  SOCKET s,
                  int level,
                  int optname,
                  char FAR *optval,
                  int FAR *optlen)
{
   XBXKRNL_UNIMPLEMENTED;
   return 0;
}

XBXKRNL int
NetDll_getsockname(XNCALLER_TYPE xnc,
                   SOCKET s,
                   struct sockaddr FAR *name,
                   int FAR *namelen)
{
   XBXKRNL_UNIMPLEMENTED;
   return 0;
}

XBXKRNL int
NetDll_getpeername(XNCALLER_TYPE xnc,
                   SOCKET s,
                   struct sockaddr FAR *name,
                   int FAR *namelen)
{
   XBXKRNL_UNIMPLEMENTED;
   return 0;
}

XBXKRNL int
NetDll_bind(XNCALLER_TYPE xnc,
            SOCKET s,
            const struct sockaddr FAR *name,
            int namelen)
{
   XBXKRNL_UNIMPLEMENTED;
   return 0;
}

XBXKRNL int
NetDll_connect(XNCALLER_TYPE xnc,
               SOCKET s,
               const struct sockaddr FAR *name,
               int namelen)
{
   XBXKRNL_UNIMPLEMENTED;
   return 0;
}

XBXKRNL int
NetDll_listen(XNCALLER_TYPE xnc,
              SOCKET s,
              int backlog)
{
   XBXKRNL_UNIMPLEMENTED;
   return 0;
}

XBXKRNL SOCKET
NetDll_accept(XNCALLER_TYPE xnc,
              SOCKET s,
              struct sockaddr FAR *addr,
              int FAR *addrlen)
{
   XBXKRNL_UNIMPLEMENTED;
   return 0;
}

XBXKRNL int
NetDll_select(XNCALLER_TYPE xnc,
              int nfds,
              fd_set FAR *readfds,
              fd_set FAR *writefds,
              fd_set FAR *exceptfds,
              const struct timeval FAR *timeout)
{
   XBXKRNL_UNIMPLEMENTED;
   return 0;
}

XBXKRNL BOOL
NetDll_WSAGetOverlappedResult(XNCALLER_TYPE xnc,
                              SOCKET s,
                              LPWSAOVERLAPPED lpOverlapped,
                              LPDWORD lpcbTransfer,
                              BOOL fWait,
                              LPDWORD lpdwFlags)
{
   XBXKRNL_UNIMPLEMENTED;
   return 0;
}

XBXKRNL int
NetDll_WSACancelOverlappedIO(XNCALLER_TYPE xnc,
                             SOCKET s)
{
   XBXKRNL_UNIMPLEMENTED;
   return 0;
}

XBXKRNL int
NetDll_recv(XNCALLER_TYPE xnc,
            SOCKET s,
            char FAR *buf,
            int len,
            int flags)
{
   XBXKRNL_UNIMPLEMENTED;
   return 0;
}

XBXKRNL int
NetDll_WSARecv(XNCALLER_TYPE xnc,
               SOCKET s,
               LPWSABUF lpBuffers,
               DWORD dwBufferCount,
               LPDWORD lpNumberOfBytesRecvd,
               LPDWORD lpFlags,
               LPWSAOVERLAPPED lpOverlapped,
               LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
   XBXKRNL_UNIMPLEMENTED;
   return 0;
}

XBXKRNL int
NetDll_recvfrom(XNCALLER_TYPE xnc,
                SOCKET s,
                char FAR *buf,
                int len,
                int flags,
                struct sockaddr FAR *from,
                int FAR *fromlen)
{
   XBXKRNL_UNIMPLEMENTED;
   return 0;
}

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
                   LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
   XBXKRNL_UNIMPLEMENTED;
   return 0;
}

XBXKRNL int
NetDll_send(XNCALLER_TYPE xnc,
            SOCKET s,
            const char FAR *buf,
            int len,
            int flags)
{
   XBXKRNL_UNIMPLEMENTED;
   return 0;
}

XBXKRNL int
NetDll_WSASend(XNCALLER_TYPE xnc,
               SOCKET s,
               LPWSABUF lpBuffers,
               DWORD dwBufferCount,
               LPDWORD lpNumberOfBytesSent,
               DWORD dwFlags,
               LPWSAOVERLAPPED lpOverlapped,
               LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
   XBXKRNL_UNIMPLEMENTED;
   return 0;
}

XBXKRNL int
NetDll_sendto(XNCALLER_TYPE xnc,
              SOCKET s,
              const char FAR *buf,
              int len,
              int flags,
              const struct sockaddr FAR *to,
              int tolen)
{
   XBXKRNL_UNIMPLEMENTED;
   return 0;
}

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
                 LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
   XBXKRNL_UNIMPLEMENTED;
   return 0;
}

XBXKRNL long
NetDll_inet_addr(XNCALLER_TYPE xnc,
                 const char FAR *cp)
{
   XBXKRNL_UNIMPLEMENTED;
   return 0;
}

XBXKRNL int
NetDll_WSAGetLastError(XNCALLER_TYPE xnc)
{
   return WSAGetLastError();
}

XBXKRNL void
NetDll_WSASetLastError(XNCALLER_TYPE xnc,
                       int iError)
{
   return WSASetLastError(iError);
}

XBXKRNL WSAEVENT
NetDll_WSACreateEvent(XNCALLER_TYPE xnc)
{
   return WSACreateEvent();
}

XBXKRNL BOOL
NetDll_WSACloseEvent(XNCALLER_TYPE xnc,
                     WSAEVENT hEvent)
{
   return WSACloseEvent(hEvent);
}

XBXKRNL BOOL
NetDll_WSASetEvent(XNCALLER_TYPE xnc,
                   WSAEVENT hEvent)
{
   return WSASetEvent(hEvent);
}

XBXKRNL BOOL
NetDll_WSAResetEvent(XNCALLER_TYPE xnc,
                     WSAEVENT hEvent)
{
   return WSAResetEvent(hEvent);
}

XBXKRNL DWORD
NetDll_WSAWaitForMultipleEvents(XNCALLER_TYPE xnc,
                                DWORD cEvents,
                                const WSAEVENT FAR *lphEvents,
                                BOOL fWaitAll,
                                DWORD dwTimeout,
                                BOOL fAlertable)
{
   XBXKRNL_UNIMPLEMENTED;
   return 0;
}

XBXKRNL INT
NetDll___WSAFDIsSet(XNCALLER_TYPE xnc,
                    SOCKET fd,
                    fd_set *set)
{
   XBXKRNL_UNIMPLEMENTED;
   return 0;
}

XBXKRNL int
NetDll_WSAEventSelect(XNCALLER_TYPE xnc,
                      SOCKET s,
                      WSAEVENT hEventObject,
                      long lNetworkEvents)
{
   return WSAEventSelect(s, hEventObject, lNetworkEvents);
}
