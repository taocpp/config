// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_INTERNAL_ANY_OF_HPP
#define TAO_CONFIG_SCHEMA_INTERNAL_ANY_OF_HPP

#include "container.hpp"

namespace tao::config::schema::internal
{
   struct any_of : container
   {
      using container::container;

      json::value validate( const value& v ) const override
      {
         const auto& vs = v.skip_value_ptr();

         json::value errors = json::empty_array;
         for( const auto& p : m_properties ) {
            if( auto e = p->validate( vs ) ) {
               errors.emplace_back( std::move( e ) );
            }
            else {
               // short-circuit
               return ok();
            }
         }
         // TODO: Pick "best" error
         return error( v, "no match", { { "errors", std::move( errors ) } } );
      }
   };

}  // namespace tao::config::schema::internal

#endif
