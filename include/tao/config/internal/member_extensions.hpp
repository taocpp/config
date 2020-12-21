// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_MEMBER_EXTENSIONS_HPP
#define TAO_CONFIG_INTERNAL_MEMBER_EXTENSIONS_HPP

#include "extension_maps.hpp"
#include "extension_types.hpp"
#include "extension_utility.hpp"
#include "parse_utility.hpp"
#include "state.hpp"

namespace tao::config::internal
{
   [[nodiscard]] inline const member_extension_map& the_member_extension_map();

   [[nodiscard]] inline bool do_member_extension( pegtl_input_t& in, state& st, const extension_maps& em )
   {
      assert( st.suffix.empty() );

      const std::string name = parse_extension( in );
      {
         const auto i = em.member.find( name );
         if( i != em.member.end() ) {
            i->second( in, st, em );
            return true;
         }
      }
      throw pegtl::parse_error( "unknown member extension " + name, in.position() );
   }

}  // namespace tao::config::internal

#endif
