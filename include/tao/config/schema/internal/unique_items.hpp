// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_INTERNAL_UNIQUE_ITEMS_HPP
#define TAO_CONFIG_SCHEMA_INTERNAL_UNIQUE_ITEMS_HPP

#include "node.hpp"

namespace tao::config::schema::internal
{
   struct unique_items : node
   {
      using node::node;

      json::value validate( const value& v ) const override
      {
         if( auto e = array( m_source ).validate( v ) ) {
            return e;
         }
         std::set< value > s;
         for( const auto& e : v.get_array() ) {
            if( !s.emplace( &e ).second ) {
               return error( v, "duplicate items detected" );  // TODO: Add indices/positions/...?
            }
         }
         return ok();
      }
   };

}  // namespace tao::config::schema::internal

#endif
