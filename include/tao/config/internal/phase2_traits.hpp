// Copyright (c) 2018-2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE2_TRAITS_HPP
#define TAO_CONFIG_INTERNAL_PHASE2_TRAITS_HPP

#include "../traits.hpp"

#include "annotation.hpp"
#include "json.hpp"

namespace tao::config::internal
{
   template< typename T >
   struct phase2_traits
      : public config::traits< T >
   {
   };

   template<>
   struct phase2_traits< void >
      : public json::traits< void >
   {
      static constexpr const bool enable_implicit_constructor = false;

      template< typename Value >
      using public_base = annotation;
   };

}  // namespace tao::config::internal

#endif
