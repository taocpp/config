// Copyright (c) 2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_HPP
#define TAO_CONFIG_SCHEMA_HPP

#include "schema/parse_file.hpp"
#include "schema/parse_input.hpp"

namespace tao::config::schema
{
   inline json::value parse_and_validate( const std::string& schema_filename, const value& config_value )
   {
      return parse_file( schema_filename ).validate( config_value );
   }

}  // namespace tao::config::schema

#endif
