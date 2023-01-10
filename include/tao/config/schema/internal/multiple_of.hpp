// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_INTERNAL_MULTIPLE_OF_HPP
#define TAO_CONFIG_SCHEMA_INTERNAL_MULTIPLE_OF_HPP

#include <cmath>
#include <limits>

#include "node.hpp"
#include "number.hpp"

namespace tao::config::schema::internal
{
   struct multiple_of : node
   {
      explicit multiple_of( const value& v )
         : node( v )
      {
         assert( m_source.is_number() );
         assert( m_source > 0 );
      }

      json::value validate( const value& v ) const override
      {
         if( auto e = number( m_source ).validate( v ) ) {
            return e;
         }
         // TODO: enhance for large integer values (uint64_t, ...)
         const auto x = v.as< double >();
         const auto d = m_source.as< double >();
         const auto r = std::fmod( x, d );
         if( std::fabs( r ) < std::numeric_limits< double >::epsilon() ) {
            return ok();
         }
         if( std::fabs( r - d ) < std::numeric_limits< double >::epsilon() ) {
            return ok();
         }
         return error( v, "value is not a multiple of given constant", { { "value", x }, { "multiplier", d } } );
      }
   };

}  // namespace tao::config::schema::internal

#endif
