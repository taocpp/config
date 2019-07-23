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
   struct phase2_processor
   {
      explicit phase2_processor( entry& e )
      {
         process( e );
      }

      void process( entry& e )
      {
         m_done = 0;
         m_todo = 0;

         (void)process_object_entry( e );

         if( ( m_todo > 0 ) && ( m_done == 0 ) ) {
            throw std::runtime_error( "phase2 processing deadlocked" );  // TODO: Find all not-done sub-values and print their position.
         }
      }

      [[nodiscard]] explicit operator bool() const noexcept
      {
         return m_todo == 0;
      }

   private:
      std::size_t m_done = 0;
      std::size_t m_todo = 0;

      [[nodiscard]] const json_t* process_concat( concat& l )
      {
         assert( !l.entries().empty() );

         bool done = true;

         for( auto& i : l.private_entries() ) {
            done &= process_entry( i );
         }
         if( !done ) {
            ++m_todo;
            return nullptr;
         }
         if( l.entries().size() > 1 ) {
            for( auto i = std::next( l.private_entries().begin() ); i != l.private_entries().end(); ++i ) {
               phase2_add( l.private_entries().front().get_atom(), std::move( i->get_atom() ) );
            }
            l.private_entries().erase( std::next( l.private_entries().begin() ), l.private_entries().end() );
         }
         ++m_done;
         return &l.private_entries().front().get_atom();
      }

      [[nodiscard]] bool process_array_entry( entry& e )
      {
         json_t j( json::empty_array, e.position() );

         bool done = true;

         for( auto& i : e.get_array().private_list() ) {
            if( const json_t* t = process_concat( i ) ) {
               j.unsafe_emplace_back( *t );
               continue;
            }
            done = false;
         }
         if( done ) {
            e.set_atom( std::move( j ) );
            ++m_done;
            return true;
         }
         ++m_todo;
         return false;
      }

      [[nodiscard]] bool process_object_entry( entry& e )
      {
         json_t j( json::empty_object, e.position() );

         bool done = true;

         for( auto& i : e.get_object().private_map() ) {
            if( const json_t* t = process_concat( i.second ) ) {
               j.unsafe_emplace( i.first, *t );
               continue;
            }
            done = false;
         }
         if( done ) {
            e.set_atom( std::move( j ) );
            ++m_done;
            return true;
         }
         ++m_todo;
         return false;
      }

      [[nodiscard]] const json_t* process_reference_impl( entry& e, json_t& r )
      {
         config::key k;

         for( auto& i : r.get_array() ) {
            if( i.is_array() ) {
               if( const json_t* t = process_reference_impl( e, i ) ) {
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
         return phase2_access( r.position, e, k );
      }

      [[nodiscard]] bool process_reference_entry( entry& e )
      {
         if( const json_t* t = process_reference_impl( e.parent()->parent(), e.get_reference() ) ) {
            e.set_atom( *t );
            ++m_done;
            return true;
         }
         ++m_todo;
         return false;
      }

      [[nodiscard]] bool process_entry( entry& e )
      {
         const phase2_guard p2g( e );

         switch( e.type() ) {
            case entry::atom:
               return true;
            case entry::array:
               return process_array_entry( e );
            case entry::object:
               return process_object_entry( e );
            case entry::nothing:
               assert( false );
            case entry::reference:
               return process_reference_entry( e );
         }
         assert( false );
      }
   };

   inline json::basic_value< value_traits > phase2_process( entry& root )
   {
      assert( root.is_object() );

      for( phase2_processor p( root ); !p; p.process( root ) ) {
      }
      assert( root.is_atom() );

      return root.get_atom();
   }

}  // namespace tao::config::internal

#endif
