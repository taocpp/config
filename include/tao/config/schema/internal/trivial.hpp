// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_INTERNAL_TRIVIAL_HPP
#define TAO_CONFIG_SCHEMA_INTERNAL_TRIVIAL_HPP

#include "node.hpp"

namespace tao::config::schema::internal
{
   template< bool B >
   struct trivial : node
   {
      using node::node;

      json::value validate( const value& v ) const override
      {
         return B ? ok() : error( v, "always fails" );
      }
   };

}  // namespace tao::config::schema::internal

#endif
