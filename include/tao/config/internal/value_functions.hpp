// Copyright (c) 2018-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_VALUE_FUNCTIONS_HPP
#define TAO_CONFIG_INTERNAL_VALUE_FUNCTIONS_HPP

#include "config_action.hpp"
#include "config_grammar.hpp"
#include "extension_maps.hpp"
#include "state.hpp"

namespace tao::config::internal
{
   inline void value_function( const pegtl::position& p, state& st, const extension_maps& em, const std::string& s )
   {
      pegtl::string_input< pegtl::tracking_mode::eager, pegtl_input_t::eol_t > in( s, __FUNCTION__ );
      pegtl::parse_nested< rules::value, config_action >( p, static_cast< pegtl_input_t& >( in ), st, em );
   }

}  // namespace tao::config::internal

#endif
