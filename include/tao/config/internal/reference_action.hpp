// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_REFERENCE_ACTION_HPP
#define TAO_CONFIG_INTERNAL_REFERENCE_ACTION_HPP

#include "grammar.hpp"
#include "reference_state.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         template< typename Rule >
         struct reference_action
            : public pegtl::nothing< Rule >
         {
         };

         template<>
         struct reference_action< rules::round_a >
         {
            static void apply0( reference_state& st )
            {
               assert( !st.rstack.empty() );

               st.rstack.emplace_back( &st.rstack.back()->emplace_back( json::empty_array ) );
            }
         };

         template<>
         struct reference_action< rules::round_z >
         {
            static void apply0( reference_state& st )
            {
               assert( !st.rstack.empty() );

               st.rstack.pop_back();
            }
         };

         template<>
         struct reference_action< rules::identifier >
         {
            template< typename Input >
            static void apply( const Input& in, reference_state& st )
            {
               assert( !st.rstack.empty() );
               assert( st.rstack.back()->is_array() );

               st.rstack.back()->emplace_back( in.string() );
            }
         };

         template<>
         struct reference_action< rules::index >
         {
            template< typename Input >
            static void apply( const Input& in, reference_state& st )
            {
               assert( !st.rstack.empty() );
               assert( st.rstack.back()->is_array() );

               st.rstack.back()->emplace_back( std::stoul( in.string() ) );
            }
         };

         template<>
         struct reference_action< rules::minus >
         {
            static void apply0( reference_state& st )
            {
               assert( !st.rstack.empty() );
               assert( st.rstack.back()->is_array() );

               st.rstack.back()->emplace_back( false );  // See token_from_value() in utility.hpp -> false -> token::MINUS.
            }
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
