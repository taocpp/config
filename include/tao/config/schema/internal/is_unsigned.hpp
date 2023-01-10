// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_INTERNAL_IS_UNSIGNED_HPP
#define TAO_CONFIG_SCHEMA_INTERNAL_IS_UNSIGNED_HPP

#include "is_integer.hpp"

namespace tao::config::schema::internal
{
   struct is_unsigned : is_integer
   {
      using is_integer::is_integer;

      json::value validate( const value& v ) const override
      {
         if( auto e = is_integer::validate( v ) ) {
            return e;
         }
         return ( v >= 0 ) ? ok() : error( v, "expected positive integer" );
      }
   };

}  // namespace tao::config::schema::internal

#endif
