// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_PARSE_FILE_HPP
#define TAO_CONFIG_PARSE_FILE_HPP

#include <string>

#include "internal/parse_file.hpp"
#include "internal/phase2.hpp"

#include "value.hpp"

namespace tao
{
   namespace config
   {
      template< template< typename... > class Traits >
      json::basic_value< Traits > basic_parse_file( const std::string& filename )
      {
         internal::state st;
         internal::parse_file_impl( st, filename );
         return internal::phase2< Traits >( st.result );
      }

      inline value parse_file( const std::string& filename )
      {
         return basic_parse_file< traits >( filename );
      }

   }  // namespace config

}  // namespace tao

#endif
