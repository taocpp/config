// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_TYPE_TRAITS_HPP
#define TAO_CONFIG_INTERNAL_TYPE_TRAITS_HPP

#include <type_traits>

#include "json.hpp"
#include "pegtl.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         template< typename V, typename = void >
         struct has_set_pointer
            : public std::false_type
         {
         };

         template< typename V >
         struct has_set_pointer< V, decltype( std::declval< V >().set_pointer( std::declval< const config::pointer& >() ), void() ) >
            : public std::true_type
         {
         };

         template< typename V, typename = void >
         struct has_set_position
            : public std::false_type
         {
         };

         template< typename V >
         struct has_set_position< V, decltype( std::declval< V >().set_position( std::declval< const pegtl::position& >() ), void() ) >
            : public std::true_type
         {
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
