// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_PARSE_FILES_HPP
#define TAO_CONFIG_PARSE_FILES_HPP

#include <string>
#include <vector>

#include "internal/parse_file.hpp"
#include "internal/phase2.hpp"

#include "value.hpp"

namespace tao
{
   namespace config
   {
      template< template< typename... > class Traits >
      json::basic_value< Traits > basic_parse_files( const std::vector< std::string >& filenames )
      {
         internal::state st;
         for( const auto& filename : filenames ) {
            internal::parse_file_impl( st, filename );
         }
         return internal::phase2< Traits >( st.result );
      }

      inline value parse_files( const std::vector< std::string >& filenames )
      {
         return basic_parse_files< traits >( filenames );
      }

   }  // namespace config

}  // namespace tao

#endif
