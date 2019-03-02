// Copyright (c) 2018-2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_ACTION_HPP
#define TAO_CONFIG_INTERNAL_ACTION_HPP

#include <sstream>
#include <stdexcept>

#include "access.hpp"
#include "binary_state.hpp"
#include "grammar.hpp"
#include "json.hpp"
#include "number_action.hpp"
#include "number_state.hpp"
#include "pegtl.hpp"
#include "quoted_state.hpp"
#include "state.hpp"
#include "string_state.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         template< typename Rule >
         struct action
            : public pegtl::nothing< Rule >
         {
         };

         template<>
         struct action< rules::null_s >
         {
            template< typename Input >
            static void apply( const Input& in, state& st )
            {
               assert( !st.lstack.empty() );

               st.lstack.back()->emplace_back_atom( in.position(), json::null );
            }
         };

         template<>
         struct action< rules::true_s >
         {
            template< typename Input >
            static void apply( const Input& in, state& st )
            {
               assert( !st.lstack.empty() );

               st.lstack.back()->emplace_back_atom( in.position(), true );
            }
         };

         template<>
         struct action< rules::false_s >
         {
            template< typename Input >
            static void apply( const Input& in, state& st )
            {
               assert( !st.lstack.empty() );

               st.lstack.back()->emplace_back_atom( in.position(), false );
            }
         };

         template<>
         struct action< rules::identifier >
         {
            template< typename Input >
            static void apply( const Input& in, state& st )
            {
               assert( !st.rstack.empty() );
               assert( st.rstack.back()->is_array() );

               st.rstack.back()->emplace_back( in.string() );
            }
         };

         template<>
         struct action< rules::index >
         {
            template< typename Input >
            static void apply( const Input& in, state& st )
            {
               assert( !st.rstack.empty() );
               assert( st.rstack.back()->is_array() );

               st.rstack.back()->emplace_back( std::stoul( in.string() ) );
            }
         };

         template<>
         struct action< rules::star >
         {
            static void apply0( state& st )
            {
               assert( !st.rstack.empty() );
               assert( st.rstack.back()->is_array() );

               st.rstack.back()->emplace_back( true );  // See token_from_value() in utility.hpp -> true -> token::star.
            }
         };

         template<>
         struct action< rules::minus >
         {
            static void apply0( state& st )
            {
               assert( !st.rstack.empty() );
               assert( st.rstack.back()->is_array() );

               st.rstack.back()->emplace_back( false );  // See token_from_value() in utility.hpp -> false -> token::minus.
            }
         };

         template<>
         struct action< rules::equals >
         {
            static void apply0( state& st )
            {
               st.clear_for_assign = true;
            }
         };

         template<>
         struct action< rules::ext_name >
         {
            template< typename Input >
            static void apply( const Input& in, state& st )
            {
               st.extension = in.string();
            }
         };

         template<>
         struct action< json::jaxn::internal::rules::sor_value >
            : public pegtl::change_action_and_state< number_action, number_state >
         {
         };

         template< bool Neg >
         struct action< json::jaxn::internal::rules::number< Neg > >
            : public pegtl::change_state< json::internal::number_state< Neg > >
         {
         };

         template<>
         struct action< rules::binary_choice >
            : public pegtl::change_action_and_state< json::jaxn::internal::bunescape_action, binary_state >
         {
         };

         template<>
         struct action< rules::string_choice >
            : public pegtl::change_action_and_state< json::jaxn::internal::unescape_action, string_state >
         {
         };

         template<>
         struct action< rules::quoted_choice >
            : public pegtl::change_action_and_state< json::jaxn::internal::unescape_action, quoted_state >
         {
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
