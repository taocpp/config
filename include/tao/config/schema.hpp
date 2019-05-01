// Copyright (c) 2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_HPP
#define TAO_CONFIG_SCHEMA_HPP

#include <cmath>
#include <map>
#include <memory>
#include <regex>
#include <sstream>
#include <string>
#include <string_view>
#include <strings.h>
#include <utility>
#include <vector>

#include "parse_file.hpp"
#include "parse_input.hpp"
#include "value.hpp"

namespace tao::config
{
   namespace schema
   {
      namespace internal
      {
         const value& find( const value& v, const std::string& k )
         {
            static const value nope;
            const auto& o = v.get_object();
            const auto it = o.find( k );
            return ( it != o.end() ) ? it->second : nope;
         }

         std::string pos( const json::position& p )
         {
            std::ostringstream os;
            p.append_message_extension( os );
            return os.str();
         }

         json::value pos( const value& v )
         {
            return { { "key", config::to_string( v.key ) }, { "pos", pos( v.position ) } };
         }

         json::value append_via( json::value& result, json::value&& via )
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

         json::value ok()
         {
            return json::value();
         }

         struct node;
         using node_map = std::map< std::string, std::unique_ptr< node > >;

         struct node
         {
            value m_source;

            explicit node( const value& source )
               : m_source( source )
            {}

            virtual ~node() = default;

            virtual void resolve( const node_map& /*unused*/ ) {}
            virtual json::value validate( const value& v ) const = 0;

            virtual json::value pos() const
            {
               return internal::pos( m_source );
            }

            json::value error( const value& v, const char* message, json::value data = json::empty_object ) const
            {
               data.unsafe_emplace( "_message", message );
               data.unsafe_emplace( "_schema", pos() );
               data.unsafe_emplace( "_value", internal::pos( v ) );
               return data;
            }
         };

         template< bool B >
         struct trivial : node
         {
            using node::node;

            json::value validate( const value& v ) const override
            {
               return B ? ok() : error( v, "always fails" );
            }
         };

         struct null : node
         {
            using node::node;

            json::value validate( const value& v ) const override
            {
               return v.is_null() ? ok() : error( v, "expected null" );
            }
         };

         struct boolean : node
         {
            using node::node;

            json::value validate( const value& v ) const override
            {
               return v.is_boolean() ? ok() : error( v, "expected boolean" );
            }
         };

         struct number : node
         {
            using node::node;

            json::value validate( const value& v ) const override
            {
               return v.is_number() ? ok() : error( v, "expected number" );
            }
         };

         struct string : node
         {
            using node::node;

            json::value validate( const value& v ) const override
            {
               return v.is_string_type() ? ok() : error( v, "expected string" );
            }
         };

         struct binary : node
         {
            using node::node;

            json::value validate( const value& v ) const override
            {
               return v.is_binary_type() ? ok() : error( v, "expected binary" );
            }
         };

         struct array : node
         {
            using node::node;

            json::value validate( const value& v ) const override
            {
               return v.is_array() ? ok() : error( v, "expected array" );
            }
         };

         struct object : node
         {
            using node::node;

            json::value validate( const value& v ) const override
            {
               return v.is_object() ? ok() : error( v, "expected object" );
            }
         };

         struct integer : node
         {
            using node::node;

            json::value validate( const value& v ) const override
            {
               if( auto e = number( m_source ).validate( v ) ) {
                  return e;
               }
               if( v.is_double() ) {
                  const double d = v.as< double >();
                  double dummy;
                  if( std::modf( d, &dummy ) != 0 ) {
                     return error( v, "expected integer", { { "value", d } } );
                  }
               }
               return ok();
            }
         };

         struct container : node
         {
            using node::node;

            std::vector< std::unique_ptr< node > > m_properties;

            void resolve( const node_map& m ) override
            {
               for( const auto& p : m_properties ) {
                  p->resolve( m );
               }
            }

            json::value pos() const override
            {
               if( m_properties.size() == 1 ) {
                  auto result = m_properties.front()->pos();
                  append_via( result, node::pos() );
                  return result;
               }
               else {
                  return node::pos();
               }
            }
         };

         struct all_of : container
         {
            using container::container;

            json::value validate( const value& v ) const override
            {
               const auto& vs = v.skip_value_ptr();
               for( const auto& p : m_properties ) {
                  if( auto e = p->validate( vs ) ) {
                     // short-circuit
                     return e;
                  }
               }
               return ok();
            }
         };

