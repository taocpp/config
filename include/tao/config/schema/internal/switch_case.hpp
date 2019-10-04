// Copyright (c) 2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_INTERNAL_SWITCH_CASE_HPP
#define TAO_CONFIG_SCHEMA_INTERNAL_SWITCH_CASE_HPP

#include "iless.hpp"
#include "node.hpp"
#include "ref.hpp"

namespace tao::config::schema::internal
{
   template< typename Compare >
   struct switch_string : node
   {
      std::string_view m_key;
      std::map< std::string_view, std::unique_ptr< node >, Compare > m_cases;
      std::unique_ptr< node > m_default;

      switch_string( const value& v, node_map& m, const std::string& path )
         : node( v )
      {
         const auto s = m_source.get_object().begin();
         m_key = s->first;
         if( const auto& c = internal::find( s->second, "case" ) ) {
            for( const auto& p : c.get_object() ) {
               m_cases.emplace( p.first, std::make_unique< ref >( p.second, m, path ) );
            }
         }
         if( const auto& d = internal::find( s->second, "default" ) ) {
            if( d != false ) {
               m_default = std::make_unique< ref >( d, m, path );
            }
         }
      }

      void resolve( const node_map& m ) override
      {
         for( const auto& p : m_cases ) {
            p.second->resolve( m );
         }
         if( m_default ) {
            m_default->resolve( m );
         }
      }

      json::value validate( const value& v ) const override
      {
         if( auto e = object( m_source ).validate( v ) ) {
            return e;
         }
         const auto& o = v.unsafe_get_object();
         const auto it = o.find( m_key );
         if( it != o.end() ) {
            if( auto e = string( m_source ).validate( it->second ) ) {
               return e;
            }
            const auto k = it->second.as< std::string_view >();
            const auto jt = m_cases.find( k );
            if( jt != m_cases.end() ) {
               return jt->second->validate( v );
            }
            if( m_default ) {
               return m_default->validate( v );
            }
            std::vector< std::string_view > keys;
            for( const auto& e : m_cases ) {
               keys.push_back( e.first );
            }
            return error( it->second, "invalid property value", { { "value", k }, { "valid", keys } } );
         }
         return error( v, "missing property", { { "key", m_key } } );
      }
   };

   struct switch_case : node
   {
      std::unique_ptr< node > m_impl;

      switch_case( const value& v, node_map& m, const std::string& path )
         : node( v )
      {
         const auto s = m_source.get_object().begin();
         if( s->first == "type" ) {
            // TODO: Implement me!
            throw 42;
         }
         else if( s->first == "string" ) {
            m_impl = std::make_unique< switch_string< std::less< std::string_view > > >( s->second, m, path );
         }
         else if( s->first == "istring" ) {
            m_impl = std::make_unique< switch_string< iless > >( s->second, m, path );
         }
         else {
            // TODO: Throw proper exception
            throw 42;
         }
      }

      void resolve( const node_map& m ) override
      {
         m_impl->resolve( m );
      }

      json::value validate( const value& v ) const override
      {
         return m_impl->validate( v );
      }

      json::value pos() const override
      {
         return m_impl->pos();
      }
   };

}  // namespace tao::config::schema::internal

#endif
