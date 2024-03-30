// Copyright (c) 2018-2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_JSON_HPP
#define TAO_CONFIG_INTERNAL_JSON_HPP

#include <tao/json.hpp>

#include <tao/json/contrib/position.hpp>
#include <tao/json/contrib/traits.hpp>  // TODO: This might be a problem...

#include "pegtl.hpp"

namespace tao::config::internal::rules
{
   namespace jaxn = tao::json::jaxn::internal::rules;

   struct wss
      : pegtl::star< jaxn::ws >
   {};

   struct wsp
      : pegtl::plus< jaxn::ws >
   {};

}  // namespace tao::config::internal::rules

#endif
