// Copyright (c) 2018-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_ASSIGN_HPP
#define TAO_CONFIG_ASSIGN_HPP

#include <cassert>
#include <stdexcept>

#include "key.hpp"
#include "value.hpp"

#include "internal/json.hpp"
#include "internal/string_utility.hpp"

namespace tao::config
{
   template< template< typename... > class Traits >
   [[nodiscard]] json::basic_value< Traits >& assign( json::basic_value< Traits >& v, const key& k );

   template< template< typename... > class Traits >
   [[nodiscard]] json::basic_value< Traits >& assign( json::basic_value< Traits >& v, const std::string& k, const key& p )
   {
      if( !v.is_object() ) {
         throw std::runtime_error( internal::strcat( "attempt to index non-object with \"", k, "\"" ) );
      }
      const auto t = v.get_object().try_emplace( k, json::empty_object );  // TODO: Detect empty_array vs. empty_object?
      return assign( t.first->second, p );
   }

   template< template< typename... > class Traits >
   [[nodiscard]] json::basic_value< Traits >& assign( json::basic_value< Traits >& v, const std::size_t n, const key& p )
   {
      if( !v.is_array() ) {
         throw std::runtime_error( internal::strcat( "attempt to index non-array with ", n ) );
      }
      if( v.get_array().size() <= n ) {
         throw std::runtime_error( internal::strcat( "array index ", n, " out of bounds ", v.get_array().size() ) );
      }
      return assign( v.get_array()[ n ], p );
   }

   template< template< typename... > class Traits >
   [[nodiscard]] json::basic_value< Traits >& assign( json::basic_value< Traits >& v, const key& k )
   {
      if( k.empty() ) {
         return v;
      }
      switch( k[ 0 ].kind() ) {
         case key_kind::name:
            return assign( v, k[ 0 ].get_name(), pop_front( k ) );
         case key_kind::index:
            return assign( v, k[ 0 ].get_index(), pop_front( k ) );
      }
      throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
   }

}  // namespace tao::config

#endif
