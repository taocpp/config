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
               ATOM,
               ARRAY,
               OBJECT,
               NOTHING,
               REFERENCE
            };

            entry( entry&& r ) noexcept
               : m_type( r.m_type ),
                 m_position( std::move( r.m_position ) )
            {
               seize( std::move( r ) );
            }

            entry( const entry& r )
               : m_type( NOTHING ),
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
               return m_type == ATOM;
            }

            bool is_array() const noexcept
            {
               return m_type == ARRAY;
            }

            bool is_object() const noexcept
            {
               return m_type == OBJECT;
            }

            bool is_nothing() const noexcept
            {
               return m_type == NOTHING;
            }

            bool is_reference() const noexcept
            {
               return m_type == REFERENCE;
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
               m_type = ATOM;
            }

            void set_array()
            {
               discard();
               new( &m_union.a ) array_t();
               m_type = ARRAY;
            }

            void set_object()
            {
               discard();
               new( &m_union.o ) object_t();
               m_type = OBJECT;
            }

            void set_reference( json::value&& v )
            {
               discard();
               new( &m_union.i ) reference_t( std::move( v ) );
               m_type = REFERENCE;
            }

            template< typename T >
            static entry atom( const position& p, T&& t )
            {
               entry r( p );
               r.set_atom( std::forward< T >( t ) );
               return r;
            }

            static entry array( const position& p )
            {
               entry r( p );
               r.set_array();
               return r;
            }

            static entry object( const position& p )
            {
               entry r( p );
               r.set_object();
               return r;
            }

            static entry reference( const position& p, json::value&& v )
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
               : m_type( NOTHING ),
                 m_position( p )
            {
            }

            void discard() noexcept
            {
               switch( m_type ) {
                  case ATOM:
                     m_union.v.~basic_value();
                     break;
                  case ARRAY:
                     m_union.a.~vector();
                     break;
                  case OBJECT:
                     m_union.o.~map();
                     break;
                  case NOTHING:
                     break;
                  case REFERENCE:
                     m_union.i.~basic_value();
                     break;
               }
               m_type = NOTHING;
            }

            void seize( entry&& r ) noexcept
            {
               switch( r.m_type ) {
                  case ATOM:
                     new( &m_union.v ) atom_t( std::move( r.m_union.v ) );
                     break;
                  case ARRAY:
                     new( &m_union.a ) array_t( std::move( r.m_union.a ) );
                     break;
                  case OBJECT:
                     new( &m_union.o ) object_t( std::move( r.m_union.o ) );
                     break;
                  case NOTHING:
                     break;
                  case REFERENCE:
                     new( &m_union.i ) reference_t( std::move( r.m_union.i ) );
                     break;
               }
               r.discard();
            }

            void embed( const entry& r )
            {
               switch( r.m_type ) {
                  case ATOM:
                     new( &m_union.v ) atom_t( r.m_union.v );
                     break;
                  case ARRAY:
                     new( &m_union.a ) array_t( r.m_union.a );
                     break;
                  case OBJECT:
                     new( &m_union.o ) object_t( r.m_union.o );
                     break;
                  case NOTHING:
                     break;
                  case REFERENCE:
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
         struct traits< entry >
         {
            template< template< typename... > class, typename Consumer >
            static void produce( Consumer& c, const entry& v )
            {
               switch( v.type() ) {
                  case entry::ATOM:
                     json::events::produce< traits >( c, v.get_atom() );
                     return;
                  case entry::ARRAY:
                     json::events::produce< traits >( c, v.get_array() );
                     return;
                  case entry::OBJECT:
                     json::events::produce< traits >( c, v.get_object() );
                     return;
                  case entry::NOTHING:
                     assert( false );
                  case entry::REFERENCE:
                     json::events::produce< traits >( c, v.get_reference() );
                     return;
               }
               assert( false );
            }
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
