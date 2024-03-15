// Copyright (c) 2019-2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PARSE_UTILITY_HPP
#define TAO_CONFIG_INTERNAL_PARSE_UTILITY_HPP

#include <utility>

#include "forward.hpp"
#include "jaxn_action.hpp"
#include "json.hpp"
#include "json_to_value.hpp"
#include "json_traits.hpp"
#include "key1.hpp"
#include "key1_action.hpp"
#include "key1_grammar.hpp"
#include "pegtl.hpp"
#include "reference2.hpp"
#include "reference2_action.hpp"
#include "reference2_grammar.hpp"

namespace tao::config::internal
{
   [[nodiscard]] inline key1 parse_key1( pegtl_input_t& in )
   {
      key1 result;
      pegtl::parse< pegtl::must< rules::key1_rule >, key1_action >( in, result.vector() );
      return result;
   }

   [[nodiscard]] inline reference2 parse_reference2( pegtl_input_t& in )
   {
      reference2 result;
      pegtl::parse< pegtl::must< rules::reference2_rest >, reference2_action >( in, result.vector() );  // NOTE: Assumes that the opening bracket was already parsed!
      return result;
   }

   [[nodiscard]] inline json_t parse_jaxn( pegtl_input_t& in )
   {
      json_to_value consumer( in.position() );
      pegtl::parse< pegtl::must< json::jaxn::internal::rules::sor_single_value >, jaxn_action, json::jaxn::internal::errors >( in, consumer );
      return std::move( consumer.value );
   }

}  // namespace tao::config::internal

#endif
