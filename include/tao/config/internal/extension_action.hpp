// Copyright (c) 2020-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_EXTENSION_ACTION_HPP
#define TAO_CONFIG_INTERNAL_EXTENSION_ACTION_HPP

#include "extension_grammar.hpp"
#include "pegtl.hpp"

namespace tao::config::internal
{
   template< typename Rule >
   struct extension_action
      : pegtl::nothing< Rule >
   {};

   template<>
   struct extension_action< rules::extension_name >
   {
      template< typename Input >
      static void apply( const Input& in, std::string& result )
      {
         result = in.string();
      }
   };

}  // namespace tao::config::internal

#endif
