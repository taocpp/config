// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_FORWARD_HPP
#define TAO_CONFIG_INTERNAL_FORWARD_HPP

namespace tao::config::internal
{
   struct entry;
   struct state;

   template< typename E >
   struct basic_concat;

   using concat = basic_concat< entry >;

   template< typename C >
   struct basic_array;
   template< typename C >
   struct basic_object;

   using array = basic_array< concat >;
   using object = basic_object< concat >;

   template< typename T >
   struct argument_traits;
   template< typename T >
   struct result_traits;

   struct extension_maps;

}  // namespace tao::config::internal

#endif
