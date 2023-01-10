// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_INTERNAL_NODE_UTILITY_HPP
#define TAO_CONFIG_SCHEMA_INTERNAL_NODE_UTILITY_HPP

#include <sstream>
#include <string>
#include <utility>

#include "../../value.hpp"

namespace tao::config::schema::internal
{
   inline const value& find( const value& v, const std::string_view k )
   {
      static const value nope;
      const auto& o = v.get_object();
      const auto it = o.find( k );
      return ( it != o.end() ) ? it->second : nope;
   }

   inline std::string pos( const json::position& p )
   {
      std::ostringstream oss;
      p.append_message_extension( oss );
      return std::move( oss ).str();
   }

   inline json::value pos( const value& v )
   {
      return { { "key", config::to_string( v.key ) }, { "pos", pos( v.position ) } };
   }

   inline json::value append_via( json::value& result, json::value&& via )
   {
      auto& a = result[ "via" ];
      if( !a ) {
         a.emplace_back( std::move( via ) );
      }
      else {
         auto& l = a.get_array().back();
         auto sv = via.at( "key" ).as< std::string_view >();
         if( l.at( "key" ).as< std::string_view >().substr( 0, sv.size() ) == sv ) {
            l = std::move( via );
         }
         else {
            a.emplace_back( std::move( via ) );
         }
      }
      return result;
   }

   inline json::value ok()
   {
      return json::value();
   }

}  // namespace tao::config::schema::internal

#endif
