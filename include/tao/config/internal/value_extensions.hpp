// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_VALUE_EXTENSIONS_HPP
#define TAO_CONFIG_INTERNAL_VALUE_EXTENSIONS_HPP

#include <sstream>

#include "action.hpp"
#include "control.hpp"
#include "extension_utility.hpp"
#include "grammar.hpp"
#include "inner_extensions.hpp"
#include "pegtl.hpp"
#include "phase1_access.hpp"
#include "state.hpp"
#include "to_stream.hpp"

namespace tao::config::internal
{
   inline void copy_extension( pegtl_input_t& in, state& st )
   {
      assert( !st.ostack.empty() );
      assert( !st.lstack.empty() );

      const auto pos = in.position();
      const auto p = obtain_key( in, st );

      concat& d = *st.lstack.back();
      const concat& s = phase1_access( pos, d.parent(), p );

      if( &d == &s ) {
         throw pegtl::parse_error( format( __FILE__, __LINE__, "copy to self detected", { &p } ), pos );
      }
      d.append( s );  // TOOD: Modify/update d.position?
   }

   inline void debug_extension( pegtl_input_t& in, state& st )
   {
      const auto pos = in.position();
      const auto p = obtain_key( in, st );

      std::ostringstream oss;
      to_stream( oss, phase1_access( pos, st.lstack.back()->parent(), p ) );
      st.temporary.assign( oss.str(), pos );
   }

   inline bool do_value_extension( pegtl_input_t& in, state& st )
   {
      assert( !st.clear );
      assert( !st.lstack.empty() );

      const auto pos = in.position();
      pegtl::parse< rules::outer, action, control >( in, st );

      if( st.extension == "copy" ) {
         copy_extension( in, st );
         return true;
      }
      if( st.extension == "parse" ) {
         parse_extension( in, st );
         return true;
      }
      const auto ext = std::move( st.extension );
      const auto& map = st.value_extensions;
      const auto i = map.find( ext );

      if( i != map.end() ) {
         i->second( in, st );
         st.lstack.back()->emplace_back_atom( std::move( st.temporary ) );
         return true;
      }
      throw pegtl::parse_error( format( __FILE__, __LINE__, "unknown value extension", { { "name", ext } } ), pos );
   }

}  // namespace tao::config::internal

#endif
