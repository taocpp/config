// Copyright (c) 2020-2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_ENTRY_KIND_HPP
#define TAO_CONFIG_INTERNAL_ENTRY_KIND_HPP

#include <cassert>
#include <cstdlib>
#include <ostream>
#include <string_view>

namespace tao::config::internal
{
   enum class entry_kind : char
   {
      NULL_ = 0,
      BOOLEAN = 1,
      STRING = 2,
      BINARY = 3,
      SIGNED = 4,
      UNSIGNED = 5,
      DOUBLE = 6,
      ARRAY = 7,
      OBJECT = 8,
      ASTERISK = 9,
      REFERENCE = 10
   };

   [[nodiscard]] constexpr std::string_view to_string( const entry_kind k ) noexcept
   {
      switch( k ) {
         case entry_kind::NULL_:
            return "null";
         case entry_kind::BOOLEAN:
            return "boolean";
         case entry_kind::STRING:
            return "string";
         case entry_kind::BINARY:
            return "binary";
         case entry_kind::SIGNED:
            return "signed";
         case entry_kind::UNSIGNED:
            return "unsigned";
         case entry_kind::DOUBLE:
            return "double";
         case entry_kind::ARRAY:
            return "array";
         case entry_kind::OBJECT:
            return "object";
         case entry_kind::ASTERISK:
            return "asterisk";
         case entry_kind::REFERENCE:
            return "reference";
      }
      std::abort();
   }

   inline std::ostream& operator<<( std::ostream& o, const entry_kind k )
   {
      return o << to_string( k );
   }

}  // namespace tao::config::internal

#endif