         struct any_of : container
         {
            using container::container;

            json::value validate( const value& v ) const override
            {
               const auto& vs = v.skip_value_ptr();

               json::value errors = json::empty_array;
               for( const auto& p : m_properties ) {
                  if( auto e = p->validate( vs ) ) {
                     errors.unsafe_emplace_back( std::move( e ) );
                  }
                  else {
                     // short-circuit
                     return ok();
                  }
               }
               return error( v, "no match", { { "errors", std::move( errors ) } } );
            }
         };

         struct one_of : container
         {
            using container::container;

            json::value validate( const value& v ) const override
            {
               const auto& vs = v.skip_value_ptr();

               json::value errors = json::empty_array;
               std::vector< node* > matched;
               for( const auto& p : m_properties ) {
                  if( auto e = p->validate( vs ) ) {
                     errors.unsafe_emplace_back( std::move( e ) );
                  }
                  else {
                     matched.emplace_back( p.get() );
                  }
               }

               if( matched.size() == 1 ) {
                  return ok();
               }
               if( matched.empty() ) {
                  return error( v, "no match", { { "errors", std::move( errors ) } } );
               }
               json::value data = json::empty_array;
               for( const auto& e : matched ) {
                  data.unsafe_emplace_back( e->pos() );
               }
               return error( v, "multiple matches found", { { "matched", data } } );
            }
         };

         struct exclusive : container
         {
            using container::container;

            json::value validate( const value& v ) const override
            {
               const auto& vs = v.skip_value_ptr();

               json::value errors = json::empty_array;
               std::vector< node* > matched;
               for( const auto& p : m_properties ) {
                  if( auto e = p->validate( vs ) ) {
                     errors.unsafe_emplace_back( std::move( e ) );
                  }
                  else {
                     matched.emplace_back( p.get() );
                  }
               }

               if( matched.size() <= 1 ) {
                  return ok();
               }
               json::value data = json::empty_array;
               for( const auto& e : matched ) {
                  data.unsafe_emplace_back( e->pos() );
               }
               return error( v, "multiple matches found", { { "matched", data } } );
            }
         };

         struct ref : node
         {
            std::unique_ptr< node > m_node;
            const node* m_ptr = nullptr;

            ref( const value& v, node_map& m, const std::string& path );

            void resolve( const node_map& m ) override
            {
               if( m_node ) {
                  m_node->resolve( m );
                  m_ptr = m_node.get();
               }
               else {
                  const auto k = m_source.as< std::string >();
                  const auto it = m.find( k );
                  if( it == m.end() ) {
                     std::ostringstream os;
                     os << "can not resolve '" << k << "' defined here:";
                     m_source.append_message_extension( os );
                     throw std::runtime_error( os.str() );
                  }
                  m_ptr = it->second.get();
               }
            }

            json::value validate( const value& v ) const override
            {
               return m_ptr->validate( v );
            }

            json::value pos() const override
            {
               auto result = m_ptr->pos();
               auto via = node::pos();
               if( result != via ) {
                  append_via( result, std::move( via ) );
               }
               return result;
            }
         };

         struct not_ref : ref
         {
            using ref::ref;

            json::value validate( const value& v ) const override
            {
               return ref::validate( v ) ? ok() : error( v, "unexpected match" );
            }
         };

         struct constant : node
         {
            using node::node;

            json::value validate( const value& v ) const override
            {
               return ( v == m_source ) ? ok() : error( v, "value did not match" );
               // TODO: return ( v == m_value ) ? ok() : error( v, "value did not match", { { "actual", v }, { "expected", m_value } } );
            }
         };

         struct istring : node
         {
            std::vector< std::string_view > m_values;

            explicit istring( const value& v )
               : node( v )
            {
               if( m_source.is_array() ) {
                  for( const auto& e : m_source.unsafe_get_array() ) {
                     m_values.emplace_back( e.as< std::string_view >() );
                  }
               }
               else {
                  m_values.emplace_back( m_source.as< std::string_view >() );
               }
            }

            json::value validate( const value& v ) const override
            {
               const auto s = v.as< std::string_view >();
               for( const auto sv : m_values ) {
                  if( ( s.size() == sv.size() ) && ( ::strncasecmp( s.begin(), sv.begin(), s.size() ) == 0 ) ) {
                     return ok();
                  }
               }
               json::value candidates = json::empty_array;
               for( const auto sv : m_values ) {
                  candidates.unsafe_emplace_back( sv );
               }
               return error( v, "value did not match", { { "value", s }, { "candidates", std::move( candidates ) } } );
            }
         };

