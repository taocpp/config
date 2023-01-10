// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_FROM_INPUT_HPP
#define TAO_CONFIG_SCHEMA_FROM_INPUT_HPP

#include <iostream>
#include <stdexcept>

#include "../from_input.hpp"

#include "builtin.hpp"
#include "validator.hpp"

#include "internal/validator.hpp"

namespace tao::config::schema
{
   inline validator from_input( pegtl_input_t&& in, const builtin& b = builtin() )
   {
      const config::value s = config::from_input( std::move( in ) );

      if( const auto error = tao::config::schema::internal::validator.validate( s ) ) {
         std::cerr << std::setw( 2 ) << error << std::endl;                                       // TODO: Remove this line...
         throw std::runtime_error( std::string( "invalid schema from '" ) + in.source() + "'" );  // TODO: ...and store error in exception.
      }
      return validator( s, b );
   }

}  // namespace tao::config::schema

#endif
