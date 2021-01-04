// Copyright (c) 2019-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_CONFIG_PARSER_HPP
#define TAO_CONFIG_INTERNAL_CONFIG_PARSER_HPP

#include <filesystem>
#include <ostream>

#include "config_action.hpp"
#include "config_grammar.hpp"
#include "extension_maps.hpp"
#include "inner_functions.hpp"
#include "json.hpp"
#include "json_traits.hpp"
#include "member_functions.hpp"
#include "pegtl.hpp"
#include "phase2_combine.hpp"
#include "phase2_resolve.hpp"
#include "phase3_remove.hpp"
#include "phase4_repack.hpp"
#include "state.hpp"
#include "to_stream.hpp"
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
               { { "include", wrap( include_function ) },
                 { "include?", wrap( include_if_function ) },
                 { "parse", wrap( member_function ) },
                 { "schema", wrap( schema_function ) },
                 { "setenv", wrap( setenv_function ) } },
               { { "parse", wrap( value_function ) } } )
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

      void parse( const std::string& data, const std::string& source )
      {
         parse( pegtl::memory_input( data, source ) );
      }

      unsigned phase2()
      {
         unsigned r = 0;

         while( ( phase2_combine( st.root ) > 0 ) || ( phase2_resolve( st.root ) > 0 ) ) {
            ++r;
         }
         return r;
      }

      void phase3()
      {
         phase3_remove( st.root );
      }

      template< template< typename... > class Traits >
      [[nodiscard]] json::basic_value< Traits > finish()
      {
         phase2();
         phase3();
         // TODO: Check config schema(s).
         return phase4_repack< Traits >( st.root );
      }
   };

}  // namespace tao::config::internal

#endif
