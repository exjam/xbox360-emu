#ifndef GDB_SERVER_H
#define GDB_SERVER_H

#include <WinSock2.h>
#include <string>
#include <vector>

namespace gdb
{

class Packet;
   
class Server
{
public:
   enum GdbRegisters
   {
      GPR00 = 0,
      GPR31 = 31,
      FPR00 = 32,
      FPR31 = 63,
      PC = 64,
      MSR,
      CR,
      LR,
      CTR,
      XER,
      FPSCR
   };

public:
   Server();
   ~Server();

   bool start(const std::string &host, int port);
   bool run();

protected:
   void onReceivePacket(const Packet &pak);
   
   bool sendEmptyPacket();
   bool sendString(const std::string &reply);
   bool sendPacket(Packet *out);

   bool erasePacket(Packet *out);
   void printPacket(const Packet &pak);

   /* GDB Receive Packets */
   void gdbGetRegister(const Packet &pak);
   void gdbOnQueryPacket(const Packet &pak);
   void gdbGetHaltReason(const Packet &pak);
   void gdbGetAllRegisters(const Packet &pak);
   void gdbOnVerbosePacket(const Packet &pak);
   void gdbSetCurrentThread(const Packet &pak);
   void gdbEnableExtendedMode(const Packet &pak);

   /* GDB Send Packets */
   void gdbSendOK();

private:
   volatile bool mRunning;

   SOCKET mListenSocket;
   SOCKET mGDBSocket;

   WSAOVERLAPPED mListenOV;
   WSAOVERLAPPED mReadOV;

   CHAR mAcceptBuffer[2 * (sizeof(SOCKADDR_IN) + 16)];

   std::vector<HANDLE> mEvents;

   WSABUF mReadBuf;
   std::vector<Packet*> mSendPackets;
};

};

#endif // ifndef GDB_SERVER_H
