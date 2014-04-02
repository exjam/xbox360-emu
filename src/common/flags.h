#ifndef FLAGS_H
#define FLAGS_H

template<typename Enum>
class Flags
{
private:
   int m_value;

public:
   Flags() :
      m_value(0)
   {
   }

   Flags(Enum f) :
      m_value(static_cast<int>(f))
   {
   }

   Flags & operator&=(Flags f)
   {
      m_value &= f.m_value;
      return *this;
   }

   Flags & operator|=(Flags f)
   {
      m_value |= f.m_value;
      return *this;
   }

   Flags & operator^=(Flags f)
   {
      m_value ^= f.m_value;
      return *this;
   }

   Flags & operator&=(Enum f)
   {
      m_value &= static_cast<int>(f);
      return *this;
   }

   Flags & operator|=(Enum f)
   {
      m_value |= static_cast<int>(f);
      return *this;
   }

   Flags & operator^=(Enum f)
   {
      m_value ^= static_cast<int>(f);
      return *this;
   }
 
   inline bool operator!() const
   {
      return !m_value;
   }

   inline operator int() const
   {
      return m_value;
   }
 
   inline Flags operator~ () const
   {
      return Flags(Enum(~m_value));
   }

   inline Flags operator|(Flags f) const
   {
      return Flags(Enum(m_value | f.m_value));
   }

   inline Flags operator^(Flags f) const
   {
      return Flags(Enum(m_value ^ f.m_value));
   }

   inline Flags operator&(Flags f) const
   {
      return Flags(Enum(m_value & f.m_value));
   }

   inline Flags operator|(Enum f) const
   {
      return Flags(Enum(m_value | static_cast<int>(f)));
   }

   inline Flags operator^(Enum f) const
   {
      return Flags(Enum(m_value ^ static_cast<int>(f)));
   }

   inline Flags operator&(Enum f) const
   {
      return Flags(Enum(m_value & static_cast<int>(f)));
   }
};

#endif
