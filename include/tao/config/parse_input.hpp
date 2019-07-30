// Copyright (c) 2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_PARSE_INPUT_HPP
#define TAO_CONFIG_PARSE_INPUT_HPP

#include <utility>

#include "value.hpp"

#include "internal/configurator.hpp"

namespace tao::config
{
   template< template< typename... > class Traits >
   json::basic_value< Traits > basic_parse_input( pegtl_input_t&& in )
   {
      return internal::configurator().parse( std::move( in ) ).process< Traits >();
   }

   inline value parse_input( pegtl_input_t&& in )
   {
      return basic_parse_input< traits >( std::move( in ) );
   }

}  // namespace tao::config

#endif
