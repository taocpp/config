// Copyright (c) 2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_REF2_RULES_HPP
#define TAO_CONFIG_INTERNAL_REF2_RULES_HPP

#include "key_rules.hpp"
#include "pegtl.hpp"

namespace tao::config::internal::rules
{
   struct ref2_must;

   struct ref2_part : pegtl::sor< ident, quoted, ref2_must, index, minus > {};
   struct ref2_list : pegtl::list_must< ref2_part, dot > {};
   struct ref2_rest : pegtl::seq< ref2_list, pegtl::one< ')' > > {};
   struct ref2_must : pegtl::if_must< pegtl::one< '(' >, ref2_list, pegtl::one< ')' > > {};
   struct ref2_rule : pegtl::seq< pegtl::one< '(' >, ref2_list, pegtl::one< ')' > > {};

}  // namespace tao::config::internal::rules

#endif
