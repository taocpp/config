// Copyright (c) 2018-2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE2_HPP
#define TAO_CONFIG_INTERNAL_PHASE2_HPP

#include <stdexcept>

#include "json.hpp"
#include "phase2_process.hpp"
#include "phase2_repackage.hpp"
#include "state.hpp"

namespace tao::config::schema
{
   inline json::value read_and_validate( const std::string& filename, const value& v );

}  // namespace tao::config::schema

namespace tao::config::internal
{
   template< template< typename... > class Traits >
   json::basic_value< Traits > phase2( state& st )
   {
      assert( st.astack.empty() );
      assert( st.lstack.empty() );
      assert( st.rstack.empty() );
      assert( st.ostack.size() == 1 );

      auto tmp = phase2_process( st.root );

      phase2_filter_temporaries( st.temporaries, tmp );

      if( !st.schema.empty() ) {
         const auto error = schema::read_and_validate( st.schema, phase2_repackage< config::traits >( tmp ) );

         if( error ) {
            throw std::runtime_error( json::to_string( error ) );
         }
      }
      return phase2_repackage< Traits >( tmp );  // TODO: Don't repackage twice when result uses config::traits, too.
   }

}  // namespace tao::config::internal

#endif
