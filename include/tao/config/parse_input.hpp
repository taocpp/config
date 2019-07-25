// Copyright (c) 2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_PARSE_INPUT_HPP
#define TAO_CONFIG_PARSE_INPUT_HPP

#include "value.hpp"

#include "internal/action.hpp"
#include "internal/control.hpp"
#include "internal/grammar.hpp"
#include "internal/pegtl.hpp"
#include "internal/phase2.hpp"
#include "internal/state.hpp"

#include "internal/member_extensions.hpp"
#include "internal/value_extensions.hpp"

namespace tao::config
{
   template< template< typename... > class Traits >
   json::basic_value< Traits > basic_parse_input( pegtl_input_t&& in )
   {
      try {
         internal::state st;
         pegtl::parse< internal::rules::config_file, internal::action, internal::control >( in, st );
         return internal::phase2< Traits >( in.source(), st );
      }
      catch( const pegtl::parse_error& e ) {
         const auto p = e.positions.front();
         std::cerr << p << ": " << e.what() << std::endl
                   << in.line_at( p ) << std::endl
                   << std::string( p.byte_in_line, ' ' ) << '^' << std::endl;
         throw;
      }
   }

   inline value parse_input( pegtl_input_t&& in )
   {
      return basic_parse_input< traits >( std::move( in ) );
   }

}  // namespace tao::config

#endif