         template< typename T >
         struct compare_size : node
         {
            const std::size_t m_size;

            explicit compare_size( const value& v )
               : node( v ),
                 m_size( v.as< std::size_t >() )
            {}

            json::value validate( const value& v ) const override
            {
               if( v.is_string_type() ) {
                  return T()( m_size, v.as< std::string_view >().size() ) ? ok() : error( v, "size did not match" );
               }
               if( v.is_binary_type() ) {
                  return T()( m_size, v.as< tao::binary_view >().size() ) ? ok() : error( v, "size did not match" );
               }
               if( v.is_array() ) {
                  return T()( m_size, v.unsafe_get_array().size() ) ? ok() : error( v, "size did not match" );
               }
               if( v.is_object() ) {
                  return T()( m_size, v.unsafe_get_object().size() ) ? ok() : error( v, "size did not match" );
               }
               return error( v, "invalid type for size comparison" );
            }
         };

         using size = compare_size< std::equal_to<> >;
         using min_size = compare_size< std::less_equal<> >;
         using max_size = compare_size< std::greater_equal<> >;

         struct pattern : node
         {
            const std::regex m_regex;

            pattern( const value& v, const std::string_view sv )
               : node( v ),
                 m_regex( sv.begin(), sv.end() )
            {}

            explicit pattern( const value& v )
               : pattern( v, v.as< std::string_view >() )
            {}

            json::value validate( const value& v ) const override
            {
               if( auto e = string( m_source ).validate( v ) ) {
                  return e;
               }
               const std::string_view sv = v.as< std::string_view >();
               return std::regex_search( sv.begin(), sv.end(), m_regex ) ? ok() : error( v, "pattern did not match" );
            }
         };

         struct regex : node
         {
            using node::node;

            json::value validate( const value& v ) const override
            {
               if( auto e = string( m_source ).validate( v ) ) {
                  return e;
               }
               const std::string_view sv = v.as< std::string_view >();
               try {
                  std::regex( sv.begin(), sv.end() );
               }
               catch( const std::regex_error& ) {
                  return error( v, "invalid regular expression" );
               }
               return ok();
            }
         };

         template< typename T >
         struct compare : node
         {
            explicit compare( const value& v )
               : node( v )
            {
               assert( m_source.is_number() );
            }

            json::value validate( const value& v ) const override
            {
               if( auto e = number( m_source ).validate( v ) ) {
                  return e;
               }
               return T()( m_source, v ) ? ok() : error( v, "value did not match" );
            }
         };

         using minimum = compare< std::less_equal<> >;
         using maximum = compare< std::greater_equal<> >;
         using exclusive_minimum = compare< std::less<> >;
         using exclusive_maximum = compare< std::greater<> >;

         struct multiple_of : node
         {
            explicit multiple_of( const value& v )
               : node( v )
            {
               assert( m_source.is_number() );
               assert( m_source > 0 );
            }

            json::value validate( const value& v ) const override
            {
               if( auto e = number( m_source ).validate( v ) ) {
                  return e;
               }
               // TODO: enhance for large integer values (uint64_t, ...)
               const auto x = v.as< double >();
               const auto d = m_source.as< double >();
               const auto r = std::fmod( x, d );
               if( std::fabs( r ) < std::numeric_limits< double >::epsilon() ) {
                  return ok();
               }
               if( std::fabs( r - d ) < std::numeric_limits< double >::epsilon() ) {
                  return ok();
               }
               return error( v, "value is not a multiple of given constant", { { "value", x }, { "divident", d } } );
            }
         };

         struct unique_items : node
         {
            using node::node;

            json::value validate( const value& v ) const override
            {
               if( auto e = array( m_source ).validate( v ) ) {
                  return e;
               }
               std::set< value > s;
               for( const auto& e : v.unsafe_get_array() ) {
                  if( !s.emplace( &e ).second ) {
                     return error( v, "duplicate items detected" );
                  }
               }
               return ok();
            }
         };

         struct items : ref
         {
            using ref::ref;

