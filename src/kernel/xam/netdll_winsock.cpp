#include "netdll_winsock.h"

#include <WinSock2.h>
#include <MSWSock.h>

#include "common/bits.h"
#include "common/memory.h"

#pragma comment(lib, "ws2_32.lib")

template<typename SrcType, typename DstType>
void swapSockAddr(const SrcType src, DstType dst)
{
   if (dst && src) {
      dst->sa_family = bits::swap(src->sa_family);

      memcpy(dst->sa_data, src->sa_data, 14);
   }
}

template<typename SrcType, typename DstType>
void swapWsaData(const SrcType src, DstType dst)
{
   if (dst && src) {
      dst->wVersion     = bits::swap(src->wVersion);
      dst->wHighVersion = bits::swap(src->wHighVersion);
      dst->iMaxSockets  = bits::swap(src->iMaxSockets);
      dst->iMaxUdpDg    = bits::swap(src->iMaxUdpDg);
      dst->lpVendorInfo = bits::swap(src->lpVendorInfo);

      memcpy(dst->szDescription, src->szDescription, 257);
      memcpy(dst->szSystemStatus, src->szSystemStatus, 257);
   }
}

template<typename SrcType, typename DstType>
void swapFdSet(const SrcType src, DstType dst)
{
   if (dst && src) {
      DWORD count;
      auto test = dst->fd_array[0];

      dst->fd_count = bits::swap(src->fd_count);

      count = min(src->fd_count, dst->fd_count);

      for (DWORD i = 0; i < count; ++i) {
         dst->fd_array[i] = bits::swap(src->fd_array[i]);
      }
   }
}

template<typename SrcType, typename DstType>
void swapTimeval(const SrcType src, DstType dst)
{
   if (dst && src) {
      dst->tv_sec = bits::swap(src->tv_sec);
      dst->tv_usec = bits::swap(src->tv_usec);
   }
}

XBXKRNL XINT
NetDll_WSAStartup(XNCALLER_TYPE xnc,
                  XWORD wVersionRequested,
                  XLPWSADATA lpXWSAData)
{
   WSADATA wsaData;
   XINT result;

   result = WSAStartup(wVersionRequested, &wsaData);

   swapWsaData(&wsaData, lpXWSAData);

   return result;
}

XBXKRNL XINT
NetDll_WSACleanup(XNCALLER_TYPE xnc)
{
   return WSACleanup();
}

XBXKRNL XPVOID
NetDll_socket(XNCALLER_TYPE xnc,
              XINT af,
              XINT type,
              XINT protocol)
{
   return reinterpret_cast<XPVOID>(socket(af, type, protocol));
}

XBXKRNL XINT
NetDll_closesocket(XNCALLER_TYPE xnc,
                   XSOCKET s)
{
   return closesocket(s);
}

XBXKRNL XINT
NetDll_shutdown(XNCALLER_TYPE xnc,
                XSOCKET s,
                XINT how)
{
   return shutdown(s, how);
}

XBXKRNL XINT
NetDll_ioctlsocket(XNCALLER_TYPE xnc,
                   XSOCKET s,
                   XINT cmd,
                   XLPDWORD argp)
{
   XDWORD wsaArgp;
   XINT result;

   swapField(argp, &wsaArgp);

   result = ioctlsocket(s,
                        cmd,
                        reinterpret_cast<u_long*>(wsaArgp));

   swapField(&wsaArgp, argp);

   return result;
}

void swapSockOpt(XINT optname, const XLPCHAR src, XLPCHAR dst)
{
   if (!src || !dst) {
      return;
   }

   switch (optname) {
   case SO_ACCEPTCONN:
   case SO_BROADCAST:
   case SO_CONDITIONAL_ACCEPT:
   case SO_DEBUG:
   case SO_DONTLINGER:
   case SO_DONTROUTE:
   case SO_EXCLUSIVEADDRUSE:
   case SO_KEEPALIVE:
   case SO_OOBINLINE:
   case SO_PORT_SCALABILITY:
   case SO_REUSEADDR:
   {
      *XLPBOOL(dst) = bits::swap(*XLPBOOL(src));
      break;
   }

   case SO_CONNECT_TIME:
   case SO_MAX_MSG_SIZE:
   case SO_GROUP_ID:
   case SO_RCVTIMEO:
   case SO_SNDTIMEO:
   {
      *XLPDWORD(dst) = bits::swap(*XLPDWORD(src));
      break;
   }

   case SO_ERROR:
   case SO_GROUP_PRIORITY:
   case SO_RCVBUF:
   case SO_SNDBUF:
   case SO_TYPE:
   {
      *XLPINT(dst) = bits::swap(*XLPINT(src));
      break;
   }

   case SO_LINGER:
   {
      XLP<LINGER> srcLinger(src), dstLinger(dst);
      dstLinger->l_linger = bits::swap(srcLinger->l_linger);
      dstLinger->l_onoff  = bits::swap(srcLinger->l_onoff);
      break;
   }

   case SO_BSP_STATE:     /* CSADDR_INFO */
   case SO_PROTOCOL_INFO: /* WSAPROTOCOL_INFO */
      XBXKRNL_UNIMPLEMENTED
   }
}

