#ifndef UTIL_LOG_H
#define UTIL_LOG_H

#include <stdint.h>
#include <iostream>
#include <iomanip>
#include <string>

class Log 
{
public:
   struct hex {
      hex(uint64_t number, uint64_t pad = 0) : number(number), pad(pad) {}
      uint64_t number;
      uint64_t pad;
   };

   struct bin {
      bin(uint32_t number, uint32_t pad = 0) : number(number), pad(pad) {}
      uint32_t number;
      uint32_t pad;
   };

private:
   class Output
   {
   public:
      Output(const std::string &pre, bool newLine = true)
      {
         if (newLine) {
            std::cout << std::endl << pre << " ";
         }
      }

      template<typename T>
      Output& operator << (const T &val)
      {
         std::cout << val;
         return *this;
      }

      template<>
      Output& operator << (const Log::hex &val)
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
      Output& operator << (const Log::bin &val)
      {
         for (uint32_t i = 0; i < val.pad; ++i) {
            std::cout << ((val.number >> i) & 1);
         }

         return *this;
      }

   private:
   };

public:
   Output debug(bool newLine = true)
   {
      return Output("[DEBUG]", newLine);
   }

   Output warning(bool newLine = true)
   {
      return Output("[WARNING]", newLine);
   }

   Output error(bool newLine = true)
   {
      return Output("[ERROR]", newLine);
   }

private:
};

extern Log g_log;

#define xDebug() g_log.debug()
#define xWarning() g_log.warning()
#define xError() g_log.error()

#define xDebugInline() g_log.debug(false)
#define xWarningInline() g_log.warning(false)
#define xErrorInline() g_log.error(false)

#endif // UTIL_LOG_H
