// Copyright (c) 2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE2_PROCESS_HPP
#define TAO_CONFIG_INTERNAL_PHASE2_PROCESS_HPP

#include "access.hpp"
#include "concat.hpp"
#include "entry.hpp"
#include "json.hpp"
#include "phase2_add.hpp"
#include "phase2_guard.hpp"
#include "phase2_traits.hpp"
#include "utility.hpp"

namespace tao::config::internal
{
   inline json::basic_value< phase2_traits > phase2_process_entry( const entry& e );

   inline json::basic_value< phase2_traits > phase2_process_atom( const atom_t& v )
   {
      json::events::to_basic_value< phase2_traits > consumer;
      json::events::from_value( consumer, v );
      return std::move( consumer.value );
   }

   inline json::basic_value< phase2_traits > phase2_process_list( const concat& l )
   {
      assert( !l.entries().empty() );

      json::basic_value< phase2_traits > r = phase2_process_entry( l.entries().front() );

      for( auto i = std::next( l.entries().begin() ); i != l.entries().end(); ++i ) {
         phase2_add( r, phase2_process_entry( *i ), i->position() );
      }
      r.set_position( l.p );
      return r;
   }

   inline json::basic_value< phase2_traits > phase2_process_array( const array_t& a )
   {
      json::basic_value< phase2_traits > t( json::empty_array );

      for( const auto& i : a ) {
         if( !i.is_temporary() ) {
            t.emplace_back( phase2_process_list( i ) );
         }
      }
      return t;
   }

   inline json::basic_value< phase2_traits > phase2_process_object( const object_t& o )
   {
      json::basic_value< phase2_traits > t( json::empty_object );

      for( const auto& i : o ) {
         if( !i.second.is_temporary() ) {
            t.emplace( i.first, phase2_process_list( i.second ) );
         }
      }
      return t;
   }

   inline const concat& phase2_process_reference_impl( const position& pos, const entry& e, const reference_t& r )
   {
      config::key k;

      for( auto& i : r.get_array() ) {
         if( i.is_array() ) {
            k.emplace_back( part_from_value( pos, phase2_process_list( phase2_process_reference_impl( pos, e, i ) ) ) );
         }
         else {
            k.emplace_back( part_from_value( pos, i ) );
         }
      }
      return access( pos, e, k );
   }

   inline json::basic_value< phase2_traits > phase2_process_reference( const position& pos, const entry& e, const reference_t& r )
   {
      return phase2_process_list( phase2_process_reference_impl( pos, e, r ) );
   }

   inline json::basic_value< phase2_traits > phase2_process_entry( const entry& e )
   {
      json::basic_value< phase2_traits > r;

      const phase2_guard p2g( e );

      switch( e.type() ) {
         case entry::atom:
            r = phase2_process_atom( e.get_atom() );
            break;
         case entry::array:
            r = phase2_process_array( e.get_array() );
            break;
         case entry::object:
            r = phase2_process_object( e.get_object() );
            break;
         case entry::nothing:
            assert( false );
         case entry::reference:
            r = phase2_process_reference( e.position(), e.parent()->parent(), e.get_reference() );
            break;
         default:
            assert( false );
      }
      r.clear = e.clear();
      r.set_position( e.position() );
      return r;
   }

   inline json::basic_value< phase2_traits > phase2_process( const entry& root )
   {
      assert( root.is_object() );

      return phase2_process_object( root.get_object() );
   }

}  // namespace tao::config::internal

#endif
