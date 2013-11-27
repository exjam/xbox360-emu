#ifndef GDB_PACKET_H
#define GDB_PACKET_H

#include <WinSock2.h>
#include <vector>
#include <string>
#include <stdint.h>

namespace gdb
{
   
class Packet
{
public:
   Packet() :
      mReadFinished(false)
   {
   }

   ~Packet()
   {
      if (overlapped.hEvent) {
         WSACloseEvent(overlapped.hEvent);
      }
   }

   void reset()
   {
      mReadFinished = false;
      mChecksum = 0;
      data.clear();

      ZeroMemory(&wsaBuf, sizeof(WSABUF));
      ZeroMemory(&overlapped, sizeof(WSAOVERLAPPED));
   }

   void pack()
   {
      static char hex[] =
      {
         '0', '1', '2', '3', '4', '5', '6', '7',
         '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
      };

      mChecksum = calcChecksum();

      data.insert(data.begin(), '$');
      data.push_back('#');
      data.push_back(hex[mChecksum >> 4]);
      data.push_back(hex[mChecksum % 16]);

      overlapped.hEvent = WSACreateEvent();

      wsaBuf.len = data.size();
      wsaBuf.buf = reinterpret_cast<CHAR*>(data.data());
   }

   bool unpack(DWORD &pos, DWORD max, const CHAR *buf)
   {
      DWORD start = pos;

      for (; pos < max; ++pos) {
         /* Find packet start*/
         if (data.size() == 0 && buf[pos] != '$') {
            continue;
         }

         data.push_back(buf[pos]);

         /* Find packet end*/
         if (data.size() >= 4 && data.at(data.size() - 3) == '#') {
            pos++;

            mReadFinished = true;
            mReadPosition = 0;
            mChecksum = readChecksum();

            data.erase(data.begin());
            data.erase(data.end() - 3, data.end());
            break;
         }
      }

      return mReadFinished;
   }

   bool verify() const
   {
      if (!mReadFinished) {
         return true;
      } else {
         return mChecksum == calcChecksum();
      }
   }

   bool isReadFinished() const
   {
      return mReadFinished;
   }

   unsigned char operator[](int i) const
   {
      return data.at(i);
   }

   std::size_t size() const
   {
      return data.size();
   }

   bool isCommand(const char *cmd) const
   {
      auto len = strlen(cmd);

      if (data.size() < len) {
         return false;
      }

      return memcmp(reinterpret_cast<const char*>(data.data()), cmd, len) == 0;
   }

   template<typename Type>
   void add(const Type &v)
   {
      assert(0 && "Unknown add<Type>");
   }

   template<>
   void add(const std::string &v)
   {
      data.insert(data.end(), v.begin(), v.end());
   }

   template<typename Type>
   void addHex(Type value)
   {
      assert(0 && "Unknown addHex<Type>");
   }

   template<>
   void addHex<uint32_t>(uint32_t value)
   {
      char buffer[32];
      sprintf_s(buffer, 32, "%08X", value);
      add(std::string(buffer));
   }

   template<>
   void addHex<uint64_t>(uint64_t value)
   {
      char buffer[32];
      sprintf_s(buffer, 32, "%016llX", value);
      add(std::string(buffer));
   }

   template<typename Type>
   Type read() const
   {
      assert(0 && "Unknown read<Type>");
   }

   template<>
   char read() const
   {
      return static_cast<char>(data[mReadPosition++]);
   }

   uint64_t readHex() const
   {
      unsigned long long result;
      std::size_t ofs = 0;

      result = std::stoull(reinterpret_cast<const char*>(data.data() + mReadPosition), &ofs, 16);
      mReadPosition += ofs;

      return result;
   }

protected:
   unsigned char readChecksum() const
   {
      std::string hex(data.end() - 2, data.end());
      return static_cast<unsigned char>(std::stoul(hex, NULL, 16));
   }

   unsigned char calcChecksum() const
   {
      unsigned char checksum = 0;

      for (auto c : data) {
         if (c == '$') {
            continue;
         }

         if (c == '#') {
            break;
         }

         checksum += c;
      }

      return checksum;
   }

public:
   std::vector<unsigned char> data;
   WSABUF wsaBuf;
   WSAOVERLAPPED overlapped;

private:
   bool mReadFinished;
   mutable std::size_t mReadPosition;
   unsigned char mChecksum;
};

}; // namespace gdb

#endif // ifndef GDB_PACKET_H
