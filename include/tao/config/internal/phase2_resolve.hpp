// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE2_RESOLVE_HPP
#define TAO_CONFIG_INTERNAL_PHASE2_RESOLVE_HPP

#include <cassert>
#include <set>
#include <stdexcept>

#include "array.hpp"
#include "concat.hpp"
#include "entry.hpp"
#include "forward.hpp"
#include "json.hpp"
#include "json_traits.hpp"
#include "object.hpp"
#include "phase2_access.hpp"
#include "phase2_guard.hpp"
#include "string_utility.hpp"

namespace tao::config::internal
{
   struct phase2_resolve_impl
   {
      explicit phase2_resolve_impl( object& root )
         : m_root( root ),
           m_changes( 0 )
      {}

      [[nodiscard]] std::size_t process()
      {
         assert( m_stack.empty() );

         for( auto& p : m_root.object ) {
            process_concat( key1{ key1_part( p.first, m_root.position ) }, p.second );
         }
         assert( m_stack.empty() );
         return m_changes;
      }

   private:
      object& m_root;
      std::size_t m_changes;
      std::set< const void* > m_stack;

      void process_concat( const key1& prefix, concat& c )
      {
         const phase2_guard dog( m_stack, c );

         for( auto& e : c.concat ) {
            if( const concat* d = process_entry( prefix, e ); d != nullptr ) {
               assert( d != &c );  // TODO: This needs to be ensured elsewhere/in another way.

               if( !d->concat.empty() ) {
                  assert( d->concat.size() == 1 );

                  e = d->concat.front();
                  // TODO: Call phase2_combine( c ) to get things done quicker?
                  ++m_changes;
               }
            }
         }
      }

      [[nodiscard]] std::optional< key1_part > process_reference_part( const key1& prefix, const reference2_part& part )
      {
         switch( part.kind() ) {
            case reference2_kind::name:
               return key1_part( part.get_name(), part.position );
            case reference2_kind::index:
               return key1_part( part.get_index(), part.position );
            case reference2_kind::vector:
               return process_inner_reference( prefix, part.get_vector() );
         }
         throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
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
                     throw pegtl::parse_error( strcat( "invalid json type '", j->type(), "' for reference part" ), j->position );
               }
            }
         }
         return std::nullopt;
      }

      [[nodiscard]] const concat* process_entry( const key1& prefix, entry& e )
      {
         switch( e.kind() ) {
            case entry_kind::value:
               return nullptr;
            case entry_kind::reference:
               return process_reference_parts( prefix, e.get_reference() );
            case entry_kind::array: {
               std::size_t i = 0;
               for( auto& c : e.get_array().array ) {
                  process_concat( prefix + key1_part( i++, m_root.position ), c );
               }
               return nullptr;
            }
            case entry_kind::object:
               for( auto& p : e.get_object().object ) {
                  process_concat( prefix + key1_part( p.first, m_root.position ), p.second );
               }
               return nullptr;
            case entry_kind::concat:
               //               process_concat( prefix + key1_part( part_star, m_root.position ), e.get_concat() );
               return nullptr;
         }
         throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
      }
   };

   [[nodiscard]] inline std::size_t phase2_resolve( object& root )
   {
      return phase2_resolve_impl( root ).process();
   }

}  // namespace tao::config::internal

#endif
