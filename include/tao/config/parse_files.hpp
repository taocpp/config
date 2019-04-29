// Copyright (c) 2018-2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_PARSE_FILES_HPP
#define TAO_CONFIG_PARSE_FILES_HPP

#include <string>
#include <vector>

#include "value.hpp"

#include "internal/action.hpp"
#include "internal/control.hpp"
#include "internal/grammar.hpp"
#include "internal/pegtl.hpp"
#include "internal/phase2.hpp"
#include "internal/state.hpp"

#include "internal/member_extensions.hpp"
#include "internal/value_extensions.hpp"

namespace tao::config
{
   template< template< typename... > class Traits >
   json::basic_value< Traits > basic_parse_files( const std::vector< std::string >& filenames )
   {
      internal::state st;
      for( const auto& filename : filenames ) {
         pegtl::file_input in( filename );
         pegtl::parse< internal::rules::grammar, internal::action, internal::control >( in, st );
      }
      return internal::phase2< Traits >( filenames.empty() ? std::string() : filenames[ 0 ], st );
   }

   inline value parse_files( const std::vector< std::string >& filenames )
   {
      return basic_parse_files< traits >( filenames );
   }

}  // namespace tao::config

#endif
