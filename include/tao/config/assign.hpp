// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_ASSIGN_HPP
#define TAO_CONFIG_ASSIGN_HPP

#include <cassert>
#include <stdexcept>

#include "key.hpp"
#include "value.hpp"

#include "external/json.hpp"
#include "internal/format.hpp"

namespace tao::config
{
   template< template< typename... > class Traits >
   json::basic_value< Traits >& assign( json::basic_value< Traits >& v, const key& k );

   template< template< typename... > class Traits >
   json::basic_value< Traits >& assign( json::basic_value< Traits >& v, const std::string& k, const key& p )
   {
      if( !v.is_object() ) {
         throw std::runtime_error( internal::format( __FILE__, __LINE__, "attempt to index non-object with string", { &v.key, &v.position, { "string", k } } ) );
      }
      const auto t = v.get_object().try_emplace( k, json::empty_object );
      return assign( t.first->second, p );
   }

   template< template< typename... > class Traits >
   json::basic_value< Traits >& assign( json::basic_value< Traits >& v, const std::uint64_t n, const key& p )
   {
      if( !v.is_array() ) {
         throw std::runtime_error( internal::format( __FILE__, __LINE__, "attempt to index non-array with integer", { &v.key, &v.position, { "integer", n } } ) );
      }
      if( v.get_array().size() <= n ) {
         throw std::runtime_error( internal::format( __FILE__, __LINE__, "array index out of bounds", { &v.key, &v.position, { "integer", n } } ) );
      }
      return assign( v.get_array()[ n ], p );
   }

   template< template< typename... > class Traits >
   json::basic_value< Traits >& assign( json::basic_value< Traits >& v, const key& k )
   {
      if( k.empty() ) {
         return v;
      }
      switch( k[ 0 ].type() ) {
         case part::name:
            return assign( v, k[ 0 ].get_name(), pop_front( k ) );
         case part::index:
            return assign( v, k[ 0 ].get_index(), pop_front( k ) );
         case part::star:
            throw std::runtime_error( internal::format( __FILE__, __LINE__, "attempt to assign to star", { &v.key, &v.position } ) );
         case part::minus:
            throw std::runtime_error( internal::format( __FILE__, __LINE__, "attempt to assign to minus", { &v.key, &v.position } ) );
      }
      assert( false );
   }

}  // namespace tao::config

#endif
