// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_INTERNAL_PROPERTY_HPP
#define TAO_CONFIG_SCHEMA_INTERNAL_PROPERTY_HPP

#include "node.hpp"

namespace tao::config::schema::internal
{
   struct property : node
   {
      std::string m_key;
      std::shared_ptr< ref > m_schema;

      property( const value& v, node_map& m, const std::string& path )
         : node( v )
      {
         const auto& e = *v.get_object().begin();
         m_key = e.first;
         m_schema = std::make_shared< ref >( e.second, m, path );
      }

      void resolve( const node_map& m ) override
      {
         m_schema->resolve( m );
      }

      json::value validate( const value& v ) const override
      {
         if( auto e = object( m_source ).validate( v ) ) {
            return e;
         }
         const auto& o = v.get_object();
         const auto it = o.find( m_key );
         if( it == o.end() ) {
            return error( v, "missing property", { { "key", m_key } } );
         }
         return m_schema->validate( it->second );
      }
   };

}  // namespace tao::config::schema::internal

#endif
