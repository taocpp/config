// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_INTERNAL_PROPERTY_NAMES_HPP
#define TAO_CONFIG_SCHEMA_INTERNAL_PROPERTY_NAMES_HPP

#include "ref.hpp"

namespace tao::config::schema::internal
{
   struct property_names : ref
   {
      using ref::ref;

      json::value validate( const value& v ) const override
      {
         if( auto e = object( m_source ).validate( v ) ) {
            return e;
         }
         for( const auto& e : v.get_object() ) {
            if( ref::validate( value( e.first ) ) ) {
               return error( v.at( e.first ), "invalid key" );
            }
         }
         return ok();
      }
   };

}  // namespace tao::config::schema::internal

#endif
