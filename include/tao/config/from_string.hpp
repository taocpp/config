// Copyright (c) 2018-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_FROM_STRING_HPP
#define TAO_CONFIG_FROM_STRING_HPP

#include <string>
#include <string_view>
#include <utility>

#include "internal/config_parser.hpp"
#include "schema/builtin.hpp"
#include "value.hpp"

namespace tao::config
{
   template< template< typename... > class Traits >
   [[nodiscard]] json::basic_value< Traits > basic_from_string( const char* data, const std::size_t size, const std::string& source, const schema::builtin& b = schema::builtin() )
   {
      internal::config_parser c;
      c.parse( data, size, source );
      return c.finish< Traits >( b );
   }

   template< template< typename... > class Traits >
   [[nodiscard]] json::basic_value< Traits > basic_from_string( const std::string_view data, const std::string& source, const schema::builtin& b = schema::builtin() )
   {
      return basic_from_string< Traits >( data.data(), data.size(), source, b );
   }

   [[nodiscard]] inline value from_string( const char* data, const std::size_t size, const std::string& source, const schema::builtin& b = schema::builtin() )
   {
      return basic_from_string< traits >( data, size, source, b );
   }

   [[nodiscard]] inline value from_string( const std::string_view data, const std::string& source, const schema::builtin& b = schema::builtin() )
   {
      return from_string( data.data(), data.size(), source, b );
   }

}  // namespace tao::config

#endif
