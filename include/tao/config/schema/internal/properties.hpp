// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_INTERNAL_PROPERTIES_HPP
#define TAO_CONFIG_SCHEMA_INTERNAL_PROPERTIES_HPP

#include "node.hpp"

namespace tao::config::schema::internal
{
   struct properties : node
   {
      std::map< std::string, std::shared_ptr< ref > > m_required;
      std::map< std::string, std::shared_ptr< ref > > m_optional;
      std::shared_ptr< ref > m_default;

      using node::node;

      void add_required( const value& v, node_map& m, const std::string& path )
      {
         for( const auto& e : v.get_object() ) {
            m_required.try_emplace( e.first, std::make_shared< ref >( e.second, m, path ) );
         }
      }

      void add_optional( const value& v, node_map& m, const std::string& path )
      {
         for( const auto& e : v.get_object() ) {
            m_optional.try_emplace( e.first, std::make_shared< ref >( e.second, m, path ) );
         }
      }

      void resolve( const node_map& m ) override
      {
         for( const auto& e : m_required ) {
            e.second->resolve( m );
         }
         for( const auto& e : m_optional ) {
            e.second->resolve( m );
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

         json::value errors = json::empty_array;
         const auto& o = v.get_object();

         for( const auto& e : m_required ) {
            const auto it = o.find( e.first );
            if( it != o.end() ) {
               if( auto r = e.second->validate( it->second ) ) {
                  errors.emplace_back( std::move( r ) );
               }
            }
            else {
               errors.emplace_back( error( v, "missing property", { { "key", e.first } } ) );
            }
         }

         for( const auto& e : o ) {
            const auto it = m_optional.find( e.first );
            if( it != m_optional.end() ) {
               if( auto r = it->second->validate( e.second ) ) {
                  errors.emplace_back( std::move( r ) );
               }
            }
            else if( m_required.find( e.first ) == m_required.end() ) {
               if( m_default ) {
                  if( auto r = m_default->validate( e.second ) ) {
                     errors.emplace_back( std::move( r ) );
                  }
               }
               else {
                  // TODO: The position will point to the value of the key/value-pair,
                  // but it should point to the key itself.
                  errors.emplace_back( error( e.second, "unexpected property" ) );
               }
            }
         }

         auto& a = errors.get_array();
         if( a.empty() ) {
            return ok();
         }
         if( a.size() == 1 ) {
            return std::move( a.back() );
         }
         return error( v, "multiple errors", { { "errors", std::move( errors ) } } );
      }
   };

}  // namespace tao::config::schema::internal

#endif
