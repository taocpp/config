// Copyright (c) 2018-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_FROM_FILES_HPP
#define TAO_CONFIG_FROM_FILES_HPP

#include <filesystem>
#include <utility>
#include <vector>

#include "internal/config_parser.hpp"
#include "schema/builtin.hpp"
#include "value.hpp"

namespace tao::config
{
   template< template< typename... > class Traits >
   [[nodiscard]] json::basic_value< Traits > basic_from_files( const std::vector< std::filesystem::path >& paths, const schema::builtin& b = schema::builtin() )
   {
      internal::config_parser c;
      for( const auto& path : paths ) {
         c.parse( path );
      }
      return c.finish< Traits >( b );
   }

   [[nodiscard]] inline value from_files( const std::vector< std::filesystem::path >& paths, const schema::builtin& b = schema::builtin() )
   {
      return basic_from_files< traits >( paths, b );
   }

}  // namespace tao::config

#endif
