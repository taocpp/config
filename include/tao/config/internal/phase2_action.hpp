// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE2_ACTION_HPP
#define TAO_CONFIG_INTERNAL_PHASE2_ACTION_HPP

#include "grammar.hpp"
#include "phase2_state.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         template< typename Rule >
         struct phase2_action
            : public pegtl::nothing< Rule >
         {
         };

         template<>
         struct phase2_action< rules::round_a >
         {
            static void apply0( phase2_state& st )
            {
               assert( !st.rstack.empty() );

               st.rstack.emplace_back( &st.rstack.back()->emplace_back( json::empty_array ) );
            }
         };

         template<>
         struct phase2_action< rules::round_z >
         {
            static void apply0( phase2_state& st )
            {
               assert( !st.rstack.empty() );

               st.rstack.pop_back();
            }
         };

         template<>
         struct phase2_action< rules::phase2_name >
         {
            template< typename Input >
            static void apply( const Input& in, phase2_state& st )
            {
               assert( !st.rstack.empty() );
               assert( st.rstack.back()->is_array() );

               st.rstack.back()->emplace_back( in.string() );
            }
         };

         template<>
         struct phase2_action< rules::phase2_index >
         {
            template< typename Input >
            static void apply( const Input& in, phase2_state& st )
            {
               assert( !st.rstack.empty() );
               assert( st.rstack.back()->is_array() );

               st.rstack.back()->emplace_back( std::stoul( in.string() ) );
            }
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
