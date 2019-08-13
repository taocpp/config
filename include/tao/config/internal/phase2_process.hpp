// Copyright (c) 2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE2_PROCESS_HPP
#define TAO_CONFIG_INTERNAL_PHASE2_PROCESS_HPP

#include <cassert>
#include <vector>
#include <numeric>

#include "concat.hpp"
#include "entry.hpp"
#include "format.hpp"
#include "json.hpp"
#include "key_utility.hpp"
#include "phase2_access.hpp"
#include "phase2_add.hpp"
#include "phase2_guard.hpp"
#include "to_stream.hpp"
#include "value_traits.hpp"

namespace tao::config::internal
{
   struct phase2_processor
   {
      explicit phase2_processor( entry& e )
         : m_root( e )
      {
      }

      void process()
      {
         m_done = 0;
         m_todo.clear();

         (void)process_object_entry( m_root );

         if( ( m_done == 0 ) && ( !finished() ) ) {
            throw std::runtime_error( format( __FILE__, __LINE__, "phase2 processing deadlocked", { { "where", m_todo } } ) );
         }
      }

      [[nodiscard]] bool finished() const noexcept
      {
         return m_root.is_atom();
      }

   private:
      entry& m_root;
      std::size_t m_done = 0;
      std::vector< std::pair< const json_t*, const pegtl::position* > > m_todo;

      [[nodiscard]] const json_t* process_concat( concat& l )
      {
         assert( !l.entries().empty() );

         bool done = true;

         for( auto& i : l.private_entries() ) {
            done &= process_entry( i );
         }
         if( !done ) {
            return nullptr;
         }
         if( l.entries().size() > 1 ) {
            for( auto i = std::next( l.private_entries().begin() ); i != l.private_entries().end(); ++i ) {
               phase2_add( l.private_entries().front().get_atom(), std::move( i->get_atom() ) );
            }
            l.private_entries().erase( std::next( l.private_entries().begin() ), l.private_entries().end() );
            ++m_done;
         }
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
         return false;
      }

      [[nodiscard]] bool process_object_entry( entry& e )
      {
         json_t j( json::empty_object, e.position() );

         bool done = true;

         for( auto& i : e.get_object().private_map() ) {
            if( const json_t* t = process_concat( i.second ) ) {
               j.unsafe_try_emplace( i.first, *t );
               continue;
            }
            done = false;
         }
         if( done ) {
            e.set_atom( std::move( j ) );
            ++m_done;
            return true;
         }
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
         assert( r.position );
         return phase2_access( *r.position, e, k );
      }

      [[nodiscard]] bool process_reference_entry( entry& e )
      {
         if( const json_t* t = process_reference_impl( e.parent()->parent(), e.get_reference() ) ) {
            e.set_atom( *t );
            ++m_done;
            return true;
         }
         m_todo.emplace_back( &e.get_reference(), &e.position() );
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


   template< typename T >
   void to_stream( std::ostream* os, const char* tag, const T& root, const std::size_t indent = 3 )
   {
      if( os ) {
         ( *os ) << tag << std::endl;
         to_stream( *os, root, indent );
         ( *os ) << std::endl;
      }
   }

   inline json::basic_value< value_traits > phase2_process( entry& root, std::ostream* os = nullptr )
   {
      to_stream( os, "INITIAL", root );

      for( phase2_processor p( root ); !p.finished(); p.process() ) {
         to_stream( os, "ITERATION", root );
      }
      to_stream( os, "FINAL", root );
      return root.get_atom();
   }

}  // namespace tao::config::internal

#endif