            json::value validate( const value& v ) const override
            {
               if( auto e = array( m_source ).validate( v ) ) {
                  return e;
               }
               for( const auto& e : v.unsafe_get_array() ) {
                  if( ref::validate( e ) ) {
                     return error( v, "invalid item" );
                  }
               }
               return ok();
            }
         };

         struct property_names : ref
         {
            using ref::ref;

            json::value validate( const value& v ) const override
            {
               if( auto e = object( m_source ).validate( v ) ) {
                  return e;
               }
               for( const auto& e : v.unsafe_get_object() ) {
                  if( ref::validate( value( e.first ) ) ) {
                     return error( v.at( e.first ), "invalid key" );
                  }
               }
               return ok();
            }
         };

         struct has_property : node
         {
            std::vector< std::string > m_keys;

            explicit has_property( const value& v )
               : node( v )
            {
               if( v.is_string_type() ) {
                  m_keys.emplace_back( v.as< std::string_view >() );
               }
               else {
                  for( const auto& e : v.get_array() ) {
                     m_keys.emplace_back( e.as< std::string_view >() );
                  }
               }
            }

            json::value validate( const value& v ) const override
            {
               if( auto e = object( m_source ).validate( v ) ) {
                  return e;
               }
               const auto& o = v.unsafe_get_object();
               for( const auto& k : m_keys ) {
                  if( o.find( k ) != o.end() ) {
                     return ok();
                  }
               }
               return error( v, "missing property", { { "keys", m_keys } } );
            }
         };

         struct property : node
         {
            std::string m_key;
            std::unique_ptr< ref > m_schema;

            property( const value& v, node_map& m, const std::string& path )
               : node( v )
            {
               const auto& e = *v.get_object().begin();
               m_key = e.first;
               m_schema = std::make_unique< ref >( e.second, m, path );
            }

            void resolve( const node_map& m ) override
            {
               m_schema->resolve( m );
            }

            json::value validate( const value& v ) const override
            {
               if( auto e = object( m_source ).validate( v ) ) {
                  return e;
               }
               const auto& o = v.unsafe_get_object();
               const auto it = o.find( m_key );
               if( it == o.end() ) {
                  return error( v, "missing property", { { "key", m_key } } );
               }
               return m_schema->validate( it->second );
            }
         };

         struct properties : node
         {
            std::map< std::string, std::unique_ptr< ref > > m_required;
            std::map< std::string, std::unique_ptr< ref > > m_optional;
            std::unique_ptr< ref > m_default;

            using node::node;

            void add_required( const value& v, node_map& m, const std::string& path )
            {
               for( const auto& e : v.get_object() ) {
                  m_required.emplace( e.first, std::make_unique< ref >( e.second, m, path ) );
               }
            }

            void add_optional( const value& v, node_map& m, const std::string& path )
            {
               for( const auto& e : v.get_object() ) {
                  m_optional.emplace( e.first, std::make_unique< ref >( e.second, m, path ) );
               }
            }

            void resolve( const node_map& m ) override
            {
               for( const auto& e : m_required ) {
                  e.second->resolve( m );
               }
               for( const auto& e : m_optional ) {
                  e.second->resolve( m );
               }
               if( m_default ) {
                  m_default->resolve( m );
               }
            }

            json::value validate( const value& v ) const override
            {
               if( auto e = object( m_source ).validate( v ) ) {
                  return e;
               }

               json::value errors = json::empty_array;
               const auto& o = v.unsafe_get_object();

               for( const auto& e : m_required ) {
                  const auto it = o.find( e.first );
                  if( it != o.end() ) {
                     if( auto r = e.second->validate( it->second ) ) {
                        errors.unsafe_emplace_back( std::move( r ) );
                     }
                  }
                  else {
                     errors.unsafe_emplace_back( error( v, "missing property", { { "key", e.first } } ) );
                  }
               }

               for( const auto& e : o ) {
                  const auto it = m_optional.find( e.first );
                  if( it != m_optional.end() ) {
                     if( auto r = it->second->validate( e.second ) ) {
                        errors.unsafe_emplace_back( std::move( r ) );
                     }
                  }
                  else if( m_required.find( e.first ) == m_required.end() ) {
                     if( m_default ) {
                        if( auto r = m_default->validate( e.second ) ) {
                           errors.unsafe_emplace_back( std::move( r ) );
                        }
                     }
                     else {
                        errors.unsafe_emplace_back( error( v, "unexpected property", { { "key", e.first } } ) );
                     }
                  }
               }

               auto& a = errors.unsafe_get_array();
               if( a.empty() ) {
                  return ok();
               }
               if( a.size() == 1 ) {
                  return std::move( a.back() );
               }
               return error( v, "multiple errors", { { "errors", std::move( errors ) } } );
            }
         };

