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

         struct node_base;
         using node_map = std::map< std::string, std::unique_ptr< node_base > >;

         struct node_base
         {
            virtual ~node_base() = default;

            virtual void resolve( const node_map& /*unused*/ ) {}
            virtual bool validate( const value& v ) const = 0;
         };

         struct failure_node : node_base
         {
            bool validate( const value& /*unused*/ ) const override
            {
               return false;
            }
         };

         struct success_node : node_base
         {
            bool validate( const value& /*unused*/ ) const override
            {
               return true;
            }
         };

         struct is_null_node : node_base
         {
            bool validate( const value& v ) const override
            {
               return v.is_null();
            }
         };

         struct is_boolean_node : node_base
         {
            bool validate( const value& v ) const override
            {
               return v.is_boolean();
            }
         };

         struct is_number_node : node_base
         {
            bool validate( const value& v ) const override
            {
               return v.is_number();
            }
         };

         struct is_string_node : node_base
         {
            bool validate( const value& v ) const override
            {
               return v.is_string_type();
            }
         };

         struct is_binary_node : node_base
         {
            bool validate( const value& v ) const override
            {
               return v.is_binary_type();
            }
         };

         struct is_array_node : node_base
         {
            bool validate( const value& v ) const override
            {
               return v.is_array();
            }
         };

         struct is_object_node : node_base
         {
            bool validate( const value& v ) const override
            {
               return v.is_object();
            }
         };

         struct is_integer_node : node_base
         {
            bool validate( const value& v ) const override
            {
               if( !v.is_number() ) {
                  return false;
               }
               if( v.is_double() ) {
                  double dummy;
                  if( std::modf( v.as< double >(), &dummy ) != 0 ) {
                     return false;
                  }
               }
               return true;
            }
         };

         template< bool B >
         struct combine : node_base
         {
            std::vector< std::unique_ptr< node_base > > m_properties;

            void resolve( const node_map& m ) override
            {
               for( const auto& p : m_properties ) {
                  p->resolve( m );
               }
            }

            bool validate( const value& v ) const override
            {
               const auto& vs = v.skip_value_ptr();
               for( const auto& p : m_properties ) {
                  if( p->validate( vs ) == B ) {
                     return B;
                  }
               }
               return !B;
            }
         };

         using any_of = combine< true >;
         using all_of = combine< false >;

         struct one_of : node_base
         {
            std::vector< std::unique_ptr< node_base > > m_properties;

            void resolve( const node_map& m ) override
            {
               for( const auto& p : m_properties ) {
                  p->resolve( m );
               }
            }

            bool validate( const value& v ) const override
            {
               const auto& vs = v.skip_value_ptr();
               bool found = false;
               for( const auto& p : m_properties ) {
                  if( p->validate( vs ) ) {
                     if( found ) {
                        return false;
                     }
                     found = true;
                  }
               }
               return found;
            }
         };

         struct ref : node_base
         {
            const value m_value;
            std::unique_ptr< node_base > m_node;
            const node_base* m_ptr = nullptr;

            ref( const value& v, node_map& m, const std::string& path );

            void resolve( const node_map& m ) override
            {
               if( m_node ) {
                  m_node->resolve( m );
               }
               else {
                  const auto k = m_value.as< std::string >();
                  const auto it = m.find( k );
                  if( it == m.end() ) {
                     std::ostringstream os;
                     os << "can not resolve '" << k << "' defined here:";
                     m_value.append_message_extension( os );
                     throw std::runtime_error( os.str() );
                  }
                  m_ptr = it->second.get();
               }
            }

            bool validate( const value& v ) const override
            {
               return m_ptr->validate( v );
            }
         };

         struct not_ref : ref
         {
            using ref::ref;

            bool validate( const value& v ) const override
            {
               return !ref::validate( v );
            }
         };

         struct constant : node_base
         {
            const value m_value;

            explicit constant( const value& v )
               : m_value( v )
            {}

            bool validate( const value& v ) const override
            {
               return v == m_value;
            }
         };

         template< typename T >
         struct value_length : node_base
         {
            const std::size_t m_length;

            explicit value_length( const value& v )
               : m_length( v.as< std::size_t >() )
            {}

            bool validate( const value& v ) const override
            {
               if( v.is_string_type() ) {
                  return T()( m_length, v.as< std::string_view >().size() );
               }
               if( v.is_binary_type() ) {
                  return T()( m_length, v.as< tao::binary_view >().size() );
               }
               return false;
            }
         };

         using length = value_length< std::equal_to<> >;
         using min_length = value_length< std::less_equal<> >;
         using max_length = value_length< std::greater_equal<> >;

         struct pattern : node_base
         {
            const std::regex m_regex;

            explicit pattern( const std::string_view sv )
               : m_regex( sv.begin(), sv.end() )
            {}

            explicit pattern( const value& v )
               : pattern( v.as< std::string_view >() )
            {}

            bool validate( const value& v ) const override
            {
               if( !v.is_string_type() ) {
                  return false;
               }
               const std::string_view sv = v.as< std::string_view >();
               return std::regex_search( sv.begin(), sv.end(), m_regex );
            }
         };

         struct is_regex : node_base
         {
            bool validate( const value& v ) const override
            {
               if( !v.is_string_type() ) {
                  return false;
               }
               const std::string_view sv = v.as< std::string_view >();
               try {
                  std::regex( sv.begin(), sv.end() );
               }
               catch( const std::regex_error& ) {
                  return false;
               }
               return true;
            }
         };

         template< typename T >
         struct compare : node_base
         {
            const value m_value;

            explicit compare( const value& v )
               : m_value( v )
            {
               assert( m_value.is_number() );
            }

            bool validate( const value& v ) const override
            {
               return v.is_number() && T()( m_value, v );
            }
         };

         using minimum = compare< std::less_equal<> >;
         using maximum = compare< std::greater_equal<> >;
         using exclusive_minimum = compare< std::less<> >;
         using exclusive_maximum = compare< std::greater<> >;

         struct multiple_of : node_base
         {
            const value m_value;

            explicit multiple_of( const value& v )
               : m_value( v )
            {
               assert( m_value.is_number() );
               assert( m_value > 0 );
            }

            bool validate( const value& v ) const override
            {
               if( !v.is_number() ) {
                  return false;
               }
               // TODO: enhance for for large integer values (uint64_t, ...)
               const auto d = m_value.as< double >();
               const auto r = std::fmod( v.as< double >(), d );
               if( std::fabs( r ) < std::numeric_limits< double >::epsilon() ) {
                  return true;
               }
               if( std::fabs( r - d ) < std::numeric_limits< double >::epsilon() ) {
                  return true;
               }
               return false;
            }
         };

         template< typename T >
         struct array_length : node_base
         {
            const std::size_t m_length;

            explicit array_length( const value& v )
               : m_length( v.as< std::size_t >() )
            {}

            bool validate( const value& v ) const override
            {
               if( !v.is_array() ) {
                  return false;
               }
               return T()( m_length, v.unsafe_get_array().size() );
            }
         };

         using min_items = array_length< std::less_equal<> >;
         using max_items = array_length< std::greater_equal<> >;

         struct unique_items : node_base
         {
            bool validate( const value& v ) const override
            {
               if( !v.is_array() ) {
                  return false;
               }
               std::set< value > s;
               for( const auto& e : v.unsafe_get_array() ) {
                  if( !s.emplace( &e ).second ) {
                     return false;
                  }
               }
               return true;
            }
         };

         struct array_ref : ref
         {
            using ref::ref;

            bool validate( const value& v ) const override
            {
               if( !v.is_array() ) {
                  return false;
               }
               for( const auto& e : v.unsafe_get_array() ) {
                  if( !ref::validate( e ) ) {
                     return false;
                  }
               }
               return true;
            }
         };

         template< typename T >
         struct object_length : node_base
         {
            const std::size_t m_length;

            explicit object_length( const value& v )
               : m_length( v.as< std::size_t >() )
            {}

            bool validate( const value& v ) const override
            {
               if( !v.is_object() ) {
                  return false;
               }
               return T()( m_length, v.unsafe_get_object().size() );
            }
         };

         using min_properties = object_length< std::less_equal<> >;
         using max_properties = object_length< std::greater_equal<> >;

         struct keys_ref : ref
         {
            using ref::ref;

            bool validate( const value& v ) const override
            {
               if( !v.is_object() ) {
                  return false;
               }
               for( const auto& e : v.unsafe_get_object() ) {
                  if( !ref::validate( value( e.first ) ) ) {
                     return false;
                  }
               }
               return true;
            }
         };

         struct object_ref : ref
         {
            using ref::ref;

            bool validate( const value& v ) const override
            {
               if( !v.is_object() ) {
                  return false;
               }
               for( const auto& e : v.unsafe_get_object() ) {
                  if( !ref::validate( e.second ) ) {
                     return false;
                  }
               }
               return true;
            }
         };

         struct has_property : node_base
         {
            const std::string m_key;

            explicit has_property( const value& v )
               : m_key( v.as< std::string >() )
            {}

            bool validate( const value& v ) const override
            {
               if( !v.is_object() ) {
                  return false;
               }
               const auto& o = v.unsafe_get_object();
               return o.find( m_key ) != o.end();
            }
         };

         struct properties : node_base
         {
            std::map< std::string, std::unique_ptr< ref > > m_map;
            std::unique_ptr< ref > m_default;

            properties( const value& v, node_map& m, const std::string& path )
            {
               for( const auto& e : v.get_object() ) {
                  m_map.emplace( e.first, std::make_unique< ref >( e.second, m, path ) );
               }
            }

            void resolve( const node_map& m ) override
            {
               for( const auto& e : m_map ) {
                  e.second->resolve( m );
               }
               if( m_default ) {
                  m_default->resolve( m );
               }
            }

            bool validate( const value& v ) const override
            {
               if( !v.is_object() ) {
                  return false;
               }
               for( const auto& e : v.unsafe_get_object() ) {
                  const auto it = m_map.find( e.first );
                  if( it != m_map.end() ) {
                     if( !it->second->validate( e.second ) ) {
                        return false;
                     }
                  }
                  else if( m_default ) {
                     if( !m_default->validate( e.second ) ) {
                        return false;
                     }
                  }
               }
               return true;
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

            bool validate( const value& v ) const override
            {
               if( ref::validate( v ) ) {
                  if( m_then ) {
                     return m_then->validate( v );
                  }
               }
               else {
                  if( m_else ) {
                     return m_else->validate( v );
                  }
               }
               return true;
            }
         };

         template< typename T, typename E >
         struct list : T
         {
            template< typename... Ts >
            explicit list( const value& v, Ts&&... ts )
            {
               for( const auto& e : v.get_array() ) {
                  this->m_properties.emplace_back( std::make_unique< E >( e, ts... ) );
               }
            }
         };

         struct node : all_of
         {
            std::string m_description;

            template< typename P, typename T, typename... Ts >
            void add( T&& t, Ts&&... ts )
            {
               if( t ) {
                  m_properties.emplace_back( std::make_unique< P >( std::forward< T >( t ), std::forward< Ts >( ts )... ) );
               }
            }

            node( const value& v, node_map& m, const std::string& path = "" )
            {
               // description
               if( const auto& n = internal::find( v, "description" ) ) {
                  m_description = n.as< std::string >();
               }

               // sub/ref
               add< ref >( internal::find( v, "type" ), m, path );

               // structural
               add< not_ref >( internal::find( v, "not" ), m, path );

               add< list< all_of, ref > >( internal::find( v, "all_of" ), m, path );
               add< list< any_of, ref > >( internal::find( v, "any_of" ), m, path );
               add< list< one_of, ref > >( internal::find( v, "one_of" ), m, path );

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

               // value (generic)
               add< constant >( internal::find( v, "const" ) );
               add< list< any_of, constant > >( internal::find( v, "enum" ) );

               // string/binary
               add< length >( internal::find( v, "length" ) );
               add< min_length >( internal::find( v, "min_length" ) );
               add< max_length >( internal::find( v, "max_length" ) );

               // string
               add< pattern >( internal::find( v, "pattern" ) );

               // binary
               // TODO

               // number
               add< minimum >( internal::find( v, "minimum" ) );
               add< maximum >( internal::find( v, "maximum" ) );
               add< exclusive_minimum >( internal::find( v, "exclusive_minimum" ) );
               add< exclusive_maximum >( internal::find( v, "exclusive_maximum" ) );
               add< multiple_of >( internal::find( v, "multiple_of" ) );

               // array
               add< min_items >( internal::find( v, "min_items" ) );
               add< max_items >( internal::find( v, "max_items" ) );
               add< array_ref >( internal::find( v, "items" ), m, path );
               if( const auto& n = internal::find( v, "unique_items" ) ) {
                  if( n.as< bool >() == true ) {
                     m_properties.emplace_back( std::make_unique< unique_items >() );
                  }
               }

               // object
               add< min_properties >( internal::find( v, "min_properties" ) );
               add< max_properties >( internal::find( v, "max_properties" ) );
               add< keys_ref >( internal::find( v, "property_names" ), m, path );
               if( const auto& n = internal::find( v, "properties" ) ) {
                  auto p = std::make_unique< properties >( n, m, path );
                  if( const auto& n = internal::find( v, "additional_properties" ) ) {
                     p->m_default = std::make_unique< ref >( n, m, path );
                  }
                  m_properties.emplace_back( std::move( p ) );
               }
               else {
                  add< object_ref >( internal::find( v, "additional_properties" ), m, path );
               }
               add< list< all_of, has_property > >( internal::find( v, "required" ) );

               // definitions
               if( const auto& d = internal::find( v, "definitions" ) ) {
                  for( const auto& e : d.get_object() ) {
                     assert( is_identifier( e.first ) );
                     auto p = path.empty() ? e.first : ( path + '.' + e.first );
                     auto n = std::make_unique< node >( e.second, m, p );
                     if( !m.emplace( p, std::move( n ) ).second ) {
                        std::ostringstream os;
                        os << "type '" << p << "' already defined, redefined here:";
                        internal::find( d, e.first ).append_message_extension( os );
                        throw std::runtime_error( os.str() );
                     }
                  }
               }
            }
         };

         ref::ref( const value& v, node_map& m, const std::string& path )
            : m_value( v )
         {
            if( v.is_boolean() ) {
               if( v.as< bool >() ) {
                  m_node = std::make_unique< success_node >();
               }
               else {
                  m_node = std::make_unique< failure_node >();
               }
               m_ptr = m_node.get();
            }
            else if( v.is_array() ) {
               m_node = std::make_unique< list< any_of, ref > >( v, m, path );
               m_ptr = m_node.get();
            }
            else if( v.is_object() ) {
               m_node = std::make_unique< node >( v, m, path );
               m_ptr = m_node.get();
            }
         }

      }  // namespace internal

      struct validator
      {
         internal::node_map m_nodes;

         explicit validator( const value& v )
         {
            m_nodes.emplace( "null", std::make_unique< internal::is_null_node >() );
            m_nodes.emplace( "boolean", std::make_unique< internal::is_boolean_node >() );
            m_nodes.emplace( "number", std::make_unique< internal::is_number_node >() );
            m_nodes.emplace( "string", std::make_unique< internal::is_string_node >() );
            m_nodes.emplace( "binary", std::make_unique< internal::is_binary_node >() );
            m_nodes.emplace( "array", std::make_unique< internal::is_array_node >() );
            m_nodes.emplace( "object", std::make_unique< internal::is_object_node >() );

            m_nodes.emplace( "integer", std::make_unique< internal::is_integer_node >() );
            m_nodes.emplace( "regex", std::make_unique< internal::is_regex >() );

            m_nodes.emplace( "", std::make_unique< internal::node >( v, m_nodes ) );

            for( const auto& e : m_nodes ) {
               e.second->resolve( m_nodes );
            }
         }

         bool validate( const value& v ) const
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
            ref_list.items: "ref"

            schema
            {
                properties
                {
                    description: "string"

                    type: "ref"
                    not: "ref"

                    all_of: "ref_list"
                    any_of: "ref_list"
                    one_of: "ref_list"

                    if: "ref"
                    then: "ref"
                    else: "ref"

                    const: true
                    enum.unique_items: true

                    length: "unsigned"
                    min_length: "unsigned"
                    max_length: "unsigned"
                    pattern: "regex"

                    minimum: "number"
                    maximum: "number"
                    exclusive_minimum: "number"
                    exclusive_maximum: "number"
                    multiple_of.exclusive_minimum: 0

                    min_items: "unsigned"
                    max_items: "unsigned"
                    items: "ref"
                    unique_items: "boolean"

                    min_properties: "unsigned"
                    max_properties: "unsigned"
                    property_names: "ref"
                    properties.additional_properties: "ref"
                    additional_properties: "ref"
                    required.items: "string"

                    definitions
                    {
                        property_names: "identifier"
                        additional_properties: "schema"
                    }
                }
                additional_properties: false
            }
        }

        type: "schema"

      )", "schema" ) ) );
      // clang-format on

      validator read( const std::string& filename )
      {
         const auto data = config::parse_file( filename );
         assert( schema_validator.validate( data ) );
         return validator( data );
      }

   }  // namespace schema

}  // namespace tao::config

#endif
