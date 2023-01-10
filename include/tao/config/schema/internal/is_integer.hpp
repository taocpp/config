// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_INTERNAL_IS_INTEGER_HPP
#define TAO_CONFIG_SCHEMA_INTERNAL_IS_INTEGER_HPP

#include <cmath>

#include "node.hpp"
#include "number.hpp"

namespace tao::config::schema::internal
{
   struct is_integer : node
   {
      using node::node;

      json::value validate( const value& v ) const override
      {
         if( auto e = number( m_source ).validate( v ) ) {
            return e;
         }
         if( v.is_double() ) {
            const double d = v.as< double >();
            double dummy;
            if( std::modf( d, &dummy ) != 0 ) {
               return error( v, "expected integer", { { "value", d } } );
            }
         }
         return ok();
      }
   };

}  // namespace tao::config::schema::internal

#endif
