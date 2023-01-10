// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_INTERNAL_HAS_PROPERTY_HPP
#define TAO_CONFIG_SCHEMA_INTERNAL_HAS_PROPERTY_HPP

#include "node.hpp"
#include "object.hpp"

namespace tao::config::schema::internal
{
   struct has_property : node
   {
      std::vector< std::string > m_keys;

      explicit has_property( const value& v )
         : node( v )
      {
         if( v.is_string_type() ) {
            m_keys.emplace_back( v.as< std::string_view >() );
         }
         else {
            for( const auto& e : v.get_array() ) {
               m_keys.emplace_back( e.as< std::string_view >() );
            }
         }
      }

      json::value validate( const value& v ) const override
      {
         if( auto e = object( m_source ).validate( v ) ) {
            return e;
         }
         const auto& o = v.get_object();
         for( const auto& k : m_keys ) {
            if( o.find( k ) != o.end() ) {
               return ok();
            }
         }
         return error( v, "missing property", { { "keys", m_keys } } );
      }
   };

}  // namespace tao::config::schema::internal

#endif
