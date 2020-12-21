// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_VALUE_FUNCTIONS_HPP
#define TAO_CONFIG_INTERNAL_VALUE_FUNCTIONS_HPP

#include "config_action.hpp"
#include "config_rules.hpp"
#include "extension_maps.hpp"
// #include "phase1_access.hpp"
#include "phase1_append.hpp"
#include "state.hpp"

namespace tao::config::internal
{
   inline void copy_function( state& st, const key1& k )
   {
      //      const concat c = phase1_access( st.prefix, k );
      //      phase1_append( st.root, st.prefix + st.suffix, c );
   }

   inline void parse_function( const pegtl::position& p, state& st, const extension_maps& em, const std::string& s )
   {
      pegtl::string_input< pegtl::tracking_mode::eager, pegtl_input_t::eol_t > in( s, __FUNCTION__ );
      pegtl::parse_nested< rules::value, config_action >( p, static_cast< pegtl_input_t& >( in ), st, em );
   }

}  // namespace tao::config::internal

#endif
