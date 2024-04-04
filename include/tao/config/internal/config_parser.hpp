// Copyright (c) 2019-2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_CONFIG_PARSER_HPP
#define TAO_CONFIG_INTERNAL_CONFIG_PARSER_HPP

#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>
#include <utility>

#include "config_action.hpp"
#include "config_grammar.hpp"
#include "forward.hpp"
#include "function_implementations.hpp"
#include "function_wrapper.hpp"
#include "json.hpp"
#include "pegtl.hpp"
#include "phase2_everything.hpp"
#include "phase3_remove.hpp"
#include "phase5_repack.hpp"
#include "state.hpp"

namespace tao::config::internal
{
   struct config_parser
   {
      config_parser()
         : fm( { { "binary", wrap( binary_function ) },
                 { "default", wrap( default_function ) },
                 { "env", wrap( env_function ) },
                 { "env?", wrap( env_if_function ) },
                 { "jaxn", wrap( jaxn_function ) },
                 { "print", wrap( print_function ) },
                 { "read", wrap( read_function ) },
                 { "shell", wrap( shell_function ) },
                 { "split", wrap( split_function ) },
                 { "string", wrap( string_function ) } } )
      {}

      config_parser( config_parser&& ) = delete;
      config_parser( const config_parser& ) = delete;

      void operator=( config_parser&& ) = delete;
      void operator=( const config_parser& ) = delete;

      state st;
      function_map fm;

      void parse( pegtl_input_t&& in )
      {
         pegtl::parse< rules::config_file, config_action >( in, st, fm );
      }

      void parse( const std::filesystem::path& path )
      {
         parse( pegtl::file_input( path ) );
      }

      void parse( const char* data, const std::size_t size, const std::string& source )
      {
         parse( pegtl_input_t( data, size, source ) );
      }

      void parse( const std::string_view data, const std::string& source )
      {
         parse( data.data(), data.size(), source );
      }

      template< template< typename... > class Traits >
      [[nodiscard]] json::basic_value< Traits > finish()
      {
         phase2_everything( st, fm );
         phase3_remove( st.root );
         return phase5_repack< Traits >( st.root );
      }
   };

}  // namespace tao::config::internal

#endif
