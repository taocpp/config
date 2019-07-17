// Copyright (c) 2018-2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_ENTRY_HPP
#define TAO_CONFIG_INTERNAL_ENTRY_HPP

#include <list>
#include <map>
#include <stdexcept>
#include <string>

#include "concat.hpp"
#include "forward.hpp"
#include "json.hpp"
#include "pegtl.hpp"
#include "value_traits.hpp"

namespace tao::config::internal
{
   // NOTE: json_t now keeps it's own position(s), so:
   using array_t = std::list< concat >;  // TODO: Move position from entry to here?
   using object_t = std::map< std::string, concat >;  // TODO: Move position from entry to here?

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
      array_t a;
      object_t o;
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

      entry( concat* parent, const pegtl::position& pos )
         : m_type( nothing ),
           m_parent( parent ),
           m_position( pos )
      {
      }

      entry( concat* parent, const entry& r )
         : m_type( nothing ),
           m_parent( parent ),
           m_position( r.m_position ),
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
      }

      template< typename T >
      void set_atom( T&& t, const pegtl::position& pos )
      {
         discard();
         new( &m_union.j ) json_t( json::uninitialized, pos );
         m_union.j.assign( std::forward< T >( t ) );
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

      void set_reference( json_t&& v )
      {
         discard();
         new( &m_union.j ) json_t( std::move( v ) );
         m_type = reference;
      }

      json_t& get_atom() noexcept
      {
         assert( is_atom() );
         return m_union.j;
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
            throw std::runtime_error( "reference cycle detected -- " + to_string( m_position ) );
         }
         m_phase2_recursion_marker = true;
      }

      void clear_recursion_marker() noexcept
      {
         m_phase2_recursion_marker = false;
      }

      const pegtl::position& position() const noexcept
      {
         return m_position;
      }

      bool clear() const noexcept
      {
         return m_clear;
      }

      void set_clear( const bool c = true ) noexcept
      {
         m_clear = c;
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
               m_union.a.~list();
               break;
            case object:
               m_union.o.~map();
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
               new( &m_union.a ) array_t();
               for( const auto& i : r.m_union.a ) {
                  m_union.a.emplace_back( this, i );
               }
               break;
            case object:
               new( &m_union.o ) object_t();
               for( const auto& i : r.m_union.o ) {
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
      pegtl::position m_position;

      bool m_clear = false;
      bool m_phase2_recursion_marker = false;
   };

}  // namespace tao::config::internal

#endif
