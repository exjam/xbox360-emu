#ifndef KERNEL_MODULE_H
#define KERNEL_MODULE_H

#include <map>
#include <stdint.h>

class Module
{
public:
   //TODO: replace with index_sequence, C++14
   template<unsigned ...I>
   struct sequence { };

   template<unsigned N>
   struct make_sequence;
   template<> struct make_sequence<0u> { using type = sequence<>; };
   template<> struct make_sequence<1u> { using type = sequence<3>; };
   template<> struct make_sequence<2u> { using type = sequence<3, 4>; };
   template<> struct make_sequence<3u> { using type = sequence<3, 4, 5>; };
   template<> struct make_sequence<4u> { using type = sequence<3, 4, 5, 6>; };
   template<> struct make_sequence<5u> { using type = sequence<3, 4, 5, 6, 7>; };
   template<> struct make_sequence<6u> { using type = sequence<3, 4, 5, 6, 7, 8>; };
   template<> struct make_sequence<7u> { using type = sequence<3, 4, 5, 6, 7, 8, 9>; };
   template<> struct make_sequence<8u> { using type = sequence<3, 4, 5, 6, 7, 8, 9, 10>; };

   // TODO: Revisit if non-gpr are used for function calls!
   struct BoundFunction
   {
      virtual void call(uint64_t gpr[32]) = 0;
   };

   template<typename Ret, typename... Args>
   struct FunctionWrapper : public BoundFunction
   {
      FunctionWrapper(Ret(*func)(Args...)) :
         mWrappedFunction(func)
      {
      }

      Ret(*mWrappedFunction)(Args...);

      template<std::size_t... I>
      Ret dispatch(uint64_t gpr[32], sequence<I...>)
      {
         return mWrappedFunction(*reinterpret_cast<Args*>(&gpr[I])...);
      }

      void call(uint64_t gpr[32]) override
      {
         *reinterpret_cast<Ret*>(&gpr[3]) = dispatch(gpr, make_sequence<sizeof...(Args)>::type {});
      }
   };

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

template<typename ReturnType, typename... Args>
static inline 
Module::FunctionWrapper<ReturnType, Args...>* make_function(ReturnType(*func)(Args...))
{
   return new Module::FunctionWrapper<ReturnType, Args...> { func };
}

#define exportFunction(ordinal, function) \
   addExport(#function, ordinal, argument_count<decltype(function)>::value, make_function(&function));

#define exportVariable(ordinal, variable) \
   addExport(#variable, ordinal, (void*)&variable);

#endif // KERNEL_LIBRARY_H
