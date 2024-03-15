// Copyright (c) 2019-2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_FORWARD_HPP
#define TAO_CONFIG_INTERNAL_FORWARD_HPP

#include <functional>
#include <map>
#include <string>

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

   template< typename, typename >
   struct function_traits;

   using function = std::function< bool( entry& ) >;
   using function_map = std::map< std::string, function >;

}  // namespace tao::config::internal

#endif
