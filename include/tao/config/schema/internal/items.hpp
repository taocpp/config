// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_INTERNAL_ITEMS_HPP
#define TAO_CONFIG_SCHEMA_INTERNAL_ITEMS_HPP

#include "array.hpp"
#include "ref.hpp"

namespace tao::config::schema::internal
{
   struct items : ref
   {
      using ref::ref;

      json::value validate( const value& v ) const override
      {
         if( auto e = array( m_source ).validate( v ) ) {
            return e;
         }
         for( const auto& e : v.get_array() ) {
            if( const auto t = ref::validate( e ) ) {
               return t;
            }
         }
         return ok();
      }
   };

}  // namespace tao::config::schema::internal

#endif
