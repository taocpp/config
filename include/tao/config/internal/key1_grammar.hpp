// Copyright (c) 2020-2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_KEY1_GRAMMAR_HPP
#define TAO_CONFIG_INTERNAL_KEY1_GRAMMAR_HPP

#include "key_grammar.hpp"
#include "pegtl.hpp"

namespace tao::config::internal::rules
{
   // clang-format off
   struct asterisk : pegtl::one< '*' > {};

   struct key1_part : pegtl::sor< ident, quoted, index, asterisk > {};
   struct key1_rule : pegtl::list_must< key1_part, dot > {};
   // clang-format on

}  // namespace tao::config::internal::rules

#endif
