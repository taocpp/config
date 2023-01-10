// Copyright (c) 2018-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_KEY_ACTION_HPP
#define TAO_CONFIG_INTERNAL_KEY_ACTION_HPP

#include <vector>

#include "json.hpp"
#include "key_grammar.hpp"
#include "pegtl.hpp"

#include "../key_part.hpp"

namespace tao::config::internal
{
   template< typename Rule >
   struct key_action
      : pegtl::nothing< Rule >
   {};

   template<>
   struct key_action< rules::ident >
   {
      template< typename Input >
      static void apply( const Input& in, std::vector< key_part >& st )
      {
         st.emplace_back( in.string() );
      }
   };

   template<>
   struct key_action< rules::index >
   {
      template< typename Input >
      static void apply( const Input& in, std::vector< key_part >& st )
      {
         st.emplace_back( std::size_t( std::stoul( in.string() ) ) );
      }
   };

   template<>
   struct key_action< rules::quoted_choice >
      : pegtl::change_action_and_states< json::jaxn::internal::unescape_action, std::string >
   {
      template< typename Input >
      static void success( const Input& /*unused*/, std::string& unescaped, std::vector< key_part >& st )
      {
         st.emplace_back( std::move( unescaped ) );
      }
   };

}  // namespace tao::config::internal

#endif
