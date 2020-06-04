// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_FROM_FILES_HPP
#define TAO_CONFIG_FROM_FILES_HPP

#include <filesystem>
#include <utility>
#include <vector>

#include "value.hpp"

#include "schema/builtin.hpp"

#include "internal/configurator.hpp"

namespace tao::config
{
   template< template< typename... > class Traits >
   json::basic_value< Traits > basic_from_files( const std::vector< std::filesystem::path >& paths, schema::builtin b = schema::builtin() )
   {
      internal::configurator c;
      for( const auto& path : paths ) {
         c.parse( pegtl::file_input( path ) );
      }
      return c.process< Traits >( std::move( b ) );
   }

   inline value from_files( const std::vector< std::filesystem::path >& paths, schema::builtin b = schema::builtin() )
   {
      return basic_from_files< traits >( paths, std::move( b ) );
   }

}  // namespace tao::config

#endif
