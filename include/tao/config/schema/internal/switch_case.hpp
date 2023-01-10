// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_INTERNAL_SWITCH_CASE_HPP
#define TAO_CONFIG_SCHEMA_INTERNAL_SWITCH_CASE_HPP

#include "iless.hpp"
#include "node.hpp"
#include "ref.hpp"

namespace tao::config::schema::internal
{
   struct switch_type : node
   {
      std::map< std::string_view, std::shared_ptr< node > > m_cases;
      std::shared_ptr< node > m_default;

      switch_type( const value& v, node_map& m, const std::string& path )
         : node( v )
      {
         if( const auto& c = internal::find( m_source, "case" ) ) {
            for( const auto& p : c.get_object() ) {
               m_cases.emplace( p.first, std::make_shared< ref >( p.second, m, path ) );
            }
         }
         if( const auto& d = internal::find( m_source, "default" ) ) {
            if( d != false ) {
               m_default = std::make_shared< ref >( d, m, path );
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
         std::string_view key;
         switch( v.skip_value_ptr().type() ) {
            case json::type::NULL_:
               key = "null";
               break;
            case json::type::BOOLEAN:
               key = "boolean";
               break;
            case json::type::SIGNED:
            case json::type::UNSIGNED:
            case json::type::DOUBLE:
               key = "number";
               break;
            case json::type::STRING:
            case json::type::STRING_VIEW:
               key = "string";
               break;
            case json::type::BINARY:
            case json::type::BINARY_VIEW:
               key = "binary";
               break;
            case json::type::ARRAY:
               key = "array";
               break;
            case json::type::OBJECT:
               key = "object";
               break;
            default:
               return error( v, "invalid type", { { "type", json::to_string( v.type() ) } } );
         }
         const auto it = m_cases.find( key );
         if( it != m_cases.end() ) {
            return it->second->validate( v );
         }
         if( m_default ) {
            return m_default->validate( v );
         }
         return error( v, "invalid type", { { "type", json::to_string( v.type() ) } } );
      }
   };

   template< typename Compare >
   struct switch_string : node
   {
      std::string_view m_key;
      std::map< std::string_view, std::shared_ptr< node >, Compare > m_cases;
      std::shared_ptr< node > m_default;

      switch_string( const value& v, node_map& m, const std::string& path )
         : node( v )
      {
         const auto s = m_source.get_object().begin();
         m_key = s->first;
         if( const auto& c = internal::find( s->second, "case" ) ) {
            for( const auto& p : c.get_object() ) {
               m_cases.emplace( p.first, std::make_shared< ref >( p.second, m, path ) );
            }
         }
         if( const auto& d = internal::find( s->second, "default" ) ) {
            if( d != false ) {
               m_default = std::make_shared< ref >( d, m, path );
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
         const auto& o = v.get_object();
         const auto it = o.find( m_key );
         if( it != o.end() ) {
            if( auto e = string( m_source ).validate( it->second ) ) {
               return e;
            }
            const auto k = it->second.template as< std::string_view >();
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
      std::shared_ptr< node > m_impl;

      switch_case( const value& v, node_map& m, const std::string& path )
         : node( v )
      {
         const auto s = m_source.get_object().begin();
         if( s->first == "type" ) {
            m_impl = std::make_shared< switch_type >( s->second, m, path );
         }
         else if( s->first == "string" ) {
            m_impl = std::make_shared< switch_string< std::less< std::string_view > > >( s->second, m, path );
         }
         else if( s->first == "istring" ) {
            m_impl = std::make_shared< switch_string< iless > >( s->second, m, path );
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
