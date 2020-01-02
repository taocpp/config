// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE2_HPP
#define TAO_CONFIG_INTERNAL_PHASE2_HPP

#include <stdexcept>
#include <type_traits>

#include "json.hpp"
#include "phase2_process.hpp"
#include "phase2_repackage.hpp"
#include "state.hpp"

#include "../schema/builtin.hpp"

namespace tao::config::internal
{
   template< template< typename... > class Traits >
   json::basic_value< Traits > phase2( state& st, schema::builtin b )
   {
      assert( st.astack.empty() );
      assert( st.lstack.empty() );
      assert( st.rstack.empty() );
      assert( st.ostack.size() == 1 );

      auto t1 = phase2_process( st.root );

      phase2_filter_temporaries( st.temporaries, t1 );

      if( st.schema.empty() ) {
         return phase2_repackage< Traits >( t1 );
      }
      const auto t2 = phase2_repackage< config::traits >( t1 );

      if( const auto error = schema::internal::phase2_from_file_and_validate( st.schema, std::move( b ), t2 ) ) {
         throw std::runtime_error( json::to_string( error ) );
      }
      if constexpr( std::is_same_v< Traits< void >, config::traits< void > > ) {
         return t2;
      }
      else {
         return phase2_repackage< Traits >( t1 );
      }
   }

}  // namespace tao::config::internal

#endif
