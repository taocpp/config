// Copyright (c) 2019-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE2_PROCESS_HPP
#define TAO_CONFIG_INTERNAL_PHASE2_PROCESS_HPP

#include <cassert>
#include <numeric>
#include <vector>

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
   template< typename T >
   void to_stream( std::ostream* os, const char* tag, const T& root, const std::size_t indent = 3 )
   {
      if( os ) {
         ( *os ) << tag << std::endl;
         to_stream( *os, root, indent );
         ( *os ) << std::endl;
      }
   }

   struct phase2_processor
   {
      phase2_processor( entry& root, std::ostream* debug )
         : m_root( root ),
           m_debug( debug )
      {
         to_stream( m_debug, "BEGIN", m_root );
      }

      void process()
      {
         m_done = 0;
         m_todo.clear();

         (void)process_object_entry( m_root );

         if( ( m_done == 0 ) && ( !finished() ) ) {
            to_stream( m_debug, "ERROR", m_root );
            throw std::runtime_error( format( __FILE__, __LINE__, "phase2 processing deadlocked", { { "where", m_todo } } ) );
         }
         to_stream( m_debug, "UPDATE", m_root );
      }

      [[nodiscard]] bool finished() const noexcept
      {
         return m_root.is_atom();
      }

   private:
      entry& m_root;
      std::ostream* m_debug;
      std::size_t m_done = 0;
      std::vector< std::pair< const json_t*, const pegtl::position* > > m_todo;

      [[nodiscard]] const json_t* process_concat( concat& l )
      {
         assert( !l.entries().empty() );

         for( auto& i : l.private_entries() ) {
            process_entry( i );
         }
         auto i = l.private_entries().begin();
         auto j = l.private_entries().end();

         while( ( i != l.private_entries().end() ) && ( ( j = std::next( i ) ) != l.private_entries().end() ) ) {
            if( j->clear() ) {
               i = l.private_entries().erase( i );
               assert( i == j );
               ++m_done;
            }
            else if( i->is_atom() && j->is_atom() ) {
               phase2_add( i->get_atom(), std::move( j->get_atom() ) );
               l.private_entries().erase( j );
               ++m_done;
            }
            else if( i->is_object() && j->is_object() ) {
               auto& o = i->get_object().private_map();
               auto& p = j->get_object().private_map();

               while( !p.empty() ) {
                  const auto& [ k, v ] = *p.begin();
                  const auto [ a, b ] = o.try_emplace( k, &*i, v );
                  if( !b ) {
                     a->second.append( v );
                  }
                  p.erase( p.begin() );
               }
               l.private_entries().erase( j );
               ++m_done;
            }
            else if( i->is_atom() && i->get_atom().is_object() && j->is_object() ) {
               auto& o = i->get_atom().get_object();
               auto& p = j->get_object().private_map();

               while( !o.empty() ) {
                  auto& [ k, v ] = *o.begin();
                  assert( v.position.has_value() );  // TODO: Make sure this is always true.
                  const auto a = p.try_emplace( k, &*i, v.position.value_or( j->get_object().position() ) ).first;
                  a->second.emplace_front_atom( std::move( v ) );
                  o.erase( o.begin() );
               }
               i = l.private_entries().erase( i );
               ++m_done;
            }
            else if( i->is_object() && j->is_atom() && j->get_atom().is_object() ) {
               auto& o = i->get_object().private_map();
               auto& p = j->get_atom().get_object();

               while( !p.empty() ) {
                  auto& [ k, v ] = *p.begin();
                  assert( v.position.has_value() );  // TODO: Make sure this is always true.
                  const auto a = o.try_emplace( k, &*i, v.position.value_or( i->get_object().position() ) ).first;
                  a->second.emplace_back_atom( std::move( v ) );
                  p.erase( p.begin() );
               }
               l.private_entries().erase( j );
               ++m_done;
            }
            else {
               ++i;
            }
         }
         if( ( l.entries().size() == 1 ) && l.entries().front().is_atom() ) {
            return &l.private_entries().front().get_atom();
         }
         return nullptr;
      }

      [[nodiscard]] bool process_array_entry( entry& e )
      {
         json_t j( json::empty_array, e.position() );

         bool done = true;

         for( auto& i : e.get_array().private_list() ) {
            if( const json_t* t = process_concat( i ) ) {
               if( !t->is_uninitialized() ) {
                  j.emplace_back( *t );
               }
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
               if( !t->is_uninitialized() ) {
                  j.try_emplace( i.first, *t );
               }
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

      bool process_entry( entry& e )
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

   inline json::basic_value< value_traits > phase2_process( entry& root, std::ostream* debug = nullptr )
   {
      for( phase2_processor p( root, debug ); !p.finished(); p.process() ) {
      }
      return root.get_atom();
   }

}  // namespace tao::config::internal

#endif
