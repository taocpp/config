// Copyright (c) 2019-2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_INTERNAL_BOOLEAN_HPP
#define TAO_CONFIG_SCHEMA_INTERNAL_BOOLEAN_HPP

#include "node.hpp"

namespace tao::config::schema::internal
{
   struct boolean : node
   {
      using node::node;

      json::value validate( const value& v ) const override
      {
         return v.is_boolean() ? ok() : error( v, "expected boolean" );
      }
   };

}  // namespace tao::config::schema::internal

#endif
