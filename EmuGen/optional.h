#ifndef STD_OPTIONAL_H
#define STD_OPTIONAL_H

#include <memory>

/* Awful shitty hack implementation of std::optional, C++14 pls */
namespace std {
   template<typename Type>
   class optional {
   public:
      optional() :
         m_init(false)
      {
      }

      ~optional()
      {
      }

      optional<Type> & operator=(const optional<Type> &value)
      {
         m_value = value.m_value;
         m_init = value.m_init;
         return *this;
      }

      optional<Type> & operator=(const Type &value)
      {
         m_value = std::shared_ptr<Type>(new Type(value));
         m_init = true;
         return *this;
      }

      Type* operator->()
      {
         return m_value.operator->();
      }

      Type& operator*()
      {
         return m_value.operator*();
      }

      operator bool()
      {
         return m_init;
      }

   private:
      bool m_init;
      std::shared_ptr<Type> m_value;
   };
};

#endif // ifdef STD_OPTIONAL_H
