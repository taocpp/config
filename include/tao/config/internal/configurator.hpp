// Copyright (c) 2019-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_CONFIGURATOR_HPP
#define TAO_CONFIG_INTERNAL_CONFIGURATOR_HPP

#include <filesystem>

#include "action.hpp"
#include "control.hpp"
#include "grammar.hpp"
#include "json.hpp"
#include "pegtl.hpp"
#include "phase2.hpp"
#include "state.hpp"

#include "inner_extensions.hpp"
#include "member_extensions.hpp"
#include "value_extensions.hpp"

#include "../schema/builtin.hpp"

namespace tao::config::internal
{
   struct configurator
   {
      configurator()
         : st( member_extensions, value_extensions )
      {
      }

      configurator( configurator&& ) = delete;
      configurator( const configurator& ) = delete;

      void operator=( configurator&& ) = delete;
      void operator=( const configurator& ) = delete;

      // NOTE: This is probably NOT a customisation point.
      const extension_map_t member_extensions = {
         { "delete", internal::erase_extension },
         { "delete?", internal::erase_if_extension },
         { "include", internal::include_extension },
         { "include?", internal::include_if_extension },
         { "schema", internal::schema_extension },
         { "setenv", internal::setenv_extension },
         { "stderr", internal::stderr_extension },
         { "temporary", internal::temporary_extension }
      };

      // TODO: This is probably a future customisation point.
      extension_map_t value_extensions = {
         { "binary", internal::make_extension( internal::binary_function ) },
         { "cbor", internal::make_extension( internal::cbor_function ) },
         // "copy" does not return a single value.
         { "debug", internal::debug_extension },
         { "env", internal::env_extension },
         { "env?", internal::env_if_extension },
         { "jaxn", internal::make_extension( internal::jaxn_function ) },
         { "json", internal::make_extension( internal::json_function ) },
         { "msgpack", internal::make_extension( internal::msgpack_function ) },
         // "parse" does not return a single value.
         { "read", internal::read_extension },
         { "shell", internal::shell_extension },
         { "split", internal::split_extension },
         { "string", internal::make_extension( internal::string_function ) },
         { "ubjson", internal::make_extension( internal::ubjson_function ) }
      };

      state st;

      configurator& parse( pegtl_input_t&& in )
      {
         pegtl::parse< rules::config_file, action, control >( in, st );
         return *this;
      }

      configurator& parse( const std::filesystem::path& path )
      {
         parse( pegtl::file_input( path ) );
         return *this;
      }

      template< template< typename... > class Traits >
      json::basic_value< Traits > process( schema::builtin b )
      {
         return internal::phase2< Traits >( st, std::move( b ) );
      }
   };

}  // namespace tao::config::internal

#endif
