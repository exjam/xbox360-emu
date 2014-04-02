#ifndef PRS_OPTIONAL_H
#define PRS_OPTIONAL_H

#include <memory>

/**
 * This is a bare bones hacky version of the unreleased std::optional.
 *
 * A major difference is that we only allocate memory when optional is in use,
 * the TR std::optional and boost::optional uses static std::aligned_storage.
 */

namespace prs
{

template<typename Type>
class optional {
public:
   using value_type = Type;

public:
   optional()
   {
   }

   optional(const Type &value)
   {
      *this = value;
   }

   optional(const optional<Type> &value) :
      m_value(value.m_value)
   {
   }

   ~optional()
   {
   }

   optional<Type> & operator=(const optional<Type> &value)
   {
      m_value = value.m_value;
      return *this;
   }

   optional<Type> & operator=(const Type &value)
   {
      m_value = std::shared_ptr<Type>(new Type(value));
      return *this;
   }

   Type * operator ->() const
   {
      return m_value.operator ->();
   }

   Type& operator *() const
   {
      return m_value.operator *();
   }

   operator bool() const
   {
      return m_value.operator bool();
   }

private:
   std::shared_ptr<Type> m_value;
};

template<typename Type>
inline optional<Type> make_optional(const Type &t)
{
   return optional<Type> { t };
}

} // namespace prs

#endif // ifdef PRS_OPTIONAL_H
