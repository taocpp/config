// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_INTERNAL_COMPARE_HPP
#define TAO_CONFIG_SCHEMA_INTERNAL_COMPARE_HPP

#include <functional>

#include "node.hpp"
#include "number.hpp"

namespace tao::config::schema::internal
{
   template< typename T >
   struct compare : node
   {
      explicit compare( const value& v )
         : node( v )
      {
         assert( m_source.is_number() );
      }

      json::value validate( const value& v ) const override
      {
         if( auto e = number( m_source ).validate( v ) ) {
            return e;
         }
         return T()( m_source, v ) ? ok() : error( v, "value did not match" );
      }
   };

   using minimum = compare< std::less_equal<> >;
   using maximum = compare< std::greater_equal<> >;
   using exclusive_minimum = compare< std::less<> >;
   using exclusive_maximum = compare< std::greater<> >;

}  // namespace tao::config::schema::internal

#endif