         struct if_then_else : ref
         {
            std::unique_ptr< ref > m_then;
            std::unique_ptr< ref > m_else;

            using ref::ref;

            void resolve( const node_map& m ) override
            {
               ref::resolve( m );
               if( m_then ) {
                  m_then->resolve( m );
               }
               if( m_else ) {
                  m_else->resolve( m );
               }
            }

            json::value validate( const value& v ) const override
            {
               if( !ref::validate( v ) ) {
                  if( m_then ) {
                     return m_then->validate( v );
                  }
               }
               else {
                  if( m_else ) {
                     return m_else->validate( v );
                  }
               }
               return ok();
            }
         };

         template< typename T, typename E >
         struct list : T
         {
            template< typename... Ts >
            explicit list( const value& v, Ts&&... ts )
               : T( v )
            {
               for( const auto& e : v.get_array() ) {
                  this->m_properties.emplace_back( std::make_unique< E >( e, ts... ) );
               }
            }
         };

         struct schema : all_of
         {
            template< typename P, typename T, typename... Ts >
            void add( T&& t, Ts&&... ts )
            {
               if( t ) {
                  m_properties.emplace_back( std::make_unique< P >( std::forward< T >( t ), std::forward< Ts >( ts )... ) );
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
                     auto n = std::make_unique< schema >( e.second, m, p );
                     if( !m.emplace( p, std::move( n ) ).second ) {
                        std::ostringstream os;
                        os << "type '" << p << "' already defined, redefined here:";
                        internal::find( d, e.first ).append_message_extension( os );
                        throw std::runtime_error( os.str() );
                     }
                  }
               }

               // structural
               add< ref >( internal::find( v, "type" ), m, path );
               add< not_ref >( internal::find( v, "not" ), m, path );

               add< list< all_of, ref > >( internal::find( v, "all_of" ), m, path );
               add< list< any_of, ref > >( internal::find( v, "any_of" ), m, path );
               add< list< one_of, ref > >( internal::find( v, "one_of" ), m, path );
               add< list< exclusive, ref > >( internal::find( v, "exclusive" ), m, path );

               if( const auto& n = internal::find( v, "if" ) ) {
                  auto p = std::make_unique< if_then_else >( n, m, path );
                  if( const auto& t = internal::find( v, "then" ) ) {
                     p->m_then = std::make_unique< ref >( t, m, path );
                  }
                  if( const auto& e = internal::find( v, "else" ) ) {
                     p->m_else = std::make_unique< ref >( e, m, path );
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
                     m_properties.emplace_back( std::make_unique< unique_items >( v ) );
                  }
               }

               // object
               add< property_names >( internal::find( v, "property_names" ), m, path );
               add< has_property >( internal::find( v, "has_property" ) );
               add< property >( internal::find( v, "property" ), m, path );
               if( const auto& p = internal::find( v, "properties" ) ) {
                  auto n = std::make_unique< properties >( v );
                  if( const auto& r = internal::find( p, "required" ) ) {
                     n->add_required( r, m, path );
                  }
                  if( const auto& o = internal::find( p, "optional" ) ) {
                     n->add_optional( o, m, path );
                  }
                  if( const auto& a = internal::find( p, "additional" ) ) {
                     n->m_default = std::make_unique< ref >( a, m, path );
                  }
                  m_properties.emplace_back( std::move( n ) );
               }
            }
         };

         ref::ref( const value& v, node_map& m, const std::string& path )
            : node( v )
         {
            if( v.is_boolean() ) {
               if( v.as< bool >() ) {
                  m_node = std::make_unique< trivial< true > >( v );
               }
               else {
                  m_node = std::make_unique< trivial< false > >( v );
               }
            }
            else if( v.is_array() ) {
               m_node = std::make_unique< list< any_of, ref > >( v, m, path );
            }
            else if( v.is_object() ) {
               m_node = std::make_unique< schema >( v, m, path );
            }
         }

      }  // namespace internal

      struct validator
      {
         internal::node_map m_nodes;

         template< typename T >
         void add_builtin( const char* name )
         {
            value source;
            source.key = name;
            source.position = json::position( "<builtin>", 0, 0 );
            m_nodes.emplace( name, std::make_unique< T >( source ) );
         }

