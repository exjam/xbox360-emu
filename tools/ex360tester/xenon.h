#ifndef XENON_H
#define XENON_H

#include <string>

namespace xenon
{
   void setPath(const std::string &path);

   bool assemble(const std::string &in, const std::string &out);
};

#endif
