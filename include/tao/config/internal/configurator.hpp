// Copyright (c) 2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_CONFIGURATOR_HPP
#define TAO_CONFIG_INTERNAL_CONFIGURATOR_HPP

#include "action.hpp"
#include "control.hpp"
#include "grammar.hpp"
#include "json.hpp"
#include "pegtl.hpp"
#include "phase2.hpp"
#include "state.hpp"

#include "member_extensions.hpp"
#include "value_extensions.hpp"

namespace tao::config::internal
{
   struct configurator
   {
      configurator()
         : st( member_extensions, value_extensions )
      {
      }

      extension_map_t member_extensions = {
         { "delete", internal::erase_extension },
         { "delete?", internal::erase_if_extension },
         { "include", internal::include_extension },
         { "include?", internal::include_if_extension },
         { "stderr", internal::stderr_extension },
         { "temporary", internal::temporary_extension }
      };
      extension_map_t value_extensions = {
         { "cbor", internal::cbor_extension },
         // "copy" does not return a single value.
         { "debug", internal::debug_extension },
         { "env", internal::env_extension },
         { "env?", internal::env_if_extension },
         { "jaxn", internal::jaxn_extension },
         { "json", internal::json_extension },
         { "msgpack", internal::msgpack_extension },
         // "parse" does not return a single value.
         { "read", internal::read_extension },
         { "shell", internal::shell_extension },
         { "split", internal::split_extension },
         { "ubjson", internal::ubjson_extension }
      };
      state st;

      void parse_only( pegtl_input_t&& in )
      {
         pegtl::parse< rules::config_file, action, control >( in, st );
      }

      template< template< typename... > class Traits >
      json::basic_value< Traits > process_only()
      {
         return internal::phase2< Traits >( st );
      }

      template< template< typename... > class Traits >
      json::basic_value< Traits > parse_and_process( pegtl_input_t&& in )
      {
         parse_only( std::move( in ) );
         return process_only< Traits >();
      }
   };

}  // namespace tao::config::internal

#endif
