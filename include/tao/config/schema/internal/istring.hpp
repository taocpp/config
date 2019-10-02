// Copyright (c) 2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_INTERNAL_ISTRING_HPP
#define TAO_CONFIG_SCHEMA_INTERNAL_ISTRING_HPP

#include <strings.h>

#include "node.hpp"

namespace tao::config::schema::internal
{
   struct istring : node
   {
      std::vector< std::string_view > m_values;

      explicit istring( const value& v )
         : node( v )
      {
         if( m_source.is_array() ) {
            for( const auto& e : m_source.unsafe_get_array() ) {
               m_values.emplace_back( e.as< std::string_view >() );
            }
         }
         else {
            m_values.emplace_back( m_source.as< std::string_view >() );
         }
      }

      json::value validate( const value& v ) const override
      {
         const auto s = v.as< std::string_view >();
         for( const auto sv : m_values ) {
            if( ( s.size() == sv.size() ) && ( ::strncasecmp( s.begin(), sv.begin(), s.size() ) == 0 ) ) {
               return ok();
            }
         }
         json::value candidates = json::empty_array;
         for( const auto sv : m_values ) {
            candidates.unsafe_emplace_back( sv );
         }
         return error( v, "value did not match", { { "value", s }, { "candidates", std::move( candidates ) } } );
      }
   };

}  // namespace tao::config::schema::internal

#endif
