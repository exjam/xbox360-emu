#include "server.h"
#include "packet.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")

int main(int argc, char **argv)
{
   WSADATA wsaData;
   WSAStartup(MAKEWORD(2, 2), &wsaData);

   gdb::Server srv;

   if (srv.start("127.0.0.1", 1234)) {
      srv.run();
   }

   WSACleanup();

   return 0;
}