         explicit validator( const value& v )
         {
            add_builtin< internal::null >( "null" );
            add_builtin< internal::boolean >( "boolean" );
            add_builtin< internal::number >( "number" );
            add_builtin< internal::string >( "string" );
            add_builtin< internal::binary >( "binary" );
            add_builtin< internal::array >( "array" );
            add_builtin< internal::object >( "object" );

            add_builtin< internal::integer >( "integer" );
            add_builtin< internal::regex >( "regex" );

            m_nodes.emplace( "", std::make_unique< internal::schema >( v, m_nodes ) );

            for( const auto& e : m_nodes ) {
               e.second->resolve( m_nodes );
            }
         }

         json::value validate( const value& v ) const
         {
            return m_nodes.at( "" )->validate( v );
         }
      };

      // clang-format off
      const validator schema_validator( config::parse_input( pegtl::memory_input( R"(

        // the schema for schemas:

        definitions
        {
            unsigned: { type: "integer", minimum: 0 }

            identifier.pattern: "^[a-zA-Z_][a-zA-Z0-9_]*$"
            key.pattern: "^[a-zA-Z_][a-zA-Z0-9_]*(\\.[a-zA-Z_][a-zA-Z0-9_]*)*$"

            ref.any_of: [ "boolean", "key", "ref_list", "schema" ]
            ref_list: { min_size: 1, items: "ref" }

            schema
            {
                properties.optional
                {
                    definitions
                    {
                        property_names: "identifier"
                        properties.additional: "schema"
                    }

                    // structural
                    type: "ref"
                    not: "ref"

                    all_of: "ref_list"  // short-circuit
                    any_of: "ref_list"  // short-circuit
                    one_of: "ref_list"  // exactly one
                    exclusive: "ref_list"  // at most one

                    if: "ref"
                    then: "ref"
                    else: "ref"

                    // any
                    value: true
                    enum.items: true

                    // string/binary/array/object
                    size: "unsigned"
                    min_size: "unsigned"
                    max_size: "unsigned"

                    // string
                    istring: [ "string", { items: "string" } ]
                    pattern: "regex"

                    // number
                    minimum: "number"
                    maximum: "number"
                    exclusive_minimum: "number"
                    exclusive_maximum: "number"
                    multiple_of.exclusive_minimum: 0

                    // array
                    items: "ref"
                    unique_items: "boolean"

                    // object
                    property_names: "ref"
                    has_property: [ "string", { items: "string" } ]
                    property
                    {
                        size: 1
                        properties.additional: "ref"
                    }
                    properties.properties.optional
                    {
                        required.properties.additional: "ref"
                        optional.properties.additional: "ref"
                        additional: "ref"
                    }
                }

                definitions
                {
                    has_size.any_of: [
                        { property.type.enum: [ "string", "binary", "array", "object" ] }
                        { has_property: [ "size", "min_size", "max_size" ] }
                    ]
                    is_string.any_of: [
                        { property.type.value: "string" }
                        { has_property: [ "istring", "pattern" ] }
                    ]
                    is_number.any_of: [
                        { property.type.value: "number" }
                        { has_property: [ "minimum", "maximum", "exclusive_minimum", "exclusive_maximum", "multiple_of" ] }
                    ]
                    is_array.any_of: [
                        { property.type.value: "array" }
                        { has_property: [ "items", "unique_items" ] }
                    ]
                    is_object.any_of: [
                        { property.type.value: "object" }
                        { has_property: [ "property_names", "has_property", "property", "properties" ] }
                    ]
                }

                all_of
                [
                    { if.has_property: [ "then", "else" ] then.has_property: "if" }
                    { exclusive: [ "schema.has_size", "schema.is_number" ] }
                    { exclusive: [ "schema.is_string", "schema.is_number", "schema.is_array", "schema.is_object" ] }
                ]
            }
        }

        type: "schema"

      )", "schema" ) ) );
      // clang-format on

      validator read( const std::string& filename )
      {
         const auto data = config::parse_file( filename );
         if( const auto error = schema_validator.validate( data ) ) {
            std::cerr << std::setw( 2 ) << error << std::endl;
            throw std::runtime_error( "invalid schema file '" + filename + "'" );  // TODO: store error in exception
         }
         return validator( data );
      }

   }  // namespace schema

}  // namespace tao::config

#endif
