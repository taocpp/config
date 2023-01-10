// Copyright (c) 2020-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_REFERENCE2_GRAMMAR_HPP
#define TAO_CONFIG_INTERNAL_REFERENCE2_GRAMMAR_HPP

#include "key_grammar.hpp"
#include "pegtl.hpp"

namespace tao::config::internal::rules
{
   struct reference2_must;

   // clang-format off
   struct reference2_part : pegtl::sor< ident, quoted, index, reference2_must > {};
   struct reference2_list : pegtl::list_must< reference2_part, dot > {};
   struct reference2_rest : pegtl::seq< reference2_list, pegtl::one< ')' > > {};  // For config_grammar.hpp -- used without actions.
   struct reference2_must : pegtl::if_must< pegtl::one< '(' >, reference2_list, pegtl::one< ')' > > {};
   struct reference2_rule : pegtl::if_must< pegtl::one< '(' >, reference2_list, pegtl::one< ')' > > {};
   // clang-format on

}  // namespace tao::config::internal::rules

#endif
