#include "server.h"
#include "packet.h"

#include <mswsock.h>
#include <algorithm>
#include <iostream>

namespace gdb
{

Server::Server() :
   mListenSocket(SOCKET_ERROR), mGDBSocket(SOCKET_ERROR), mReadBuf({ 0, 0 })
{
}

Server::~Server()
{
   if (mListenSocket != SOCKET_ERROR) {
      closesocket(mListenSocket);
   }

   if (mGDBSocket != SOCKET_ERROR) {
      closesocket(mGDBSocket);
   }

   if (mReadBuf.len) {
      delete[] mReadBuf.buf;
   }
}

bool Server::start(const std::string &host, int port)
{
   DWORD bytesReceived;
   BOOL res;

   mListenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
   mGDBSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

   if (mListenSocket == SOCKET_ERROR || mGDBSocket == SOCKET_ERROR) {
      return false;
   }

   sockaddr_in addr;
   addr.sin_family = AF_INET;
   addr.sin_addr.s_addr = inet_addr(host.c_str());
   addr.sin_port = htons(port);

   if (bind(mListenSocket, (SOCKADDR *)&addr, sizeof(sockaddr_in)) == SOCKET_ERROR) {
      return false;
   }

   if (listen(mListenSocket, 1) == SOCKET_ERROR) {
      return false;
   }

   ZeroMemory(&mReadOV, sizeof(WSAOVERLAPPED));
   ZeroMemory(&mListenOV, sizeof(WSAOVERLAPPED));

   mReadOV.hEvent = WSACreateEvent();
   mListenOV.hEvent = WSACreateEvent();

   mEvents.push_back(mReadOV.hEvent);
   mEvents.push_back(mListenOV.hEvent);

   res = AcceptEx(mListenSocket,
                  mGDBSocket,
                  mAcceptBuffer,
                  0,
                  sizeof(SOCKADDR_IN)+16,
                  sizeof(SOCKADDR_IN)+16,
                  &bytesReceived,
                  &mListenOV);

   if (res == FALSE) {
      if (WSAGetLastError() != ERROR_IO_PENDING) {
         return false;
      }
   }

   mReadBuf.len = 1024;
   mReadBuf.buf = new CHAR[mReadBuf.len];

   return true;
}

bool Server::run()
{
   Packet packet;

   mRunning = true;

   auto recvMore = [this] {
      DWORD flags = 0;

      mReadOV.Internal = 0;
      mReadOV.InternalHigh = 0;

      mReadOV.Offset = 0;
      mReadOV.OffsetHigh = 0;

      if (WSARecv(mGDBSocket, &mReadBuf, 1, NULL, &flags, &mReadOV, NULL) == SOCKET_ERROR) {
         if (WSAGetLastError() != ERROR_IO_PENDING) {
            return false;
         }
      }

      return true;
   };

   while (mRunning) {
      HANDLE event;
      DWORD index;

      index = WSAWaitForMultipleEvents(mEvents.size(), mEvents.data(), FALSE, WSA_INFINITE, FALSE);

      if (index == WSA_WAIT_FAILED) {
         std::cout << "WSA_WAIT_FAILED" << std::endl;
         mRunning = false;
         continue;
      }

      if (index == WSA_WAIT_TIMEOUT || index == WSA_WAIT_IO_COMPLETION) {
         std::cout << "WSA_WAIT_TIMEOUT || WSA_WAIT_IO_COMPLETION" << std::endl;
         continue;
      }

      index -= WSA_WAIT_EVENT_0;
      event = mEvents.at(index);
      WSAResetEvent(event);

      if (event == mListenOV.hEvent) {
         DWORD bytes = 0, flags = 0;

         if (WSAGetOverlappedResult(mListenSocket, &mListenOV, &bytes, FALSE, &flags) == FALSE) {
            std::cout << "WSAGetOverlappedResult mListenOV == FALSE" << std::endl;
            continue;
         }

         recvMore();
      } else if (event == mReadOV.hEvent) {
         DWORD pos = 0, bytes = 0, flags = 0;

         if (WSAGetOverlappedResult(mGDBSocket, &mReadOV, &bytes, FALSE, &flags) == FALSE) {
            std::cout << "WSAGetOverlappedResult mReadOV == FALSE" << std::endl;
            continue;
         }

         if (bytes == 0) {
            std::cout << "GDB disconnected" << std::endl;

            closesocket(mGDBSocket);
            mEvents.erase(std::remove(mEvents.begin(), mEvents.end(), event), mEvents.end());
         } else {
            while (pos < bytes) {
               if (packet.unpack(pos, bytes, mReadBuf.buf)) {
                  onReceivePacket(packet);
                  packet.reset();
               }
            }

            recvMore();
         }
      } else {
         DWORD bytes = 0, flags = 0;
         Packet *out = NULL;

         for (auto pak : mSendPackets) {
            if (pak->overlapped.hEvent == event) {
               out = pak;
               break;
            }
         }

         if (WSAGetOverlappedResult(mGDBSocket, &out->overlapped, &bytes, FALSE, &flags) == FALSE) {
            std::cout << "WSAGetOverlappedResult mSendOV == FALSE" << std::endl;
            continue;
         }

         if (bytes < out->size()) {
            out->wsaBuf.len -= bytes;
            out->data.erase(out->data.begin(), out->data.begin() + bytes);

            if (!sendPacket(out)) {
               erasePacket(out);
            }
         } else {
            erasePacket(out);
         }
      }
   }

   return true;
}

void Server::gdbGetHaltReason(const Packet &pak)
{
   //gdbSendSignal(SIGNAL_TRAP);
   sendString("S05");
}

void Server::gdbOnVerbosePacket(const Packet &pak)
{
   if (pak.isCommand("vRun")) {
      sendString("S05");
   } else if (pak.isCommand("vCont")) {
      sendString("S05");
   } else {
      sendEmptyPacket();
   }
}

void Server::gdbEnableExtendedMode(const Packet &pak)
{
   sendString("OK");
}

void Server::gdbGetAllRegisters(const Packet &pak)
{
   Packet *out = new Packet();

   /* GPR */
   for (unsigned long i = 0; i < 32; ++i) {
      out->addHex<uint32_t>(i);
   }

   /* FPR */
   for (unsigned long long i = 0; i < 32; ++i) {
      out->addHex<uint64_t>(i);
   }
   
   out->addHex<uint32_t>(0x82f23470); /* PC */
   out->addHex<uint32_t>(GdbRegisters::MSR);
   out->addHex<uint32_t>(GdbRegisters::CR);
   out->addHex<uint32_t>(GdbRegisters::LR);
   out->addHex<uint32_t>(GdbRegisters::CTR);
   out->addHex<uint32_t>(GdbRegisters::XER);
   out->addHex<uint32_t>(GdbRegisters::FPSCR);

   sendPacket(out);
}

void Server::gdbGetRegister(const Packet &pak)
{
   auto reg = pak.readHex();
   Packet *out = new Packet();

   switch (reg) {
   case GdbRegisters::PC:
      out->addHex<uint32_t>(0x82f23470);
      break;
   default:
      out->addHex<uint32_t>(0);
   }

   sendPacket(out);
}

void Server::gdbSetCurrentThread(const Packet &pak)
{
   auto cmd = pak.read<char>();
   auto id = pak.readHex();

   switch (pak[1]) {
   case 'c':
   case 'g':
      sendString("OK");
      break;

   default:
      std::cout << "Error: Unknown gdbSetCurrentThread type " << pak[1] << std::endl;
      sendString("E01");
   }
}

void Server::gdbOnQueryPacket(const Packet &pak)
{
   if (pak.isCommand("qfThreadInfo")) {
      /* begin thread list */
      sendString("m01");
   } else if (pak.isCommand("qsThreadInfo")) {
      /* next item in thread list */
      /* l = end of list */
      sendString("l");
   } else if (pak.isCommand("qC")) {
      sendString("QC01");
   } else {
      sendEmptyPacket();
   }
}

void Server::gdbSendOK()
{
   sendString("OK");
}

void Server::onReceivePacket(const Packet &pak)
{
   if (!pak.verify()) {
      std::cout << "Invalid checksum!" << std::endl;
   }
   
   std::cout << "[RECV] ";
   printPacket(pak);
   std::cout << std::endl;

   switch (pak.read<char>()) {
   case '?': /* Halt Reason */
      gdbGetHaltReason(pak);
      break;

   case '!': /* Enable Extended Mode */
      gdbEnableExtendedMode(pak);
      break;

   case 'v': /* Verbose Packets */
      gdbOnVerbosePacket(pak);
      break;

   case 'g': /* Read All Registers */
      gdbGetAllRegisters(pak);
      break;

   case 'H': /* Set Current Thread */
      gdbSetCurrentThread(pak);
      break;

   case 'c': /* Continue */
      sendString("S05");
      break;

   case 'q': /* Query Packets */
      gdbOnQueryPacket(pak);
      break;

   case 'p': /* Read Specific Register */
      gdbGetRegister(pak);
      break;

   case 's': /* Single Step */

   case 'M': /* Write Memory */
   case 'm': /* Read Memory */

   case 'Z': /* Insert Breakpoint */
   case 'z': /* Remove Breakpoint */

   default:
      sendEmptyPacket();
   }
}

bool Server::sendString(const std::string &reply)
{
   auto out = new Packet();
   out->add(reply);
   return sendPacket(out);
}

bool Server::sendEmptyPacket()
{
   return sendPacket(new Packet());
}

bool Server::sendPacket(Packet *out)
{
   DWORD bytes = 0, flags = 0;

   std::cout << "[SEND] ";
   printPacket(*out);
   std::cout << std::endl;

   out->pack();

   if (WSASend(mGDBSocket, &out->wsaBuf, 1, &bytes, 0, &out->overlapped, NULL) == SOCKET_ERROR) {
      if (WSAGetLastError() != ERROR_IO_PENDING) {
         delete out;
         return false;
      }
   }

   mEvents.push_back(out->overlapped.hEvent);
   mSendPackets.push_back(out);
   return true;
}

void Server::printPacket(const Packet &pak)
{
   for (DWORD i = 0; i < pak.size(); ++i) {
      std::cout << pak[i];
   }
}

bool Server::erasePacket(Packet *out)
{
   mEvents.erase(std::remove(mEvents.begin(), mEvents.end(), out->overlapped.hEvent), mEvents.end());
   mSendPackets.erase(std::remove(mSendPackets.begin(), mSendPackets.end(), out), mSendPackets.end());

   delete out;
   return true;
}

}; // namespace gdb
