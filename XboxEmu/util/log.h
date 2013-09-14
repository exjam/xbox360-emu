#ifndef UTIL_LOG_H
#define UTIL_LOG_H

#include <stdint.h>
#include <iostream>
#include <iomanip>

class Log 
{
public:
   struct hex {
      hex(uint32_t number, uint32_t pad = 0) : number(number), pad(pad) {}
      uint32_t number;
      uint32_t pad;
   };

   struct bin {
      bin(uint32_t number, uint32_t pad = 0) : number(number), pad(pad) {}
      uint32_t number;
      uint32_t pad;
   };

private:
   class Debug
   {
   public:
      Debug(bool newLine = true)
      {
         if (newLine) {
            std::cout << std::endl << "[DEBUG] ";
         }
      }

      template<typename T>
      Debug& operator << (const T &val) 
      {
         std::cout << val;
         return *this;
      }

      template<>
      Debug& operator << (const Log::hex &val) 
      {
         if (val.pad) {
            std::cout << std::hex << std::setfill('0') << std::setw(val.pad) << val.number;
         } else {
            std::cout << std::hex << val.number;
         }

         std::cout << std::dec;
         return *this;
      }

      template<>
      Debug& operator << (const Log::bin &val) 
      {
         for (uint32_t i = 0; i < val.pad; ++i) {
            std::cout << ((val.number >> i) & 1);
         }

         return *this;
      }

   private:
   };

public:
   Debug debug(bool newLine = true)
   {
      return Debug(newLine);
   }

private:
};

extern Log g_log;

#define xDebug() g_log.debug()
#define xDebugInline() g_log.debug(false)

#endif // UTIL_LOG_H
