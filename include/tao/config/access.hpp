// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_ACCESS_HPP
#define TAO_CONFIG_ACCESS_HPP

#include <cassert>
#include <stdexcept>

#include "key.hpp"
#include "value.hpp"

#include "external/json.hpp"
#include "internal/format.hpp"

namespace tao::config
{
   template< template< typename... > class Traits >
   const json::basic_value< Traits >& access( const json::basic_value< Traits >& v, const key& k );

   template< template< typename... > class Traits >
   const json::basic_value< Traits >& access_name( const json::basic_value< Traits >& v, const std::string& k, const key& p )
   {
      if( !v.is_object() ) {
         throw std::runtime_error( internal::format( __FILE__, __LINE__, "attempt to index non-object with string", { /*&v.key*/ &v.position, /*{ "value", &v },*/ { "string", k }, { "remaining", &p } } ) );
      }
      const auto j = v.get_object().find( k );

      if( j == v.get_object().end() ) {
         throw std::runtime_error( internal::format( __FILE__, __LINE__, "object string index not found", { /*&v.key,*/ &v.position, { "string", k } } ) );
      }
      return access( j->second, p );
   }

   template< template< typename... > class Traits >
   const json::basic_value< Traits >& access_index( const json::basic_value< Traits >& v, const std::uint64_t n, const key& p )
   {
      if( !v.is_array() ) {
         throw std::runtime_error( internal::format( __FILE__, __LINE__, "attempt to index non-array with integer", { /*&v.key,*/ &v.position, { "integer", n } } ) );
      }
      if( v.get_array().size() <= n ) {
         throw std::runtime_error( internal::format( __FILE__, __LINE__, "array index out of bounds", { /*&v.key,*/ &v.position, { "integer", n } } ) );
      }
      return access( v.get_array()[ n ], p );
   }

   template< template< typename... > class Traits >
   const json::basic_value< Traits >& access_minus( const json::basic_value< Traits >& v, const key& p )
   {
      if( !v.is_array() ) {
         throw std::runtime_error( internal::format( __FILE__, __LINE__, "attempt to access last element in non-array", { /*&v.key,*/ &v.position } ) );
      }
      if( v.get_array().empty() ) {
         throw std::runtime_error( internal::format( __FILE__, __LINE__, "array is empty", { /*&v.key,*/ &v.position } ) );
      }
      return access( v.get_array().back(), p );
   }

   template< template< typename... > class Traits >
   const json::basic_value< Traits >& access( const json::basic_value< Traits >& v, const key& k )
   {
      if( k.empty() ) {
         return v;
      }
      switch( k[ 0 ].type() ) {
         case part::name:
            return access_name( v, k[ 0 ].get_name(), pop_front( k ) );
         case part::index:
            return access_index( v, k[ 0 ].get_index(), pop_front( k ) );
         case part::star:
            throw std::runtime_error( internal::format( __FILE__, __LINE__, "attempt to access star", { /*&v.key,*/ &v.position } ) );
         case part::minus:
            return access_minus( v, pop_front( k ) );
      }
      assert( false );
   }

}  // namespace tao::config

#endif
