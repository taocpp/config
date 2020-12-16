// Copyright (c) 2019-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PARSE_UTILITY_HPP
#define TAO_CONFIG_INTERNAL_PARSE_UTILITY_HPP

#include <string>

#include "json.hpp"
#include "json_action.hpp"
#include "json_to_value.hpp"
#include "json_traits.hpp"
#include "key1.hpp"
#include "key1_action.hpp"
#include "key1_rules.hpp"
#include "pegtl.hpp"
#include "ref2.hpp"
#include "ref2_action.hpp"
#include "ref2_rules.hpp"

namespace tao::config::internal
{
   inline void skip_ws( pegtl_input_t& in )
   {
      pegtl::parse< pegtl::star< json::jaxn::internal::rules::ws > >( in );
   }

   [[nodiscard]] inline key1 parse_key1( pegtl_input_t& in )
   {
      key1 result;
      pegtl::parse< pegtl::must< rules::key1_rule >, key1_action >( in, result.vector() );
      return result;
   }

   [[nodiscard]] inline ref2 parse_ref2( pegtl_input_t& in )
   {
      ref2 result;
      pegtl::parse< pegtl::must< rules::ref2_rest >, ref2_action >( in, result.vector() );  // NOTE: Assumes that the opening bracket was already parsed!
      return result;
   }

   [[nodiscard]] inline json_t parse_jaxn( pegtl_input_t& in )
   {
      json_to_value consumer( in.position() );
      pegtl::parse< pegtl::must< json::jaxn::internal::rules::sor_single_value >, json_action, json::jaxn::internal::errors >( in, consumer );
      return std::move( consumer.value );
   }

   [[nodiscard]] inline std::string parse_string( pegtl_input_t& in )
   {
      std::string result;
      pegtl::parse< pegtl::must< json::jaxn::internal::rules::string_fragment >, json::jaxn::internal::unescape_action >( in, result );
      return result;
   }

}  // namespace tao::config::internal

#endif
