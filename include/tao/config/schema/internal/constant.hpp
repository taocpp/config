// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_INTERNAL_CONSTANT_HPP
#define TAO_CONFIG_SCHEMA_INTERNAL_CONSTANT_HPP

#include "ref.hpp"

namespace tao::config::schema::internal
{
   struct constant : node
   {
      using node::node;

      json::value validate( const value& v ) const override
      {
         return ( v == m_source ) ? ok() : error( v, "value did not match" );
      }
   };

}  // namespace tao::config::schema::internal

#endif
