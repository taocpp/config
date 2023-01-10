// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_INTERNAL_ARRAY_HPP
#define TAO_CONFIG_SCHEMA_INTERNAL_ARRAY_HPP

#include "node.hpp"

namespace tao::config::schema::internal
{
   struct array : node
   {
      using node::node;

      json::value validate( const value& v ) const override
      {
         return v.is_array() ? ok() : error( v, "expected array" );
      }
   };

}  // namespace tao::config::schema::internal

#endif
