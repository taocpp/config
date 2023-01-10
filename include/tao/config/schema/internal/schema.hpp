// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_INTERNAL_SCHEMA_HPP
#define TAO_CONFIG_SCHEMA_INTERNAL_SCHEMA_HPP

#include "all_of.hpp"
#include "any_of.hpp"
#include "compare.hpp"
#include "constant.hpp"
#include "has_property.hpp"
#include "if_then_else.hpp"
#include "istring.hpp"
#include "items.hpp"
#include "list.hpp"
#include "multiple_of.hpp"
#include "not_ref.hpp"
#include "one_of.hpp"
#include "pattern.hpp"
#include "properties.hpp"
#include "property.hpp"
#include "property_names.hpp"
#include "ref.hpp"
#include "size.hpp"
#include "switch_case.hpp"
#include "unique_items.hpp"

namespace tao::config::schema::internal
{
   struct schema : all_of
   {
      template< typename P, typename T, typename... Ts >
      void add( T&& t, Ts&&... ts )
      {
         if( t ) {
            m_properties.emplace_back( std::make_shared< P >( std::forward< T >( t ), std::forward< Ts >( ts )... ) );
         }
      }

      schema( const value& v, node_map& m, const std::string& path = "" )
         : all_of( v )
      {
         // definitions
         if( const auto& d = internal::find( v, "definitions" ) ) {
            for( const auto& e : d.get_object() ) {
               assert( is_identifier( e.first ) );
               auto p = path.empty() ? e.first : ( path + '.' + e.first );
               auto n = std::make_shared< ref >( e.second, m, p );
               if( !m.emplace( p, std::move( n ) ).second ) {
                  std::ostringstream oss;
                  oss << "type '" << p << "' already defined, redefined here:";
                  internal::find( d, e.first ).append_message_extension( oss );
                  throw std::runtime_error( std::move( oss ).str() );
               }
            }
         }

         // structural
         add< ref >( internal::find( v, "type" ), m, path );
         add< not_ref >( internal::find( v, "not" ), m, path );

         add< list< all_of, ref > >( internal::find( v, "all_of" ), m, path );
         add< list< any_of, ref > >( internal::find( v, "any_of" ), m, path );
         add< list< one_of, ref > >( internal::find( v, "one_of" ), m, path );

         if( const auto& n = internal::find( v, "if" ) ) {
            auto p = std::make_shared< if_then_else >( n, m, path );
            if( const auto& t = internal::find( v, "then" ) ) {
               p->m_then = std::make_shared< ref >( t, m, path );
            }
            if( const auto& e = internal::find( v, "else" ) ) {
               p->m_else = std::make_shared< ref >( e, m, path );
            }
            m_properties.emplace_back( std::move( p ) );
         }

         // any
         add< constant >( internal::find( v, "value" ) );
         add< list< any_of, constant > >( internal::find( v, "enum" ) );

         // string/binary/array/object
         add< size >( internal::find( v, "size" ) );
         add< min_size >( internal::find( v, "min_size" ) );
         add< max_size >( internal::find( v, "max_size" ) );

         // string
         add< istring >( internal::find( v, "istring" ) );
         add< pattern >( internal::find( v, "pattern" ) );
         add< switch_case >( internal::find( v, "switch" ), m, path );

         // number
         add< minimum >( internal::find( v, "minimum" ) );
         add< maximum >( internal::find( v, "maximum" ) );
         add< exclusive_minimum >( internal::find( v, "exclusive_minimum" ) );
         add< exclusive_maximum >( internal::find( v, "exclusive_maximum" ) );
         add< multiple_of >( internal::find( v, "multiple_of" ) );

         // array
         add< items >( internal::find( v, "items" ), m, path );
         if( const auto& n = internal::find( v, "unique_items" ) ) {
            if( n.as< bool >() ) {
               m_properties.emplace_back( std::make_shared< unique_items >( v ) );
            }
         }

         // object
         add< property_names >( internal::find( v, "property_names" ), m, path );
         add< has_property >( internal::find( v, "has_property" ) );
         add< property >( internal::find( v, "property" ), m, path );
         if( const auto& p = internal::find( v, "properties" ) ) {
            auto n = std::make_shared< properties >( v );
            if( const auto& r = internal::find( p, "required" ) ) {
               n->add_required( r, m, path );
            }
            if( const auto& o = internal::find( p, "optional" ) ) {
               n->add_optional( o, m, path );
            }
            if( const auto& a = internal::find( p, "additional" ) ) {
               n->m_default = std::make_shared< ref >( a, m, path );
            }
            m_properties.emplace_back( std::move( n ) );
         }
      }
   };

}  // namespace tao::config::schema::internal

#include "ref_impl.hpp"

#endif
