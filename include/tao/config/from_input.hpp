// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_FROM_INPUT_HPP
#define TAO_CONFIG_FROM_INPUT_HPP

#include <utility>

#include "internal/config_parser.hpp"
#include "internal/pegtl.hpp"
#include "schema/builtin.hpp"
#include "value.hpp"

namespace tao::config
{
   template< template< typename... > class Traits >
   [[nodiscard]] json::basic_value< Traits > basic_from_input( pegtl_input_t&& in, const schema::builtin& b = schema::builtin() )
   {
      internal::config_parser c;
      c.parse( std::move( in ) );
      return c.finish< Traits >( b );
   }

   [[nodiscard]] inline value from_input( pegtl_input_t&& in, const schema::builtin& b = schema::builtin() )
   {
      return basic_from_input< traits >( std::move( in ), b );
   }

}  // namespace tao::config

#endif
