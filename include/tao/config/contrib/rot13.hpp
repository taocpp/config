// Copyright (c) 2022-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_CONTRIB_ROT13_HPP
#define TAO_CONFIG_CONTRIB_ROT13_HPP

#include <string>

namespace tao::config
{
   [[nodiscard]] std::string rot13( const std::string& in )
   {
      std::string out( in );

      for( char& c : out ) {
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
