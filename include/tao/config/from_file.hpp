// Copyright (c) 2018-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_FROM_FILE_HPP
#define TAO_CONFIG_FROM_FILE_HPP

#include <filesystem>
#include <utility>

#include "internal/config_parser.hpp"
#include "schema/builtin.hpp"
#include "value.hpp"

namespace tao::config
{
   template< template< typename... > class Traits >
   [[nodiscard]] json::basic_value< Traits > basic_from_file( const std::filesystem::path& path, const schema::builtin& b = schema::builtin() )
   {
      internal::config_parser c;
      c.parse( path );
      return c.finish< Traits >( b );
   }

   [[nodiscard]] inline value from_file( const std::filesystem::path& path, const schema::builtin& b = schema::builtin() )
   {
      return basic_from_file< traits >( path, b );
   }

}  // namespace tao::config

#endif
