// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_KEY_UTILITY_HPP
#define TAO_CONFIG_INTERNAL_KEY_UTILITY_HPP

#include "../key.hpp"

#include "format.hpp"
#include "forward.hpp"
#include "json.hpp"
#include "value_traits.hpp"

namespace tao::config::internal
{
   inline part value_to_part( const json_t& v )
   {
      switch( v.type() ) {
         case json::type::BOOLEAN:
            return v.get_boolean() ? part( part::star_t() ) : part( part::minus_t() );
         case json::type::STRING:
         case json::type::STRING_VIEW:
            return part( v.template as< std::string >() );
         case json::type::SIGNED:
         case json::type::UNSIGNED:
            return part( v.template as< std::size_t >() );
         default:
            throw pegtl::parse_error( format( __FILE__, __LINE__, "invalid json for part -- expected string or integer (or bool)", { v.type() } ), *v.position );
      }
   }

   inline key value_to_key( json_t& v )
   {
      key p;

      if( v.is_string() ) {
         p.emplace_back( v.get_string() );
         v.set_uninitialized();
         return p;
      }
      if( !v.is_array() ) {
         throw pegtl::parse_error( format( __FILE__, __LINE__, "invalid json for key -- expected array", { v.type() } ), *v.position );
      }
      for( const auto& t : v.get_array() ) {
         p.emplace_back( value_to_part( t ) );
      }
      v.set_uninitialized();
      return p;
   }

   inline json::pointer key_to_pointer( const key& k )
   {
      json::pointer p;

      for( const auto& i : k ) {
         switch( i.type() ) {
            case part::star:
            case part::minus:
               assert( false );
            case part::name:
               p += i.get_name();
               continue;
            case part::index:
               p += std::size_t( i.get_index() );
               continue;
         }
         assert( false );
      }
      return p;
   }

}  // namespace tao::config::internal

#endif
