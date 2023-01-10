// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_INTERNAL_PATTERN_HPP
#define TAO_CONFIG_SCHEMA_INTERNAL_PATTERN_HPP

#include <regex>

#include "node.hpp"
#include "string.hpp"

namespace tao::config::schema::internal
{
   struct pattern : node
   {
      const std::regex m_regex;

      pattern( const value& v, const std::string_view sv )
         : node( v ),
           m_regex( sv.begin(), sv.end() )
      {}

      explicit pattern( const value& v )
         : pattern( v, v.as< std::string_view >() )
      {}

      json::value validate( const value& v ) const override
      {
         if( auto e = string( m_source ).validate( v ) ) {
            return e;
         }
         const std::string_view sv = v.as< std::string_view >();
         return std::regex_search( sv.begin(), sv.end(), m_regex ) ? ok() : error( v, "pattern did not match" );
      }
   };

}  // namespace tao::config::schema::internal

#endif
