// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_BUILTIN_HPP
#define TAO_CONFIG_SCHEMA_BUILTIN_HPP

#include <tao/pegtl/contrib/integer.hpp>
#include <tao/pegtl/contrib/json_pointer.hpp>
#include <tao/pegtl/contrib/uri.hpp>

#include "internal/array.hpp"
#include "internal/binary.hpp"
#include "internal/boolean.hpp"
#include "internal/grammar.hpp"
#include "internal/is_integer.hpp"
#include "internal/is_regex.hpp"
#include "internal/is_unsigned.hpp"
#include "internal/node.hpp"
#include "internal/null.hpp"
#include "internal/number.hpp"
#include "internal/object.hpp"
#include "internal/string.hpp"

namespace tao::config::schema
{
   struct builtin
   {
      internal::node_map m_nodes;

      template< typename T >
      void add( const char* name )
      {
         value source;
         source.key = name;
         source.position = json::position( "<builtin>", 0, 0 );
         m_nodes.emplace( name, std::make_shared< T >( source ) );
      }

      builtin()
      {
         add< internal::null >( "null" );
         add< internal::boolean >( "boolean" );
         add< internal::number >( "number" );
         add< internal::string >( "string" );
         add< internal::binary >( "binary" );
         add< internal::array >( "array" );
         add< internal::object >( "object" );

         add< internal::is_integer >( "std.integer" );
         add< internal::is_unsigned >( "std.unsigned" );
         add< internal::is_regex >( "std.regex" );
         {
            using namespace pegtl;

            add< internal::grammar< identifier > >( "std.identifier" );
            add< internal::grammar< pegtl::list< identifier, one< '.' > > > >( "std.key" );

            add< internal::grammar< json_pointer::json_pointer > >( "std.json_pointer" );
            add< internal::grammar< json_pointer::relative_json_pointer > >( "std.relative_json_pointer" );

            using namespace uri;
            add< internal::grammar< URI > >( "std.net.uri" );
            add< internal::grammar< URI_reference > >( "std.net.uri_reference" );

            add< internal::grammar< IPv4address > >( "std.net.ip_v4_address" );
            add< internal::grammar< IPv6address > >( "std.net.ip_v6_address" );
            add< internal::grammar< sor< IPv4address, IPv6address > > >( "std.net.ip_address" );

            using ip_v4_cidr = seq< IPv4address, one< '/' >, maximum_rule< std::uint8_t, 32 > >;
            using ip_v6_cidr = seq< IPv6address, one< '/' >, maximum_rule< std::uint8_t, 128 > >;
            add< internal::grammar< ip_v4_cidr > >( "std.net.ip_v4_cidr" );
            add< internal::grammar< ip_v6_cidr > >( "std.net.ip_v6_cidr" );
            add< internal::grammar< sor< ip_v4_cidr, ip_v6_cidr > > >( "std.net.ip_cidr" );
         }
      }
   };

}  // namespace tao::config::schema

#endif
