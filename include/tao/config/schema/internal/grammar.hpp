// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_INTERNAL_GRAMMAR_HPP
#define TAO_CONFIG_SCHEMA_INTERNAL_GRAMMAR_HPP

#include "../../internal/pegtl.hpp"

#include "node.hpp"
#include "string.hpp"

namespace tao::config::schema::internal
{
   template< typename Grammar >
   struct grammar : node
   {
      using node::node;

      json::value validate( const value& v ) const override
      {
         if( auto e = string( m_source ).validate( v ) ) {
            return e;
         }
         const std::string_view sv = v.as< std::string_view >();
         if( pegtl::parse< pegtl::seq< Grammar, pegtl::eof > >( pegtl_input_t( sv, "<internal>" ) ) ) {
            return ok();
         }
         return error( v, "invalid format" );
      }
   };

}  // namespace tao::config::schema::internal

#endif