XBXKRNL XINT
NetDll_setsockopt(XNCALLER_TYPE xnc,
                  XSOCKET s,
                  XINT level,
                  XINT optname,
                  XLPCHAR optval,
                  XINT optlen)
{
   char *wsaOptVal;
   XINT result;

   wsaOptVal = new char[optlen];

   swapSockOpt(optname, optval, wsaOptVal);

   result = setsockopt(s,
                       level,
                       optname,
                       wsaOptVal,
                       optlen);

   delete[] wsaOptVal;
   return result;
}

XBXKRNL XINT
NetDll_getsockopt(XNCALLER_TYPE xnc,
                  XSOCKET s,
                  XINT level,
                  XINT optname,
                  XLPCHAR optval,
                  XLPINT optlen)
{
   char wsaOptVal[255];
   int wsaOptLen, result;

   wsaOptLen = 255;
   result = getsockopt(s, level, optname, wsaOptVal, &wsaOptLen);
   
   swapSockOpt(optname, wsaOptVal, optval);

   return result;
}

XBXKRNL XINT
NetDll_getsockname(XNCALLER_TYPE xnc,
                   XSOCKET s,
                   XLPSOCKADDR name,
                   XLPINT namelen)
{
   SOCKADDR wsaName;
   XINT wsaNameLen, result;

   wsaNameLen = sizeof(sockaddr);

   result = getsockname(s, &wsaName, &wsaNameLen);

   swapSockAddr(&wsaName, name);
   swapField(&wsaNameLen, namelen);

   return result;
}

XBXKRNL XINT
NetDll_getpeername(XNCALLER_TYPE xnc,
                   XSOCKET s,
                   XLPSOCKADDR name,
                   XLPINT namelen)
{
   SOCKADDR wsaName;
   XINT wsaNameLen, result;

   wsaNameLen = sizeof(sockaddr);

   result = getpeername(s, &wsaName, &wsaNameLen);

   swapSockAddr(&wsaName, name);
   swapField(&wsaNameLen, namelen);

   return result;
}

XBXKRNL XINT
NetDll_bind(XNCALLER_TYPE xnc,
            XSOCKET s,
            const XLPSOCKADDR name,
            XINT namelen)
{
   SOCKADDR wsaName;

   swapSockAddr(name, &wsaName);

   return bind(s, &wsaName, namelen);
}

XBXKRNL XINT
NetDll_connect(XNCALLER_TYPE xnc,
               XSOCKET s,
               const XLPSOCKADDR name,
               XINT namelen)
{
   SOCKADDR wsaName;

   swapSockAddr(name, &wsaName);

   return connect(s, &wsaName, namelen);
}

XBXKRNL XINT
NetDll_listen(XNCALLER_TYPE xnc,
              XSOCKET s,
              XINT backlog)
{
   return listen(s, backlog);
}

XBXKRNL XPVOID
NetDll_accept(XNCALLER_TYPE xnc,
              XSOCKET s,
              XLPSOCKADDR addr,
              XLPINT addrlen)
{
   SOCKADDR wsaAddr;
   XINT wsaAddrLen;
   SOCKET result;

   result = accept(s, &wsaAddr, &wsaAddrLen);

   swapSockAddr(&wsaAddr, addr);
   swapField(&wsaAddrLen, addrlen);

   return reinterpret_cast<XPVOID>(result);
}

XBXKRNL XINT
NetDll_select(XNCALLER_TYPE xnc,
              XINT nfds,
              XLPFD_SET readfds,
              XLPFD_SET writefds,
              XLPFD_SET exceptfds,
              XLPTIMEVAL timeout)
{
   FD_SET wsaReadFds, wsaWriteFds, wsaExceptFds;
   TIMEVAL wsaTimeout;
   XINT result;
   
   swapFdSet(readfds,   &wsaReadFds);
   swapFdSet(writefds,  &wsaWriteFds);
   swapFdSet(exceptfds, &wsaExceptFds);
   swapTimeval(timeout, &wsaTimeout);

   result = select(nfds,
                   readfds   ? &wsaReadFds   : nullptr,
                   writefds  ? &wsaWriteFds  : nullptr,
                   exceptfds ? &wsaExceptFds : nullptr,
                   timeout   ? &wsaTimeout   : nullptr);

   swapFdSet(&wsaReadFds, readfds);
   swapFdSet(&wsaWriteFds, writefds);
   swapFdSet(&wsaExceptFds, exceptfds);

   return result;
}

XBXKRNL XBOOL
NetDll_WSAGetOverlappedResult(XNCALLER_TYPE xnc,
                              XSOCKET s,
                              XLPWSAOVERLAPPED lpOverlapped,
                              XLPDWORD lpcbTransfer,
                              XBOOL fWait,
                              XLPDWORD lpdwFlags)
{
   XBXKRNL_UNIMPLEMENTED;
   return 0;
}

