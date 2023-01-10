// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_INTERNAL_IS_REGEX_HPP
#define TAO_CONFIG_SCHEMA_INTERNAL_IS_REGEX_HPP

#include <regex>

#include "node.hpp"

namespace tao::config::schema::internal
{
   struct is_regex : node
   {
      using node::node;

      json::value validate( const value& v ) const override
      {
         if( auto e = string( m_source ).validate( v ) ) {
            return e;
         }
         const std::string_view sv = v.as< std::string_view >();
         try {
            std::regex( sv.begin(), sv.end() );
            return ok();
         }
         catch( const std::regex_error& ) {
            return error( v, "invalid regex" );
         }
      }
   };

}  // namespace tao::config::schema::internal

#endif
