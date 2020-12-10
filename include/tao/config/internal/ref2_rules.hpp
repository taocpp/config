// Copyright (c) 2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_REF2_RULES_HPP
#define TAO_CONFIG_INTERNAL_REF2_RULES_HPP

#include "key_rules.hpp"
#include "pegtl.hpp"

namespace tao::config::internal::rules
{
   struct ref2_must;

   struct ref2_other : pegtl::sor< ident, quoted, ref2_must, index, minus > {};
   struct ref2_first : pegtl::sor< ident, quoted, ref2_must > {};
   struct ref2_list : pegtl::seq< ref2_first, pegtl::star_must< dot, ref2_other > > {};

   struct ref2_must : pegtl::if_must< pegtl::one< '(' >, ref2_list, pegtl::one< ')' > > {};
   struct ref2_rule : pegtl::seq< pegtl::one< '(' >, ref2_list, pegtl::one< ')' > > {};

}  // namespace tao::config::internal::rules

#endif
