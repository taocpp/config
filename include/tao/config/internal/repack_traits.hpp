// Copyright (c) 2018-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_REPACk_TRAITS_HPP
#define TAO_CONFIG_INTERNAL_REPACk_TRAITS_HPP

#include <utility>

#include "../key.hpp"

#include "pegtl.hpp"

namespace tao::config::internal
{
   template< typename V, typename = void >
   inline constexpr bool has_set_key = false;

   template< typename V >
   inline constexpr bool has_set_key< V, decltype( std::declval< V >().public_base().set_key( std::declval< const config::key& >() ), void() ) > = true;

   template< typename V, typename = void >
   inline constexpr bool has_set_position = false;

   template< typename V >
   inline constexpr bool has_set_position< V, decltype( std::declval< V >().public_base().set_position( std::declval< const pegtl::position& >() ), void() ) > = true;

}  // namespace tao::config::internal

#endif
