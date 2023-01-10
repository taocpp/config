// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_VALIDATOR_HPP
#define TAO_CONFIG_SCHEMA_VALIDATOR_HPP

#include "internal/node.hpp"
#include "internal/schema.hpp"

#include "builtin.hpp"

namespace tao::config::schema
{
   struct validator
   {
      internal::node_map m_nodes;

      explicit validator( const value& v, const builtin& b = builtin() )
         : m_nodes( b.m_nodes )
      {
         m_nodes.emplace( "", std::make_shared< internal::schema >( v, m_nodes ) );

         for( const auto& e : m_nodes ) {
            e.second->resolve( m_nodes );
         }
      }

      json::value validate( const value& v ) const
      {
         return m_nodes.at( "" )->validate( v );
      }
   };

}  // namespace tao::config::schema

#endif
