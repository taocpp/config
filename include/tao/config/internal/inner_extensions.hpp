// Copyright (c) 2018-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_INNER_EXTENSIONS_HPP
#define TAO_CONFIG_INTERNAL_INNER_EXTENSIONS_HPP

#include "extension_maps.hpp"
#include "extension_types.hpp"
#include "extension_utility.hpp"
#include "forward.hpp"
#include "json.hpp"
#include "json_traits.hpp"
#include "parse_utility.hpp"
#include "pegtl.hpp"
#include "state.hpp"
#include "system_utility.hpp"

namespace tao::config::internal
{
   [[nodiscard]] inline const value_extension_map& the_inner_extension_map();

   [[nodiscard]] inline json_t do_inner_extension( pegtl_input_t& in, state& st, const extension_maps& em )
   {
      if( parse_open( in ) ) {
         const std::string name = parse_extension( in );
         {
            const auto i = em.inner.find( name );
            if( i != em.inner.end() ) {
               json_t result = i->second( in, st, em );
               parse_close( in );
               return result;
            }
         }
         throw pegtl::parse_error( "unknown inner extension " + name, in.position() );
      }
      json_t result = parse_jaxn( in );
      skip_ws( in );
      return result;
   }

}  // namespace tao::config::internal

#endif
