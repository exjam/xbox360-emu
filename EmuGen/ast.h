#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <utility>
#include "optional.h"

struct ast_symbol {
   std::string value;

   template<typename Type>
   void construct(Type& input)
   {
      value = ast_to_string<decltype(value)>(input);
   }
};

struct ast_string {
   std::string value;

   template<typename Type>
   void construct(Type& input)
   {
      value = ast_to_string<decltype(value)>(input.first.second);
   }
};

struct ast_number {
   ast_number() : value(0) {}
   ast_number(int value) : value(value) {}

   int value;

   template<typename Char>
   void construct(std::vector<Char>& chs)
   {
      std::string word;

      for (auto ch : chs) {
         word.push_back(ch);
      }

      value = std::stoi(word);
   }
};

struct ast_char {
   int ch;

   template<typename Sequence>
   void construct(Sequence& ps)
   {
      ch = ps.first.second;
   }
};

struct ast_bitrange {
   ast_bitrange() : end(-1) {}

   ast_number start;
   ast_number end;

   int size()
   {
      return 1 + end.value - start.value;
   }

   template<typename Sequence>
   void construct(Sequence& ps)
   {
      start = ps.first.value;

      if (ps.second) {
         end = ps.second->second.value;
      } else {
         end.value = start.value;
      }
   }
};

struct ast_cat_opcd {
   ast_symbol primary;
   std::vector<std::pair<ast_number, ast_symbol>> secondary;

   template<typename Sequence>
   void construct(Sequence& ps)
   {
      primary = ps.first.first.second;

      for (auto p : ps.first.second) {
         secondary.push_back(std::make_pair(p.first.first.second, p.second));
      }
   }
};

struct ast_arch_endian {
   std::string endian;

   template<typename Sequence>
   void construct(Sequence& ps)
   {
      endian = ps.second;
   }
};

struct ast_bitfield {
   std::vector<std::pair<ast_symbol, ast_bitrange>> bitfield;

   template<typename Sequence>
   void construct(Sequence& ps)
   {
      for (auto bf : ps.first.second) {
         bitfield.push_back(std::make_pair(bf.first.first, bf.second));
      }
   }
};

struct ast_array {
   ast_number size;

   template<typename Sequence>
   void construct(Sequence& ps)
   {
      size = ps.first.second;
   }
};

struct ast_reg_type {
   ast_symbol type;
   ast_array array;
   ast_bitfield bitfield;

   template<typename Sequence>
   void construct(Sequence& ps)
   {
      type = ps.first;

      if (ps.second) {
         if (ps.second->first) {
            array = *ps.second->first;
         }

         if (ps.second->second) {
            bitfield = *ps.second->second;
         }
      }
   }
};

struct ast_reg_define {
   ast_symbol name;
   ast_reg_type type;

   template<typename Sequence>
   void construct(Sequence& ps)
   {
      name = ps.first.first;
      type = ps.second;
   }
};

struct ast_insf_extra {
   ast_symbol name;
   std::optional<ast_number> number;
   std::optional<ast_string> string;
   std::optional<ast_char> chr;

   template<typename Sequence>
   void construct(Sequence& ps)
   {
      name = ps.first.first;

      if (ps.second.first && ps.second.first->first) {
         number = *ps.second.first->first;
      } else if (ps.second.first && ps.second.first->second) {
         string = *ps.second.first->second;
      } else if (ps.second.second) {
         chr = *ps.second.second;
      }
   }
};

struct ast_insf_field {
   ast_symbol name;
   ast_bitrange bitrange;
   std::vector<ast_insf_extra> extras;

   template<typename Sequence>
   void construct(Sequence& ps)
   {
      name = ps.first.first.first;
      bitrange = ps.first.second;

      if (ps.second) {
         extras.push_back(ps.second->first.second);

         for (auto es : ps.second->second) {
            extras.push_back(es.second);
         }
      }
   }
};

struct ast_opcd_disasm {
   ast_symbol name;
   std::vector<ast_symbol> operands;

   template<typename Sequence>
   void construct(Sequence& ps)
   {
      name = ps.first;

      if (ps.second) {
         operands.push_back(ps.second->first);

         for (auto ops : ps.second->second) {
            operands.push_back(ops.second);
         }
      }
   }
};

struct ast_opcd_extra {
   ast_symbol name;
   std::optional<ast_number> number;
   std::optional<ast_string> string;
   std::optional<ast_char> chr;

   template<typename Sequence>
   void construct(Sequence& ps)
   {
      name = ps.first;

      if (ps.second) {
         if (ps.second->second.first && ps.second->second.first->first) {
            number = *ps.second->second.first->first;
         } else if (ps.second->second.first && ps.second->second.first->second) {
            string = *ps.second->second.first->second;
         } else if (ps.second->second.second) {
            chr = *ps.second->second.second;
         }
      }
   }
};

struct ast_opcd_def {
   ast_number id;
   ast_opcd_disasm disasm;
   std::vector<ast_opcd_extra> extras;

   template<typename Sequence>
   void construct(Sequence& ps)
   {
      id = ps.first.first.first;
      disasm = ps.first.second;

      if (ps.second) {
         extras.push_back(ps.second->first.second);

         for (auto es : ps.second->second) {
            extras.push_back(es.second);
         }
      }
   }
};

struct ast_arch {
   enum Endian {
      Invalid,
      BigEndian,
      LittleEndian
   };

   Endian endian;

   template<typename Sequence>
   void construct(Sequence& ps)
   {
      if (ps.endian.compare("BigEndian") == 0) {
         endian = BigEndian;
      } else if (ps.endian.compare("LittleEndian") == 0) {
         endian = LittleEndian;
      } else {
         endian = Invalid;
      }
   }
};

struct ast_reg {
   std::vector<ast_reg_define> registers;

   template<typename Sequence>
   void construct(Sequence& ps)
   {
      registers = ps;
   }
};

struct ast_insf {
   std::vector<ast_insf_field> fields;

   template<typename Sequence>
   void construct(Sequence& ps)
   {
      fields = ps;
   }
};

struct ast_opcd {
   ast_cat_opcd declaration;
   std::vector<ast_opcd_def> opcodes;

   template<typename Sequence>
   void construct(Sequence& ps)
   {
      declaration = ps.first;
      opcodes = ps.second;
   }
};

struct ast_root {
   ast_arch arch;
   ast_reg reg;
   ast_insf insf;
   std::vector<ast_opcd> opcodes;

   template<typename Sequence>
   void construct(Sequence& ps)
   {
      for (auto p : ps) {
         if (p.first) {
            if (p.first->first) {
               if (p.first->first->first) {
                  arch = p.first->first->first->second;
               } else if (p.first->first->second) {
                  reg = p.first->first->second->second;
               }
            } else if (p.first->second) {
               insf = p.first->second->second;
            }
         }

         if (p.second) {
            opcodes.push_back(*p.second);
         }
      }
   }
};

#endif // ifdef AST_H
