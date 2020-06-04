// Copyright (c) 2019-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_FROM_FILE_HPP
#define TAO_CONFIG_SCHEMA_FROM_FILE_HPP

#include <filesystem>
#include <utility>

#include "from_input.hpp"

namespace tao::config::schema
{
   inline validator from_file( const std::filesystem::path& path, builtin b = builtin() )
   {
      return from_input( pegtl::file_input( path ), std::move( b ) );
   }

}  // namespace tao::config::schema

#endif
