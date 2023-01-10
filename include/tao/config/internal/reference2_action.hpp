// Copyright (c) 2018-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_REFERENCE2_ACTION_HPP
#define TAO_CONFIG_INTERNAL_REFERENCE2_ACTION_HPP

#include <vector>

#include "json.hpp"
#include "pegtl.hpp"
#include "reference2_grammar.hpp"
#include "reference2_part.hpp"

namespace tao::config::internal
{
   template< typename Rule >
   struct reference2_action
      : pegtl::nothing< Rule >
   {};

   template<>
   struct reference2_action< rules::ident >
   {
      template< typename Input >
      static void apply( const Input& in, std::vector< reference2_part >& st )
      {
         st.emplace_back( in.string(), in.position() );
      }
   };

   template<>
   struct reference2_action< rules::index >
   {
      template< typename Input >
      static void apply( const Input& in, std::vector< reference2_part >& st )
      {
         st.emplace_back( std::stoul( in.string() ), in.position() );
      }
   };

   template<>
   struct reference2_action< rules::quoted_choice >
      : pegtl::change_action_and_states< json::jaxn::internal::unescape_action, std::string >
   {
      template< typename Input >
      static void success( const Input& in, std::string& unescaped, std::vector< reference2_part >& st )
      {
         st.emplace_back( std::move( unescaped ), in.position() );  // TODO: Position from beginning of quoted string instead of end.
      }
   };

   template<>
   struct reference2_action< rules::reference2_must >
      : pegtl::nothing< rules::reference2_must >
   {
      template< typename Rule,
                pegtl::apply_mode A,
                pegtl::rewind_mode M,
                template< typename... >
                class Action,
                template< typename... >
                class Control,
                typename ParseInput >
      [[nodiscard]] static bool match( ParseInput& in, std::vector< reference2_part >& st )
      {
         st.emplace_back( part_vector, in.position() );
         return pegtl::match< Rule, A, M, Action, Control >( in, st.back().get_vector() );
      }
   };

}  // namespace tao::config::internal

#endif
