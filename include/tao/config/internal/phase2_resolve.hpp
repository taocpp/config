// Copyright (c) 2019-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE2_RESOLVE_HPP
#define TAO_CONFIG_INTERNAL_PHASE2_RESOLVE_HPP

#include <cassert>
#include <set>

#include "array.hpp"
#include "concat.hpp"
#include "entry.hpp"
#include "phase2_guard.hpp"
#include "forward.hpp"
#include "json.hpp"
#include "json_traits.hpp"
#include "object.hpp"
#include "phase2_access.hpp"

namespace tao::config::internal
{
   struct phase2_resolve_impl
   {
      explicit phase2_resolve_impl( object& root )
         : m_root( root )
      {}

      [[nodiscard]] std::size_t process()
      {
         assert( m_stack.empty() );

         const std::size_t references = m_root.all_references();

         for( auto& p : m_root.object ) {
            process_concat( key1{ key1_part( p.first, m_root.position ) }, p.second );
         }
         assert( m_stack.empty() );

         return references - m_root.all_references();
      }

   private:
      object& m_root;
      std::set< const void* > m_stack;

      void process_concat( const key1& prefix, concat& c )
      {
         const phase2_guard dog( m_stack, c );

         auto i = c.concat.begin();

         while( i != c.concat.end() ) {
            if( const concat* d = process_entry( prefix, *i ) ) {
               for( const entry& e : d->concat ) {
                  auto j = c.concat.emplace( i, e );
                  j->make_permanent();
                  c.post_insert_merge( j );
               }
               i = c.concat.erase( i );
               if( ( i != c.concat.end() ) && ( i != c.concat.begin() ) ) {
                  auto j = i;
                  ++i;
                  c.post_insert_merge( j );
               }
               continue;
            }
            ++i;
         }
      }

      [[nodiscard]] std::optional< key1_part > process_reference_part( const key1& prefix, const reference2_part& part )
      {
         switch( part.kind() ) {
            case reference2_kind::minus:
               return key1_part( part_minus, part.position );
            case reference2_kind::name:
               return key1_part( part.get_name(), part.position );
            case reference2_kind::index:
               return key1_part( part.get_index(), part.position );
            case reference2_kind::vector:
               return process_inner_reference( prefix, part.get_vector() );
         }
         assert( false );  // UNREACHABLE
      }

      [[nodiscard]] const concat* process_reference_parts( const key1& prefix, const std::vector< reference2_part >& reference )
      {
         key1 suffix;

         for( auto& p : reference ) {
            if( const std::optional< key1_part > k = process_reference_part( prefix, p ) ) {
               suffix += *k;
               continue;
            }
            return nullptr;
         }
         assert( !prefix.empty() );

         return phase2_access( m_root, pop_back( prefix ), suffix );
      }

      [[nodiscard]] std::optional< key1_part > process_inner_reference( const key1& prefix, const std::vector< reference2_part >& reference )
      {
         if( const concat* c = process_reference_parts( prefix, reference ) ) {
            if( const json_t* j = c->get_value() ) {
               switch( j->type() ) {
                  case json::type::STRING:
                  case json::type::STRING_VIEW:
                     return key1_part( j->as< std::string >(), j->position );
                  case json::type::SIGNED:
                  case json::type::UNSIGNED:
                     return key1_part( j->as< std::size_t >(), j->position );
                  default:
                     throw std::string( "invalid json type for nested reference part" );
               }
            }
         }
         return std::nullopt;
      }

      [[nodiscard]] const concat* process_entry( const key1& prefix, entry& e )
      {
         const phase2_guard dog( m_stack, e );

         switch( e.kind() ) {
            case entry_kind::value:
               return nullptr;
            case entry_kind::reference:
               return process_reference_parts( prefix, e.get_reference() );
            case entry_kind::array:
               for( std::size_t i = 0; i < e.get_array().array.size(); ++i ) {
                  auto j = e.get_array().array.begin();
                  std::advance( j, i );  // Yeah, we're not overly concerned with O(n^2) ... yet.
                  process_concat( prefix + key1_part( i, m_root.position ), *j );
               }
               return nullptr;
            case entry_kind::object:
               for( auto& p : e.get_object().object ) {
                  process_concat( prefix + key1_part( p.first, m_root.position ), p.second );
               }
               return nullptr;
            case entry_kind::remove:
               return nullptr;
         }
         assert( false );  // UNREACHABLE
      }
   };

   [[nodiscard]] inline std::size_t phase2_resolve( object& root )
   {
      return phase2_resolve_impl( root ).process();
   }

}  // namespace tao::config::internal

#endif
