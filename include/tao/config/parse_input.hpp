// Copyright (c) 2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_PARSE_INPUT_HPP
#define TAO_CONFIG_PARSE_INPUT_HPP

#include <utility>

#include "value.hpp"

#include "schema/builtin.hpp"

#include "internal/configurator.hpp"

namespace tao::config
{
   template< template< typename... > class Traits >
   json::basic_value< Traits > basic_parse_input( pegtl_input_t&& in, schema::builtin b = schema::builtin() )
   {
      return internal::configurator().parse( std::move( in ) ).process< Traits >( std::move( b ) );
   }

   inline value parse_input( pegtl_input_t&& in, schema::builtin b = schema::builtin() )
   {
      return basic_parse_input< traits >( std::move( in ), std::move( b ) );
   }

}  // namespace tao::config

#endif
