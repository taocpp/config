// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_INTERNAL_ISTRING_HPP
#define TAO_CONFIG_SCHEMA_INTERNAL_ISTRING_HPP

#include "iless.hpp"
#include "node.hpp"

#include <set>
#include <string_view>

namespace tao::config::schema::internal
{
   struct istring : node
   {
      std::set< std::string_view, iless > m_values;

      explicit istring( const value& v )
         : node( v )
      {
         if( m_source.is_array() ) {
            for( const auto& e : m_source.get_array() ) {
               m_values.emplace( e.as< std::string_view >() );
            }
         }
         else {
            m_values.emplace( m_source.as< std::string_view >() );
         }
      }

      json::value validate( const value& v ) const override
      {
         const auto s = v.as< std::string_view >();
         if( m_values.count( s ) == 1 ) {
            return ok();
         }

         json::value candidates = json::empty_array;
         for( const auto& sv : m_values ) {
            candidates.emplace_back( sv );
         }
         return error( v, "value did not match", { { "value", s }, { "candidates", std::move( candidates ) } } );
      }
   };

}  // namespace tao::config::schema::internal

#endif
