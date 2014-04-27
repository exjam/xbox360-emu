#ifndef BIT_CAST_H
#define BIT_CAST_H

// reinterpret_cast for value types
template<typename SrcType, typename DstType>
static inline DstType
bit_cast(SrcType src)
{
   union
   {
      SrcType src;
      DstType dst;
   } type_caster;

   static_assert(sizeof(SrcType) == sizeof(DstType),
                 "bit_cast must be between same sized types");

   type_caster.src = src;
   return type_caster.dst;
}

#endif // BIT_CAST_H
