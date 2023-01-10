// Copyright (c) 2018-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_TRAITS_HPP
#define TAO_CONFIG_TRAITS_HPP

#include <tao/json.hpp>

#include "annotation.hpp"

namespace tao::config
{
   template< typename T >
   struct traits
      : json::traits< T >
   {};

   template<>
   struct traits< void >
      : json::traits< void >
   {
      static constexpr const bool enable_implicit_constructor = false;

      template< typename Value >
      using public_base = annotation;
   };

}  // namespace tao::config

#endif
