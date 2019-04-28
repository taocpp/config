// Copyright (c) 2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_PARSE_INPUT_HPP
#define TAO_CONFIG_PARSE_INPUT_HPP

#include <utility>

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
   template< template< typename... > class Traits, typename Input >
   json::basic_value< Traits > basic_parse_input( Input&& in )
   {
      internal::state st;
      pegtl::parse< internal::rules::grammar, internal::action, internal::control >( std::forward< Input >( in ), st );
      return internal::phase2< Traits >( st );
   }

   template< typename Input >
   inline value parse_input( Input&& in )
   {
      return basic_parse_input< traits >( std::forward< Input >( in ) );
   }

}  // namespace tao::config

#endif
