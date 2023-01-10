// Copyright (c) 2018-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_ACCESS_HPP
#define TAO_CONFIG_ACCESS_HPP

#include <cassert>
#include <stdexcept>

#include "key.hpp"
#include "value.hpp"

#include "internal/json.hpp"
#include "internal/string_utility.hpp"

namespace tao::config
{
   template< template< typename... > class Traits >
   [[nodiscard]] const json::basic_value< Traits >& access( const json::basic_value< Traits >& v, const key& k );

   template< template< typename... > class Traits >
   [[nodiscard]] const json::basic_value< Traits >& access_name( const json::basic_value< Traits >& v, const std::string& k, const key& p )
   {
      if( !v.is_object() ) {
         throw std::runtime_error( internal::strcat( "attempt to index non-object with \"", k, "\"" ) );
      }
      const auto j = v.get_object().find( k );

      if( j == v.get_object().end() ) {
         throw std::runtime_error( internal::strcat( "object index \"", k, "\" not found" ) );
      }
      return access( j->second, p );
   }

   template< template< typename... > class Traits >
   [[nodiscard]] const json::basic_value< Traits >& access_index( const json::basic_value< Traits >& v, const std::size_t n, const key& p )
   {
      if( !v.is_array() ) {
         throw std::runtime_error( internal::strcat( "attempt to index non-array with ", n ) );
      }
      if( v.get_array().size() <= n ) {
         throw std::runtime_error( internal::strcat( "array index ", n, " out of bounds ", v.get_array().size() ) );
      }
      return access( v.get_array()[ n ], p );
   }

   template< template< typename... > class Traits >
   [[nodiscard]] const json::basic_value< Traits >& access( const json::basic_value< Traits >& v, const key& k )
   {
      if( k.empty() ) {
         return v;
      }
      switch( k[ 0 ].kind() ) {
         case key_kind::name:
            return access_name( v, k[ 0 ].get_name(), pop_front( k ) );
         case key_kind::index:
            return access_index( v, k[ 0 ].get_index(), pop_front( k ) );
      }
      throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
   }

}  // namespace tao::config

#endif
