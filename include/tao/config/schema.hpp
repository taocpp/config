// Copyright (c) 2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_HPP
#define TAO_CONFIG_SCHEMA_HPP

#include "parse_file.hpp"
#include "schema_validator.hpp"
#include "value.hpp"

namespace tao::config::schema
{
   inline validator read( const std::string& filename, builtin b = builtin() )
   {
      const value data = config::parse_file( filename );
      if( const auto error = schema_validator.validate( data ) ) {
         std::cerr << std::setw( 2 ) << error << std::endl;
         throw std::runtime_error( "invalid schema file '" + filename + "'" );  // TODO: store error in exception
      }
      return validator( data, std::move( b ) );
   }

   inline json::value read_and_validate( const std::string& filename, const value& v )
   {
      return read( filename ).validate( v );
   }

}  // namespace tao::config::schema

#endif
