// Copyright (c) 2018-2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_PARSE_FILE_HPP
#define TAO_CONFIG_PARSE_FILE_HPP

#include <string>

#include "value.hpp"

#include "internal/action.hpp"
#include "internal/control.hpp"
#include "internal/grammar.hpp"
#include "internal/pegtl.hpp"
#include "internal/phase2.hpp"
#include "internal/state.hpp"

#include "internal/member_extensions.hpp"
#include "internal/value_extensions.hpp"

namespace tao
{
   namespace config
   {
      template< template< typename... > class Traits >
      json::basic_value< Traits > basic_parse_file( const std::string& filename )
      {
         internal::state st;
         json_pegtl::file_input in( filename );
         json_pegtl::parse< internal::rules::grammar, internal::action, internal::control >( in, st );
         return internal::phase2< Traits >( st );
      }

      inline value parse_file( const std::string& filename )
      {
         return basic_parse_file< traits >( filename );
      }

   }  // namespace config

}  // namespace tao

#endif
