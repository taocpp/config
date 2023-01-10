// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_INTERNAL_IF_THEN_ELSE_HPP
#define TAO_CONFIG_SCHEMA_INTERNAL_IF_THEN_ELSE_HPP

#include "node.hpp"

namespace tao::config::schema::internal
{
   struct if_then_else : ref
   {
      std::shared_ptr< ref > m_then;
      std::shared_ptr< ref > m_else;

      using ref::ref;

      void resolve( const node_map& m ) override
      {
         ref::resolve( m );
         if( m_then ) {
            m_then->resolve( m );
         }
         if( m_else ) {
            m_else->resolve( m );
         }
      }

      json::value validate( const value& v ) const override
      {
         if( !ref::validate( v ) ) {
            if( m_then ) {
               return m_then->validate( v );
            }
         }
         else {
            if( m_else ) {
               return m_else->validate( v );
            }
         }
         return ok();
      }
   };

}  // namespace tao::config::schema::internal

#endif
