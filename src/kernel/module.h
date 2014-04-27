#ifndef KERNEL_MODULE_H
#define KERNEL_MODULE_H

#include <map>

class Module
{
public:
   struct Export
   {
      std::string name;
      int ordinal;
      int args;
      void *ptr;
   };

   Module()
   {
   }

   Module(std::string name) :
      mName(name)
   {
   }

   std::string getName() const
   {
      return mName;
   }

   const Export *getExport(int ordinal) const
   {
      auto itr = mExports.find(ordinal);

      if (itr == mExports.end()) {
         return nullptr;
      } else {
         return &itr->second;
      }
   }

protected:
   void addExport(std::string name, int ordinal, int args, void *ptr)
   {
      mExports.emplace(ordinal, Export { name, ordinal, args, ptr });
   }

   void addExport(std::string name, int ordinal, void *ptr)
   {
      mExports.emplace(ordinal, Export { name, ordinal, -1, ptr });
   }

private:
   std::string mName;
   std::map<int, Export> mExports;
};

template<typename Type>
struct argument_count
{
   static const std::size_t value = 0;
};

template<typename ReturnType, typename ...Args>
struct argument_count<ReturnType (Args...)>
{
   static const std::size_t value = sizeof...(Args);
};

#define exportFunction(ordinal, function) \
   addExport(#function, ordinal, argument_count<decltype(function)>::value, (void*)&function);

#define exportVariable(ordinal, variable) \
   addExport(#variable, ordinal, (void*)&variable);

#endif // KERNEL_LIBRARY_H
