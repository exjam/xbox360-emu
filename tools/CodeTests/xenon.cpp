#include "xenon.h"
#include "tester.h"

#include <Windows.h>

std::pair<DWORD, std::string> execute(const std::string& path, const std::string& args, const std::string &workDir)
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

   success = CreateProcessA(NULL, buf, NULL, NULL, TRUE, 0, NULL, workDir.c_str(), &si, &pi);

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


namespace xenon
{

std::string xenonPath;

void setPath(const std::string &path)
{
   xenonPath = path;
}

bool assemble(const std::string &in, const std::string &out)
{
   std::string args = in + " -o \"" + out + "\"";
   std::string wd = g_tester.getTestRoot();

   auto result = execute(xenonPath + "\\xenon-as.exe", args, wd);

   return result.first == 0;
}

};
