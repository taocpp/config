// Copyright (c) 2018-2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE2_HPP
#define TAO_CONFIG_INTERNAL_PHASE2_HPP

#include "json.hpp"
#include "phase2_process.hpp"
#include "phase2_repackage.hpp"
#include "state.hpp"

namespace tao::config::internal
{
   template< template< typename... > class Traits >
   json::basic_value< Traits > phase2( const std::string& source, state& st )
   {
      assert( st.astack.empty() );
      assert( st.lstack.empty() );
      assert( st.rstack.empty() );
      assert( st.ostack.size() == 1 );

      return phase2_repackage< Traits >( source, st.temporaries, phase2_process( st.root ) );
   }

}  // namespace tao::config::internal

#endif
