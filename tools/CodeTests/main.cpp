#include <filesystem>
#include <iostream>
#include <Windows.h>
#include <sstream>
#include <utility>
#include "parser/parser.h"

/* Kind of need a rewrite of parser lib so we can have forward declarations

auto whitespace = atomic(*(char_(' ') | char_('\t') | char_('\n') | char_('\r')));
auto letter = char_range('a', 'z') | char_range('A', 'Z');
auto digit = char_range('0', '9');

auto symbol_reg = ast<ast_symbol>() >> atomic(letter >> *(letter | digit));
auto conditional = string_("==") | string_("!=") | string_(">") | string_("<") | string_(">=") | string_("<=");

auto pre_condition = symbol_reg >> '=' >> term;

auto post_condition = symbol_reg >> conditional >> value;

auto test_comment = char_('#');
auto test_pre = char_('#') >> string_("pre") >> char_(":") >> pre_condition;
auto test_post = char_('#') >> string_("post") >> char_(":") >> post_condition;
*/

std::tr2::sys::path xenon;
std::tr2::sys::path xenon_as;
std::tr2::sys::path xenon_objcopy;
std::tr2::sys::path test_root;

std::pair<DWORD, std::string> run(std::string path, std::string args)
{
   HANDLE hReadOut, hWriteOut;
   SECURITY_ATTRIBUTES sa;
   PROCESS_INFORMATION pi;
   STARTUPINFOA si;
   BOOL success;
   CHAR buf[512];
   DWORD exitCode = -1;
   std::string output;
   DWORD dwRead;
   std::string cd = test_root.string();
   
   memcpy(buf, path.c_str(), path.length());
   buf[path.length()] = ' ';
   memcpy(buf + path.length() + 1, args.c_str(), args.length());
   buf[path.length() + 1 + args.length()] = 0;

   sa.nLength = sizeof(SECURITY_ATTRIBUTES);
   sa.bInheritHandle = TRUE;
   sa.lpSecurityDescriptor = NULL;

   if (!CreatePipe(&hReadOut, &hWriteOut, &sa, 0)) {
      output = "Could not create pipe";
      return std::make_pair(exitCode, output);
   }

   ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
   ZeroMemory(&si, sizeof(STARTUPINFOA));

   si.cb = sizeof(STARTUPINFOA);
   si.hStdError = hWriteOut;
   si.hStdOutput = hWriteOut;
   si.hStdInput = NULL;
   si.dwFlags |= STARTF_USESTDHANDLES;

   success = CreateProcessA(NULL, buf, NULL, NULL, TRUE, 0, NULL, cd.c_str(), &si, &pi);

   if (!success) {
      output = "CreateProcessA failed";
      return std::make_pair(exitCode, output);
   }

   CloseHandle(hWriteOut);

   for (;;) {
      success = ReadFile(hReadOut, buf, 512, &dwRead, NULL);

      if (!success || dwRead == 0) {
         break;
      }

      output.append(buf, dwRead);
   }

   WaitForSingleObject(pi.hProcess, INFINITE);
   GetExitCodeProcess(pi.hProcess, &exitCode);
   CloseHandle(pi.hProcess);
   CloseHandle(pi.hThread);
   CloseHandle(hReadOut);

   return std::make_pair(exitCode, output);
}

void execTest(std::tr2::sys::path path)
{
   std::string relPath = path.string();
   relPath.erase(0, test_root.string().length() + 1);

   std::cout << "Testing " << relPath << std::endl;

   /* Assemble path to .elf */
   {
      std::string args = relPath + " -o tmp.elf";
      
      auto ret = run(xenon_as.string(), args);

      if (ret.first != 0) {
         std::cout << "[ERROR] xenon-as.exe failed" << std::endl;
         std::cout << ret.second << std::endl;
         return;
      }
   }

   /* Copy raw code from tmp.elf to tmp.o */
   {
      std::string args = "-O binary tmp.elf tmp.obj";

      auto ret = run(xenon_objcopy.string(), args);

      if (ret.first != 0) {
         std::cout << "[ERROR] xenon-objcopy.exe failed" << std::endl;
         std::cout << ret.second << std::endl;
         return;
      }
   }

   /* Emulate tmp.obj */
   {
      std::cout << "[FAIL] could not run interpreter on tmp.obj" << std::endl;
   }
}

void execDirectory(std::tr2::sys::path root)
{
   for (auto itr = std::tr2::sys::recursive_directory_iterator(root); itr != std::tr2::sys::recursive_directory_iterator(); ++itr) {
      if (std::tr2::sys::is_directory(itr->status())) {
         std::cout << "Entering " << itr->path() << std::endl;
      } else if (std::tr2::sys::is_regular_file(itr->status())) {
         execTest(itr->path());
      }
   }
}

int main(int argc, char** argv)
{
   if (argc < 3) {
      std::cout << argv[0] << "<xenon\bin> <in dir>" << std::endl;
      return 0;
   }

   xenon = argv[1];
   test_root = argv[2];

   xenon_as = xenon / std::tr2::sys::path("xenon-as.exe");
   xenon_objcopy = xenon / std::tr2::sys::path("xenon-objcopy.exe");

   execDirectory(test_root);

   return 0;
}
