// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_ACCESS_HPP
#define TAO_CONFIG_ACCESS_HPP

#include <cassert>
#include <stdexcept>

#include "key.hpp"
#include "value.hpp"

#include "external/json.hpp"
#include "internal/format.hpp"

namespace tao
{
   namespace config
   {
      template< template< typename... > class Traits >
      const json::basic_value< Traits >& access( const json::basic_value< Traits >& v, const key& k );

      template< template< typename... > class Traits >
      const json::basic_value< Traits >& access( const json::basic_value< Traits >& v, const std::string& k, const key& p )
      {
         if( !v.is_object() ) {
            throw std::runtime_error( internal::format( "attempt to index non-object with string", { &v.key, &v.position, { "string", k } } ) );
         }
         const auto j = v.unsafe_get_object().find( k );

         if( j == v.unsafe_get_object().end() ) {
            throw std::runtime_error( internal::format( "object string index not found", { &v.key, &v.position, { "string", k } } ) );
         }
         return access( j->second, p );
      }

      template< template< typename... > class Traits >
      const json::basic_value< Traits >& access( const json::basic_value< Traits >& v, const std::uint64_t n, const key& p )
      {
         if( !v.is_array() ) {
            throw std::runtime_error( internal::format( "attempt to index non-array with integer", { &v.key, &v.position, { "integer", n } } ) );
         }
         if( v.unsafe_get_array().size() <= n ) {
            throw std::runtime_error( internal::format( "array index out of bounds", { &v.key, &v.position, { "integer", n } } ) );
         }
         return access( v.unsafe_get_array()[ n ], p );
      }

      template< template< typename... > class Traits >
      const json::basic_value< Traits >& access( const json::basic_value< Traits >& v, const key& k )
      {
         if( k.empty() ) {
            return v;
         }
         switch( k[ 0 ].type() ) {
            case part::name:
               return access( v, k[ 0 ].get_name(), pop_front( k ) );
            case part::index:
               return access( v, k[ 0 ].get_index(), pop_front( k ) );
            case part::star:
               throw std::runtime_error( internal::format( "attempt to access star", { &v.key, &v.position } ) );
            case part::minus:
               throw std::runtime_error( internal::format( "attempt to access minus", { &v.key, &v.position } ) );
         }
         assert( false );
      }

   }  // namespace config

}  // namespace tao

#endif
