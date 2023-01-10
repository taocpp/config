// Copyright (c) 2018-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_KEY1_ACTION_HPP
#define TAO_CONFIG_INTERNAL_KEY1_ACTION_HPP

#include <vector>

#include "json.hpp"
#include "key1_grammar.hpp"
#include "key1_part.hpp"
#include "pegtl.hpp"

namespace tao::config::internal
{
   template< typename Rule >
   struct key1_action
      : pegtl::nothing< Rule >
   {};

   template<>
   struct key1_action< rules::ident >
   {
      template< typename Input >
      static void apply( const Input& in, std::vector< key1_part >& st )
      {
         st.emplace_back( in.string(), in.position() );
      }
   };

   template<>
   struct key1_action< rules::index >
   {
      template< typename Input >
      static void apply( const Input& in, std::vector< key1_part >& st )
      {
         st.emplace_back( std::size_t( std::stoul( in.string() ) ), in.position() );
      }
   };

   template<>
   struct key1_action< rules::star >
   {
      template< typename Input >
      static void apply( const Input& in, std::vector< key1_part >& st )
      {
         st.emplace_back( part_star, in.position() );
      }
   };

   template<>
   struct key1_action< rules::quoted_choice >
      : pegtl::change_action_and_states< json::jaxn::internal::unescape_action, std::string >
   {
      template< typename Input >
      static void success( const Input& in, std::string& unescaped, std::vector< key1_part >& st )
      {
         st.emplace_back( std::move( unescaped ), in.position() );  // TODO: Position from beginning of quoted string instead of end.
      }
   };

}  // namespace tao::config::internal

#endif
