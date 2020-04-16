// Copyright (c) 2019-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_EXTENSION_UTILITY_HPP
#define TAO_CONFIG_INTERNAL_EXTENSION_UTILITY_HPP

#include <string>

#include "action.hpp"
#include "control.hpp"
#include "forward.hpp"
#include "grammar.hpp"
#include "key_utility.hpp"
#include "pegtl.hpp"
#include "state.hpp"

namespace tao::config::internal
{
   inline json_t obtain_jaxn( pegtl_input_t& in )
   {
      json::events::to_basic_value< value_traits > consumer;
      pegtl::parse< pegtl::must< json::jaxn::internal::rules::sor_value >, json::jaxn::internal::action, json::jaxn::internal::errors >( in, consumer );
      return std::move( consumer.value );
   }

   inline std::string obtain_string( pegtl_input_t& in )
   {
      std::string s2;
      pegtl::parse< pegtl::must< json::jaxn::internal::rules::string_fragment >, json::jaxn::internal::unescape_action >( in, s2 );
      return s2;
   }

   inline std::vector< std::byte > obtain_binary( pegtl_input_t& in )
   {
      std::vector< std::byte > v2;
      pegtl::parse< pegtl::must< json::jaxn::internal::rules::binary_fragment >, json::jaxn::internal::bunescape_action >( in, v2 );
      return v2;
   }

   inline key obtain_key( pegtl_input_t& in, state& st )
   {
      pegtl::parse< pegtl::must< rules::pointer >, action, control >( in, st );
      return value_to_key( st.temporary );
   }

}  // namespace tao::config::internal

#endif
