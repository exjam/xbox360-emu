#include <idc.idc>

static main() {
   auto entryPoints, i, ord, addr, name, purged, file, fd;
   
   file = AskFile(1, "*.ord", "Select Ordinal save file");
   
   fd = fopen(file, "w");
   entryPoints = GetEntryPointQty();
   
   fprintf(fd, "0 : %s\n", GetInputFile());
   
   for (i = 0; i < entryPoints; i++) {
      ord = GetEntryOrdinal(i);
      if (ord == 0) {
		continue;
	  }
	  
      addr = GetEntryPoint(ord);
	  
      if (ord == addr) {
         continue;
      }
	  
      name = Name(addr);
      fprintf(fd, "%d : %s\n", ord, name);
   }
}
