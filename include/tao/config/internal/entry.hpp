// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_ENTRY_HPP
#define TAO_CONFIG_INTERNAL_ENTRY_HPP

#include <stdexcept>
#include <string>

#include "concat.hpp"
#include "entry_array.hpp"
#include "entry_object.hpp"
#include "forward.hpp"
#include "json.hpp"
#include "pegtl.hpp"
#include "value_traits.hpp"

namespace tao::config::internal
{
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

      json_t j;

      entry_array a;
      entry_object o;
   };

   class entry
   {
   public:
      enum kind : std::uint8_t
      {
         atom,
         array,
         object,
         nothing,
         reference
      };

      explicit entry( concat* parent )
         : m_type( nothing ),
           m_parent( parent )
      {
      }

      entry( concat* parent, const entry& r )
         : m_type( nothing ),
           m_parent( parent ),
           m_clear( r.m_clear )
      {
         embed( r );
         m_type = r.m_type;
      }

      entry( entry&& ) = delete;
      entry( const entry& ) = delete;

      entry& operator=( entry&& ) = delete;
      entry& operator=( const entry& ) = delete;

      ~entry() noexcept
      {
         discard();
      }

      kind type() const noexcept
      {
         return m_type;
      }

      concat* parent() const noexcept
      {
         return m_parent;
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

      void set_atom( const json_t& v )
      {
         discard();
         new( &m_union.j ) json_t( v );
         m_type = atom;
         m_union.j.clear = m_clear;  // See comment below.
      }

      template< typename T >
      void set_atom( T&& t, const pegtl::position& pos )
      {
         discard();
         new( &m_union.j ) json_t( json::uninitialized, pos );
         m_union.j.assign( std::forward< T >( t ) );
         m_type = atom;
         m_union.j.clear = m_clear;  // This is at least (?) for phase two reference substitution, when copying the atom from the target of the reference we want to keep our m_clear for the copied atom, because we don't care about whether the reference target was assigned with += or =, we want to use += or = according to how the reference itself was "added".
      }

      void set_array( const pegtl::position& pos )
      {
         discard();
         new( &m_union.a ) entry_array( pos );
         m_type = array;
      }

      void set_object( const pegtl::position& pos )
      {
         discard();
         new( &m_union.o ) entry_object( pos );
         m_type = object;
      }

      void set_reference( json_t&& v )
      {
         discard();
         new( &m_union.j ) json_t( std::move( v ) );
         m_type = reference;
      }

      json_t& get_atom() noexcept
      {
         assert( is_atom() );
         //  assert( m_union.j.clear == m_clear );  // TODO: Where, and why exactly, is this violated?
         return m_union.j;
      }

      entry_array& get_array() noexcept
      {
         assert( is_array() );
         return m_union.a;
      }

      entry_object& get_object() noexcept
      {
         assert( is_object() );
         return m_union.o;
      }

      json_t& get_reference() noexcept
      {
         assert( is_reference() );
         return m_union.j;
      }

      const json_t& get_atom() const noexcept
      {
         assert( is_atom() );
         return m_union.j;
      }

      const entry_array& get_array() const noexcept
      {
         assert( is_array() );
         return m_union.a;
      }

      const entry_object& get_object() const noexcept
      {
         assert( is_object() );
         return m_union.o;
      }

      const json_t& get_reference() const noexcept
      {
         assert( is_reference() );
         return m_union.j;
      }

      concat& emplace_back( const pegtl::position& pos )
      {
         return get_array().emplace_back( this, pos );
      }

      concat& emplace( const std::string& k, const pegtl::position& pos )
      {
         return get_object().try_emplace( k, this, pos ).first->second;
      }

      void set_recursion_marker()
      {
         if( m_phase2_recursion_marker ) {
            throw std::runtime_error( "reference cycle detected -- " + to_string( position() ) );
         }
         m_phase2_recursion_marker = true;
      }

      void clear_recursion_marker() noexcept
      {
         m_phase2_recursion_marker = false;
      }

      bool clear() const noexcept
      {
         return m_clear;
      }

      void set_clear( const bool c = true ) noexcept
      {
         switch( m_type ) {
            case atom:
            case reference:
               m_union.j.clear = c;
               break;
            case array:
            case object:
            case nothing:
               break;
         }
         m_clear = c;
      }

      void copy_atom_from( const entry& e )
      {
         assert( e.clear() );
         assert( is_atom() );
         assert( e.is_atom() );
         assert( m_union.j.clear == m_clear );
         assert( e.m_union.j.clear == e.m_clear );
         assert( m_parent == e.m_parent );

         m_clear = e.m_clear;

         embed( e );
      }

      const pegtl::position& position() const noexcept
      {
         // TODO: Check whether this function should be eliminated in refactoring.

         switch( m_type ) {
            case atom:
            case reference:
               assert( m_union.j.position );
               return *m_union.j.position;
            case array:
               return m_union.a.position();
            case object:
               return m_union.o.position();
            case nothing:
               break;
         }
         assert( false );
      }

   private:
      void discard() noexcept
      {
         switch( m_type ) {
            case atom:
            case reference:
               m_union.j.~basic_value();
               break;
            case array:
               m_union.a.~entry_array();
               break;
            case object:
               m_union.o.~entry_object();
               break;
            case nothing:
               break;
         }
         m_type = nothing;
      }

      void embed( const entry& r )
      {
         switch( r.m_type ) {
            case atom:
            case reference:
               new( &m_union.j ) json_t( r.m_union.j );
               break;
            case array:
               new( &m_union.a ) entry_array( r.m_union.a.position() );
               for( const auto& i : r.m_union.a.list() ) {
                  m_union.a.emplace_back( this, i );
               }
               break;
            case object:
               new( &m_union.o ) entry_object( r.m_union.o.position() );
               for( const auto& i : r.m_union.o.map() ) {
                  m_union.o.try_emplace( i.first, this, i.second );
               }
               break;
            case nothing:
               break;
         }
      }

      kind m_type;
      concat* m_parent;  // TODO?
      entry_union m_union;

      bool m_clear = false;
      bool m_phase2_recursion_marker = false;
   };

}  // namespace tao::config::internal

#endif
