// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_VALUE_TRAITS_HPP
#define TAO_CONFIG_INTERNAL_VALUE_TRAITS_HPP

#include "../traits.hpp"

#include "annotation.hpp"
#include "json.hpp"

namespace tao::config::internal
{
   template< typename T >
   struct value_traits
      : public config::traits< T >
   {
   };

   template<>
   struct value_traits< void >
      : public json::traits< void >
   {
      static constexpr const bool enable_implicit_constructor = false;

      template< typename Value >
      using public_base = annotation;
   };

}  // namespace tao::config::internal

#endif
