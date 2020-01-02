// Copyright (c) 2019-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_FROM_INPUT_HPP
#define TAO_CONFIG_FROM_INPUT_HPP

#include <utility>

#include "value.hpp"

#include "schema/builtin.hpp"

#include "internal/configurator.hpp"

namespace tao::config
{
   template< template< typename... > class Traits >
   json::basic_value< Traits > basic_from_input( pegtl_input_t&& in, schema::builtin b = schema::builtin() )
   {
      return internal::configurator().parse( std::move( in ) ).process< Traits >( std::move( b ) );
   }

   inline value from_input( pegtl_input_t&& in, schema::builtin b = schema::builtin() )
   {
      return basic_from_input< traits >( std::move( in ), std::move( b ) );
   }

}  // namespace tao::config

#endif
