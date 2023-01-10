// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_INTERNAL_ALL_OF_HPP
#define TAO_CONFIG_SCHEMA_INTERNAL_ALL_OF_HPP

#include "container.hpp"

namespace tao::config::schema::internal
{
   struct all_of : container
   {
      using container::container;

      json::value validate( const value& v ) const override
      {
         const auto& vs = v.skip_value_ptr();
         for( const auto& p : m_properties ) {
            if( auto e = p->validate( vs ) ) {
               // short-circuit
               return e;
            }
         }
         return ok();
      }
   };

}  // namespace tao::config::schema::internal

#endif
