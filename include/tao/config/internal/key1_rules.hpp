// Copyright (c) 2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_KEY1_RULES_HPP
#define TAO_CONFIG_INTERNAL_KEY1_RULES_HPP

#include "json.hpp"
#include "key_rules.hpp"
#include "pegtl.hpp"

namespace tao::config::internal::rules
{
   struct star : pegtl::one< '*' > {};

   struct key1_other : pegtl::sor< ident, quoted, index, minus, star > {};
   struct key1_first : pegtl::sor< ident, quoted > {};

   struct key1_must : pegtl::seq< key1_first, pegtl::star_must< dot, key1_other > > {};

}  // namespace tao::config::internal::rules

#endif
