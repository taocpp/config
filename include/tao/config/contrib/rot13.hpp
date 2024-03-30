// Copyright (c) 2022-2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_CONTRIB_ROT13_HPP
#define TAO_CONFIG_CONTRIB_ROT13_HPP

#include <string>

#include "../internal/atom.hpp"
#include "../internal/pegtl.hpp"

namespace tao::config
{
   [[nodiscard]] inline internal::string_t rot13( const pegtl::position& pos, const std::string& in )
   {
      internal::string_t out( in, pos );

      for( char& c : out.value ) {
         if( ( ( 'a' <= c ) && ( c <= 'm' ) ) || ( ( 'A' <= c ) && ( c <= 'M' ) ) ) {
            c += 13;
         }
         else if( ( ( 'n' <= c ) && ( c <= 'z' ) ) || ( ( 'N' <= c ) && ( c <= 'Z' ) ) ) {
            c -= 13;
         }
      }
      return out;
   }

}  // namespace tao::config

#endif
