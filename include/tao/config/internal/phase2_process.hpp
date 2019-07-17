// Copyright (c) 2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE2_PROCESS_HPP
#define TAO_CONFIG_INTERNAL_PHASE2_PROCESS_HPP

#include <numeric>

#include "concat.hpp"
#include "entry.hpp"
#include "json.hpp"
#include "phase2_access.hpp"
#include "phase2_add.hpp"
#include "phase2_guard.hpp"
#include "to_stream.hpp"
#include "value_traits.hpp"
#include "utility.hpp"

namespace tao::config::internal
{
   inline bool phase2_process_entry( entry& e );

   inline const json_t* phase2_process_concat( concat& l )
   {
      assert( !l.entries().empty() );

      bool done = true;

      for( auto& i : l.private_entries() ) {
         done &= phase2_process_entry( i );
      }
      if( !done ) {
         return nullptr;
      }
      if( l.entries().size() > 1 ) {
         for( auto i = std::next( l.private_entries().begin() ); i != l.private_entries().end(); ++i ) {
            phase2_add( l.private_entries().front().get_atom(), std::move( i->get_atom() ) );
         }
         l.private_entries().erase( std::next( l.private_entries().begin() ), l.private_entries().end() );
      }
      return &l.private_entries().front().get_atom();
   }

   inline bool phase2_process_array_entry( entry& e )
   {
      json_t j( json::empty_array, e.position() );

      bool done = true;

      for( auto& i : e.get_array() ) {
         if( const json_t* t = phase2_process_concat( i ) ) {
            j.unsafe_emplace_back( *t );
            continue;
         }
         done = false;
      }
      if( done ) {
         e.set_atom( std::move( j ) );
         return true;
      }
      return false;
   }

   inline bool phase2_process_object_entry( entry& e )
   {
      json_t j( json::empty_object, e.position() );

      bool done = true;

      for( auto& i : e.get_object() ) {
         if( const json_t* t = phase2_process_concat( i.second ) ) {
            j.unsafe_emplace( i.first, *t );
            continue;
         }
         done = false;
      }
      if( done ) {
         e.set_atom( std::move( j ) );
         return true;
      }
      return false;
   }

   inline const json_t* phase2_process_reference_impl( entry& e, json_t& r )
   {
      config::key k;

      for( auto& i : r.get_array() ) {
         if( i.is_array() ) {
            if( const json_t* t = phase2_process_reference_impl( e, i ) ) {
               k.emplace_back( value_to_part( *t ) );
            }
            else {
               return nullptr;
            }
         }
         else {
            k.emplace_back( value_to_part( i ) );
         }
      }
      return phase2_access( e, k );
   }

   inline bool phase2_process_reference_entry( entry& e )
   {
      if( const json_t* t = phase2_process_reference_impl( e.parent()->parent(), e.get_reference() ) ) {
         e.set_atom( *t );
         return true;
      }
      return false;
   }

   inline bool phase2_process_entry( entry& e )
   {
      const phase2_guard p2g( e );

      switch( e.type() ) {
         case entry::atom:
            return true;
         case entry::array:
            return phase2_process_array_entry( e );
         case entry::object:
            return phase2_process_object_entry( e );
         case entry::nothing:
            assert( false );
         case entry::reference:
            return phase2_process_reference_entry( e );
      }
      assert( false );
   }

   inline json::basic_value< value_traits > phase2_process( entry& root )
   {
      assert( root.is_object() );

      // TODO: Remove counter, iterate until we reach a fixed-point.

      for( unsigned i = 0; i < 42; ++i ) {
         // std::cout << "ITERATION " << i << std::endl;
         // tao::config::internal::to_stream( std::cout, root, 3 );
         // std::cout << std::endl;
         if( phase2_process_object_entry( root ) ) {
            // std::cout << "SUCCESS " << i << std::endl;
            // tao::config::internal::to_stream( std::cout, root, 3 );
            // std::cout << std::endl;
            return root.get_atom();
         }
      }
      // std::cout << "FAILURE" << std::endl;
      // tao::config::internal::to_stream( std::cout, root, 3 );
      // std::cout << std::endl;
      throw std::runtime_error( "config error" );  // TODO: Better error message (find unresolved references in root).
   }

}  // namespace tao::config::internal

#endif
