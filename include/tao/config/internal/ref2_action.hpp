// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_REF2_ACTION_HPP
#define TAO_CONFIG_INTERNAL_REF2_ACTION_HPP

#include <vector>

#include "json.hpp"
#include "ref2_part.hpp"
#include "ref2_rules.hpp"
#include "pegtl.hpp"

namespace tao::config::internal
{
   template< typename Rule >
   struct ref2_action
      : pegtl::nothing< Rule >
   {};

   template<>
   struct ref2_action< rules::ident >
   {
      template< typename Input >
      static void apply( const Input& in, std::vector< ref2_part >& st, const unsigned /*unused*/ )
      {
         st.emplace_back( in.string(), in.position() );
      }
   };

   template<>
   struct ref2_action< rules::index >
   {
      template< typename Input >
      static void apply( const Input& in, std::vector< ref2_part >& st, const unsigned /*unused*/ )
      {
         st.emplace_back( std::stoul( in.string() ), in.position() );
      }
   };

   template<>
   struct ref2_action< rules::minus >
   {
      template< typename Input >
      static void apply( const Input& in, std::vector< ref2_part >& st, const unsigned /*unused*/ )
      {
         st.emplace_back( part_minus, in.position() );
      }
   };

   template<>
   struct ref2_action< rules::quoted_choice >
      : pegtl::change_action_and_states< json::jaxn::internal::unescape_action, std::string >
   {
      template< typename Input >
      static void success( const Input& in, std::string& unescaped, std::vector< ref2_part >& st, const unsigned /*unused*/ )
      {
         st.emplace_back( std::move( unescaped ), in.position() );  // TODO: Position from beginning of quoted string instead of end.
      }
   };

   template<>
   struct ref2_action< rules::ref2_list >
      : pegtl::nothing< rules::ref2_list >
   {
      template< typename Rule,
                pegtl::apply_mode A,
                pegtl::rewind_mode M,
                template< typename... >
                class Action,
                template< typename... >
                class Control,
                typename ParseInput >
      [[nodiscard]] static bool match( ParseInput& in, std::vector< ref2_part >& st, const unsigned nest )
      {
         if( nest == 0 ) {
            return pegtl::match< Rule, A, M, Action, Control >( in, st, nest + 1 );
         }
         st.emplace_back( part_vector, in.position() );
         return pegtl::match< Rule, A, M, Action, Control >( in, st.back().get_vector(), nest + 1 );
      }
   };

}  // namespace tao::config::internal

#endif
