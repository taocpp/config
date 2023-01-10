// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_INTERNAL_ONE_OF_HPP
#define TAO_CONFIG_SCHEMA_INTERNAL_ONE_OF_HPP

#include "container.hpp"

namespace tao::config::schema::internal
{
   struct one_of : container
   {
      using container::container;

      json::value validate( const value& v ) const override
      {
         const auto& vs = v.skip_value_ptr();

         json::value errors = json::empty_array;
         std::vector< node* > matched;
         for( const auto& p : m_properties ) {
            if( auto e = p->validate( vs ) ) {
               errors.emplace_back( std::move( e ) );
            }
            else {
               matched.emplace_back( p.get() );
            }
         }
         if( matched.size() == 1 ) {
            return ok();
         }
         if( matched.empty() ) {
            // TODO: Pick "best" error
            return error( v, "no match", { { "errors", std::move( errors ) } } );
         }
         json::value data = json::empty_array;
         for( const auto& e : matched ) {
            data.emplace_back( e->pos() );
         }
         return error( v, "multiple matches found", { { "matched", std::move( data ) } } );
      }
   };

}  // namespace tao::config::schema::internal

#endif
