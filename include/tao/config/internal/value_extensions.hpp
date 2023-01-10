// Copyright (c) 2018-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_VALUE_EXTENSIONS_HPP
#define TAO_CONFIG_INTERNAL_VALUE_EXTENSIONS_HPP

#include <sstream>

#include "concat.hpp"
#include "config_action.hpp"
#include "extension_maps.hpp"
#include "extension_utility.hpp"
#include "forward.hpp"
#include "parse_utility.hpp"
#include "pegtl.hpp"
#include "phase1_append.hpp"
#include "state.hpp"

namespace tao::config::internal
{
   [[nodiscard]] inline const value_extension_map& the_inner_extension_map();
   [[nodiscard]] inline const member_extension_map& the_value_extension_map();

   [[nodiscard]] inline bool do_value_extension( pegtl_input_t& in, state& st, const extension_maps& em )
   {
      const std::string name = parse_extension( in );
      {
         const auto i = em.value.find( name );
         if( i != em.value.end() ) {
            i->second( in, st, em );
            return true;
         }
      }
      {
         const auto i = em.inner.find( name );
         if( i != em.inner.end() ) {
            const auto j = i->second( in, st, em );
            const auto f = [ & ]( concat& c ) { c.concat.emplace_back( j ); };
            phase1_append( st.root, st.prefix + st.suffix, f, phase1_mode::manifest );
            return true;
         }
      }
      throw pegtl::parse_error( "unknown value extension " + name, in.position() );
   }

}  // namespace tao::config::internal

#endif