XBXKRNL XINT
NetDll_WSACancelOverlappedIO(XNCALLER_TYPE xnc,
                             XSOCKET s)
{
   XBXKRNL_UNIMPLEMENTED;
   return 0;
}

XBXKRNL XINT
NetDll_recv(XNCALLER_TYPE xnc,
            XSOCKET s,
            XLPCHAR buf,
            XINT len,
            XINT flags)
{
   return recv(s, buf, len, flags);
}

XBXKRNL XINT
NetDll_WSARecv(XNCALLER_TYPE xnc,
               XSOCKET s,
               XLPWSABUF lpBuffers,
               XDWORD dwBufferCount,
               XLPDWORD lpNumberOfBytesRecvd,
               XLPDWORD lpFlags,
               XLPWSAOVERLAPPED lpOverlapped,
               XLPVOID lpCompletionRoutine)
{
   XBXKRNL_UNIMPLEMENTED;
   return 0;
}

XBXKRNL XINT
NetDll_recvfrom(XNCALLER_TYPE xnc,
                XSOCKET s,
                XLPCHAR buf,
                XINT len,
                XINT flags,
                XLPSOCKADDR from,
                XLPINT fromlen)
{
   XBXKRNL_UNIMPLEMENTED;
   return 0;
}

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
                   XLPVOID lpCompletionRoutine)
{
   XBXKRNL_UNIMPLEMENTED;
   return 0;
}

XBXKRNL XINT
NetDll_send(XNCALLER_TYPE xnc,
            XSOCKET s,
            const XLPCHAR buf,
            XINT len,
            XINT flags)
{
   return send(s, buf, len, flags);
}

XBXKRNL XINT
NetDll_WSASend(XNCALLER_TYPE xnc,
               XSOCKET s,
               XLPWSABUF lpBuffers,
               XDWORD dwBufferCount,
               XLPDWORD lpNumberOfBytesSent,
               XDWORD dwFlags,
               XLPWSAOVERLAPPED lpOverlapped,
               XLPVOID lpCompletionRoutine)
{
   XBXKRNL_UNIMPLEMENTED;
   return 0;
}

XBXKRNL XINT
NetDll_sendto(XNCALLER_TYPE xnc,
              XSOCKET s,
              const XLPCHAR buf,
              XINT len,
              XINT flags,
              const XLPSOCKADDR to,
              XINT tolen)
{
   XBXKRNL_UNIMPLEMENTED;
   return 0;
}

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
                 XLPVOID lpCompletionRoutine)
{
   XBXKRNL_UNIMPLEMENTED;
   return 0;
}

XBXKRNL XINT
NetDll_inet_addr(XNCALLER_TYPE xnc,
                 XLPCHAR cp)
{
   return inet_addr(cp);
}

XBXKRNL XINT
NetDll_WSAGetLastError(XNCALLER_TYPE xnc)
{
   return WSAGetLastError();
}

XBXKRNL void
NetDll_WSASetLastError(XNCALLER_TYPE xnc,
                       XINT iError)
{
   return WSASetLastError(iError);
}

XBXKRNL XPVOID
NetDll_WSACreateEvent(XNCALLER_TYPE xnc)
{
   return reinterpret_cast<XPVOID>(WSACreateEvent());
}

XBXKRNL XBOOL
NetDll_WSACloseEvent(XNCALLER_TYPE xnc,
                     XWSAEVENT hEvent)
{
   return WSACloseEvent(hEvent);
}

XBXKRNL XBOOL
NetDll_WSASetEvent(XNCALLER_TYPE xnc,
                   XWSAEVENT hEvent)
{
   return WSASetEvent(hEvent);
}

XBXKRNL XBOOL
NetDll_WSAResetEvent(XNCALLER_TYPE xnc,
                     XWSAEVENT hEvent)
{
   return WSAResetEvent(hEvent);
}

XBXKRNL XDWORD
NetDll_WSAWaitForMultipleEvents(XNCALLER_TYPE xnc,
                                XDWORD cEvents,
                                const XLPWSAEVENT lphEvents,
                                XBOOL fWaitAll,
                                XDWORD dwTimeout,
                                XBOOL fAlertable)
{
   WSAEVENT *wsaLphEvents = new WSAEVENT[cEvents];

   for (XDWORD i = 0; i < cEvents; ++i) {
      wsaLphEvents[i] = bits::swap(lphEvents[i]);
   }

   WSAWaitForMultipleEvents(cEvents,
                            wsaLphEvents,
                            fWaitAll,
                            dwTimeout,
                            fAlertable);

   delete[] wsaLphEvents;
   return 0;
}

XBXKRNL XINT
NetDll___WSAFDIsSet(XNCALLER_TYPE xnc,
                    XSOCKET fd,
                    XLPFD_SET set)
{
   XBXKRNL_UNIMPLEMENTED;
   return 0;
}

XBXKRNL XINT
NetDll_WSAEventSelect(XNCALLER_TYPE xnc,
                      XSOCKET s,
                      XWSAEVENT hEventObject,
                      XINT lNetworkEvents)
{
   return WSAEventSelect(s,
                         hEventObject,
                         lNetworkEvents);
}
