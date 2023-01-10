// Copyright (c) 2020-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_EXTENSION_GRAMMAR_HPP
#define TAO_CONFIG_INTERNAL_EXTENSION_GRAMMAR_HPP

#include "json.hpp"
#include "key_grammar.hpp"
#include "pegtl.hpp"

namespace tao::config::internal::rules
{
   // clang-format off
   struct extension_name : pegtl::seq< ident, pegtl::opt< pegtl::one< '?' > > > {};
   struct extension_rule : pegtl::must< extension_name, wsp > {};
   // clang-format on

}  // namespace tao::config::internal::rules

#endif
