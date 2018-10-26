// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_CONTROL_HPP
#define TAO_CONFIG_INTERNAL_CONTROL_HPP

#include "assign.hpp"
#include "binary_state.hpp"
#include "changes.hpp"
#include "grammar.hpp"
#include "json.hpp"
#include "key_string_state.hpp"
#include "number_action.hpp"
#include "number_state.hpp"
#include "pegtl.hpp"
#include "ref_string_state.hpp"
#include "reference_action.hpp"
#include "reference_state.hpp"
#include "state.hpp"
#include "string_state.hpp"
#include "entry.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         template< typename Rule >
         struct control
            : public pegtl::normal< Rule >
         {
         };

         template<>
         struct control< json::jaxn::internal::rules::sor_value >
            : public change_state_and_action< json::jaxn::internal::rules::sor_value, number_state, number_action >
         {
         };

         template< bool Neg >
         struct control< json::jaxn::internal::rules::number< Neg > >
            : public pegtl::change_state< json::jaxn::internal::rules::number< Neg >, json::internal::number_state< Neg > >
         {
         };

         template<>
         struct control< rules::reference >
            : public change_state_and_action< rules::reference, reference_state, reference_action >
         {
         };

         template<>
         struct control< rules::binary_choice >
            : public change_state_and_action< rules::binary_choice, binary_state, json::jaxn::internal::bunescape_action >
         {
         };

         template<>
         struct control< rules::string_choice >
            : public change_state_and_action< rules::string_choice, string_state, json::jaxn::internal::unescape_action >
         {
         };

         template<>
         struct control< rules::key_quoted_choice >
            : public change_state_and_action< rules::key_quoted_choice, key_string_state, json::jaxn::internal::unescape_action >
         {
         };

         template<>
         struct control< rules::ref_quoted_choice >
            : public change_state_and_action< rules::ref_quoted_choice, ref_string_state, json::jaxn::internal::unescape_action >
         {
         };

         template<>
         struct control< rules::element >
            : public pegtl::normal< rules::element >
         {
            template< typename Input >
            static void start( const Input& in, state& st )
            {
               assert( !st.astack.empty() );

               st.lstack.emplace_back( &st.astack.back()->emplace_back( in.position() ) );
            }

            template< typename Input >
            static void success( const Input&, state& st )
            {
               assert( !st.astack.empty() );
               assert( !st.lstack.empty() );

               st.lstack.pop_back();
            }
         };

         template<>
         struct control< rules::element_list >
            : public pegtl::normal< rules::element_list >
         {
            template< typename Input >
            static void start( const Input& in, state& st )
            {
               assert( !st.lstack.empty() );

               st.astack.emplace_back( &st.lstack.back()->v.emplace_back( entry::array( in.position() ) ).get_array() );
            }

            template< typename Input >
            static void success( const Input&, state& st )
            {
               assert( !st.astack.empty() );

               st.astack.pop_back();
            }
         };

         template<>
         struct control< rules::value_list >
            : public pegtl::normal< rules::value_list >
         {
            template< typename Input >
            static void start( const Input& in, state& st )
            {
               assert( !st.ostack.empty() );

               st.lstack.emplace_back( &assign( in.position(), *st.ostack.back(), st.key ) );

               if( st.clear_for_assign ) {
                  st.lstack.back()->v.clear();
               }
               st.key.clear();
            }

            template< typename Input >
            static void success( const Input&, state& st )
            {
               assert( !st.ostack.empty() );
               assert( !st.lstack.empty() );

               st.lstack.pop_back();
            }
         };

         template<>
         struct control< rules::member_list >
            : public pegtl::normal< rules::member_list >
         {
            template< typename Input >
            static void start( const Input& in, state& st )
            {
               assert( !st.lstack.empty() );

               st.ostack.emplace_back( &st.lstack.back()->v.emplace_back( entry::object( in.position() ) ).get_object() );
            }

            template< typename Input >
            static void success( const Input&, state& st )
            {
               assert( !st.ostack.empty() );

               st.ostack.pop_back();
            }
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
