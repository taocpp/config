// Copyright (c) 2019-2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE2_REFERENCES_HPP
#define TAO_CONFIG_INTERNAL_PHASE2_REFERENCES_HPP

#include <cassert>
#include <cstddef>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

#include "array.hpp"
#include "concat.hpp"
#include "entry.hpp"
#include "forward.hpp"
#include "json.hpp"
#include "object.hpp"
#include "phase2_access.hpp"
#include "string_utility.hpp"

namespace tao::config::internal
{
   struct phase2_references_impl
   {
      explicit phase2_references_impl( object& root )
         : m_root( root )
      {}

      [[nodiscard]] std::size_t process()
      {
         for( auto& p : m_root.object ) {
            process_concat( key1{ key1_part( p.first, m_root.position ) }, p.second );
         }
         return m_changes;
      }

   private:
      object& m_root;
      std::size_t m_changes = 0;

      void process_concat( const key1& prefix, concat& c )
      {
         for( auto& e : c.concat ) {
            if( const concat* d = process_entry( prefix, e ); d != nullptr ) {
               assert( d != &c );  // TODO: This needs to be ensured elsewhere/in another way.

               if( !d->concat.empty() ) {
                  assert( d->concat.size() == 1 );

                  e = d->concat.front();
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

         return phase2_access( m_root, pop_back( prefix ), suffix );  // Returns nullptr if not primitive.
      }

      [[nodiscard]] std::optional< key1_part > process_inner_reference( const key1& prefix, const std::vector< reference2_part >& reference )
      {
         if( const concat* c = process_reference_parts( prefix, reference ) ) {
            if( c->concat.size() == 1 ) {
               const entry& e = c->concat.back();
               switch( e.kind() ) {
                  case entry_kind::STRING:
                     return key1_part( e.get_string(), e.get_string_atom().position );
                  case entry_kind::UNSIGNED:
                     return key1_part( e.get_unsigned(), e.get_unsigned_atom().position );
                  case entry_kind::NULL_:
                  case entry_kind::BOOLEAN:
                  case entry_kind::BINARY:
                  case entry_kind::SIGNED:
                  case entry_kind::DOUBLE:
                  case entry_kind::ARRAY:
                  case entry_kind::OBJECT:
                  case entry_kind::ASTERISK:
                  case entry_kind::REFERENCE:
                     throw pegtl::parse_error( strcat( "invalid type '", e.kind(), "' for reference part" ), e.get_position() );
               }
            }
         }
         return std::nullopt;
      }

      [[nodiscard]] const concat* process_entry( const key1& prefix, entry& e )
      {
         switch( e.kind() ) {
            case entry_kind::NULL_:
            case entry_kind::BOOLEAN:
            case entry_kind::STRING:
            case entry_kind::BINARY:
            case entry_kind::SIGNED:
            case entry_kind::UNSIGNED:
            case entry_kind::DOUBLE:
               return nullptr;
            case entry_kind::ARRAY: {
               std::size_t i = 0;
               for( auto& c : e.get_array().array ) {
                  process_concat( prefix + key1_part( i++, m_root.position ), c );
               }
               return nullptr;
            }
            case entry_kind::OBJECT:
               for( auto& p : e.get_object().object ) {
                  process_concat( prefix + key1_part( p.first, m_root.position ), p.second );
               }
               return nullptr;
            case entry_kind::ASTERISK:
               return nullptr;
            case entry_kind::REFERENCE:
               return process_reference_parts( prefix, e.get_reference() );
         }
         throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
      }
   };

   [[nodiscard]] inline std::size_t phase2_references( object& root )
   {
      return phase2_references_impl( root ).process();
   }

}  // namespace tao::config::internal

#endif
