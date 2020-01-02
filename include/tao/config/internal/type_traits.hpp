// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_TYPE_TRAITS_HPP
#define TAO_CONFIG_INTERNAL_TYPE_TRAITS_HPP

#include <type_traits>

#include "../key.hpp"

#include "json.hpp"
#include "pegtl.hpp"

namespace tao::config::internal
{
   template< typename V, typename = void >
   struct has_set_key
      : public std::false_type
   {
   };

   template< typename V >
   struct has_set_key< V, decltype( std::declval< V >().public_base().set_key( std::declval< const config::key& >() ), void() ) >
      : public std::true_type
   {
   };

   template< typename V, typename = void >
   struct has_set_position
      : public std::false_type
   {
   };

   template< typename V >
   struct has_set_position< V, decltype( std::declval< V >().public_base().set_position( std::declval< const pegtl::position& >() ), void() ) >
      : public std::true_type
   {
   };

}  // namespace tao::config::internal

#endif
