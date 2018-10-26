// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_ENTRY_HPP
#define TAO_CONFIG_INTERNAL_ENTRY_HPP

#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "concat.hpp"
#include "format.hpp"
#include "pegtl.hpp"
#include "traits.hpp"
#include "json.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         using atom_t = json::value;
         using array_t = std::vector< concat >;
         using object_t = std::map< std::string, concat >;
         using reference_t = json::value;

         union entry_union
         {
            entry_union() noexcept
            {
            }

            entry_union( entry_union&& ) = delete;
            entry_union( const entry_union& ) = delete;

            void operator=( entry_union&& ) = delete;
            void operator=( const entry_union& ) = delete;

            ~entry_union() noexcept
            {
            }

            atom_t v;
            array_t a;
            object_t o;
            reference_t i;
         };

         class entry
         {
         public:
            enum kind : char
            {
               atom,
               array,
               object,
               nothing,
               reference
            };

            entry( entry&& r ) noexcept
               : m_type( r.m_type ),
                 m_position( std::move( r.m_position ) )
            {
               seize( std::move( r ) );
            }

            entry( const entry& r )
               : m_type( nothing ),
                 m_position( r.m_position )
            {
               embed( r );
               m_type = r.m_type;
            }

            void operator=( entry r ) noexcept
            {
               discard();
               seize( std::move( r ) );
               m_type = r.m_type;
               m_position = std::move( r.m_position );
            }

            ~entry() noexcept
            {
               discard();
            }

            void swap( entry& r ) noexcept
            {
               entry t( std::move( r ) );
               r = std::move( *this );
               ( *this ) = ( std::move( t ) );
            }

            kind type() const noexcept
            {
               return m_type;
            }

            bool is_atom() const noexcept
            {
               return m_type == atom;
            }

            bool is_array() const noexcept
            {
               return m_type == array;
            }

            bool is_object() const noexcept
            {
               return m_type == object;
            }

            bool is_nothing() const noexcept
            {
               return m_type == nothing;
            }

            bool is_reference() const noexcept
            {
               return m_type == reference;
            }

            void reset()
            {
               discard();
            }

            template< typename T >
            void set_atom( T&& t )
            {
               discard();
               new( &m_union.v ) atom_t( std::forward< T >( t ) );
               m_type = atom;
            }

            void set_array()
            {
               discard();
               new( &m_union.a ) array_t();
               m_type = array;
            }

            void set_object()
            {
               discard();
               new( &m_union.o ) object_t();
               m_type = object;
            }

            void set_reference( json::value&& v )
            {
               discard();
               new( &m_union.i ) reference_t( std::move( v ) );
               m_type = reference;
            }

            template< typename T >
            static entry make_atom( const position& p, T&& t )
            {
               entry r( p );
               r.set_atom( std::forward< T >( t ) );
               return r;
            }

            static entry make_array( const position& p )
            {
               entry r( p );
               r.set_array();
               return r;
            }

            static entry make_object( const position& p )
            {
               entry r( p );
               r.set_object();
               return r;
            }

            static entry make_reference( const position& p, json::value&& v )
            {
               entry r( p );
               r.set_reference( std::move( v ) );
               return r;
            }

            atom_t& get_atom() noexcept
            {
               assert( is_atom() );
               return m_union.v;
            }

            array_t& get_array() noexcept
            {
               assert( is_array() );
               return m_union.a;
            }

            object_t& get_object() noexcept
            {
               assert( is_object() );
               return m_union.o;
            }

            reference_t& get_reference() noexcept
            {
               assert( is_reference() );
               return m_union.i;
            }

            const atom_t& get_atom() const noexcept
            {
               assert( is_atom() );
               return m_union.v;
            }

            const array_t& get_array() const noexcept
            {
               assert( is_array() );
               return m_union.a;
            }

            const object_t& get_object() const noexcept
            {
               assert( is_object() );
               return m_union.o;
            }

            const reference_t& get_reference() const noexcept
            {
               assert( is_reference() );
               return m_union.i;
            }

            void set_recursion_marker() const
            {
               if( m_phase2_recursion_marker ) {
                  throw std::runtime_error( format( "reference cycle detected", { &m_position } ) );
               }
               m_phase2_recursion_marker = true;
            }

            void clear_recursion_marker() const noexcept
            {
               m_phase2_recursion_marker = false;
            }

            const pegtl::position& position() const noexcept
            {
               return m_position;
            }

         private:
            explicit
            entry( const pegtl::position& p )
               : m_type( nothing ),
                 m_position( p )
            {
            }

            void discard() noexcept
            {
               switch( m_type ) {
                  case atom:
                     m_union.v.~basic_value();
                     break;
                  case array:
                     m_union.a.~vector();
                     break;
                  case object:
                     m_union.o.~map();
                     break;
                  case nothing:
                     break;
                  case reference:
                     m_union.i.~basic_value();
                     break;
               }
               m_type = nothing;
            }

            void seize( entry&& r ) noexcept
            {
               switch( r.m_type ) {
                  case atom:
                     new( &m_union.v ) atom_t( std::move( r.m_union.v ) );
                     break;
                  case array:
                     new( &m_union.a ) array_t( std::move( r.m_union.a ) );
                     break;
                  case object:
                     new( &m_union.o ) object_t( std::move( r.m_union.o ) );
                     break;
                  case nothing:
                     break;
                  case reference:
                     new( &m_union.i ) reference_t( std::move( r.m_union.i ) );
                     break;
               }
               r.discard();
            }

            void embed( const entry& r )
            {
               switch( r.m_type ) {
                  case atom:
                     new( &m_union.v ) atom_t( r.m_union.v );
                     break;
                  case array:
                     new( &m_union.a ) array_t( r.m_union.a );
                     break;
                  case object:
                     new( &m_union.o ) object_t( r.m_union.o );
                     break;
                  case nothing:
                     break;
                  case reference:
                     new( &m_union.i ) reference_t( r.m_union.i );
                     break;
               }
            }

            mutable bool m_phase2_recursion_marker = false;
            kind m_type;
            entry_union m_union;
            pegtl::position m_position;
         };

         template<>
         struct traits< entry::kind >
         {
            TAO_JSON_DEFAULT_KEY( "type" );

            template< template< typename... > class Traits >
            static void assign( json::basic_value< Traits >& v, const entry::kind k )
            {
               switch( k ) {
                  case entry::atom:
                     v = "atom";
                     return;
                  case entry::array:
                     v = "array";
                     return;
                  case entry::object:
                     v = "object";
                     return;
                  case entry::nothing:
                     v = "nothing";
                     return;
                  case entry::reference:
                     v = "reference";
                     return;
               }
               assert( false );
            }
         };

         template<>
         struct traits< entry >
         {
            template< template< typename... > class Traits, typename Consumer >
            static void produce( Consumer& c, const entry& v )
            {
               switch( v.type() ) {
                  case entry::atom:
                     json::events::produce< Traits >( c, v.get_atom() );
                     return;
                  case entry::array:
                     json::events::produce< Traits >( c, v.get_array() );
                     return;
                  case entry::object:
                     json::events::produce< Traits >( c, v.get_object() );
                     return;
                  case entry::nothing:
                     assert( false );
                  case entry::reference:
                     json::events::produce< Traits >( c, v.get_reference() );
                     return;
               }
               assert( false );
            }
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
