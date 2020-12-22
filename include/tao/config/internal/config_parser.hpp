// Copyright (c) 2019-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_CONFIG_PARSER_HPP
#define TAO_CONFIG_INTERNAL_CONFIG_PARSER_HPP

#include <filesystem>

#include "config_action.hpp"
#include "config_grammar.hpp"
#include "extension_maps.hpp"
#include "inner_functions.hpp"
#include "json.hpp"
#include "json_traits.hpp"
#include "member_functions.hpp"
#include "pegtl.hpp"
#include "state.hpp"
#include "value_functions.hpp"

// #include "../schema/builtin.hpp"

namespace tao::config::internal
{
   struct config_parser
   {
      config_parser()
         : em( { { "binary", wrap( binary_function ) },
                 { "cbor", wrap( cbor_function ) },
                 { "default", wrap( default_function ) },
                 { "env", wrap( env_function ) },
                 { "env?", wrap( env_if_function ) },
                 { "identity", wrap( identity_function ) },
                 { "jaxn", wrap( jaxn_function ) },
                 { "json", wrap( json_function ) },
                 { "msgpack", wrap( msgpack_function ) },
                 { "read", wrap( read_function ) },
                 { "shell", wrap( shell_function ) },
                 { "split", wrap( split_function ) },
                 { "string", wrap( string_function ) },
                 { "ubjson", wrap( ubjson_function ) } },
               { { "delete", wrap( delete_function ) },
                 { "include", wrap( include_function ) },
                 { "include?", wrap( include_if_function ) },
                 { "schema", wrap( schema_function ) },
                 { "setenv", wrap( setenv_function ) },
                 { "temporary", wrap( temporary_function ) } },
               { { "copy", wrap( copy_function ) },
                 { "parse", wrap( parse_function ) } } )
      {}

      config_parser( config_parser&& ) = delete;
      config_parser( const config_parser& ) = delete;

      void operator=( config_parser&& ) = delete;
      void operator=( const config_parser& ) = delete;

      state st;
      extension_maps em;

      void parse( pegtl_input_t&& in )
      {
         pegtl::parse< rules::config_file, config_action >( in, st, em );
      }

      void parse( const std::filesystem::path& path )
      {
         parse( pegtl::file_input( path ) );
      }

      // template< template< typename... > class Traits >
      // [[nodiscard]] json::basic_value< Traits > process( schema::builtin b )
      // {
      //    return internal::phase2< Traits >( st, std::move( b ) );
      // }
   };

}  // namespace tao::config::internal

#endif
