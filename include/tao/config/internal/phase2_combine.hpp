// Copyright (c) 2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE2_COMBINE_HPP
#define TAO_CONFIG_INTERNAL_PHASE2_COMBINE_HPP

#include <cassert>

#include "array.hpp"
#include "concat.hpp"
#include "entry.hpp"
#include "forward.hpp"
#include "json.hpp"
#include "json_traits.hpp"
#include "object.hpp"
#include "phase2_concat.hpp"

namespace tao::config::internal
{
   struct phase2_combine_impl
   {
      explicit phase2_combine_impl( object& root )
         : m_root( root ),
           m_changes( 0 )
      {}

      [[nodiscard]] std::size_t process()
      {
         for( auto& p : m_root.object ) {
            process_concat( p.second );
         }
         return m_changes;
      }

   private:
      object& m_root;
      std::size_t m_changes;

      void process_concat( concat& c )
      {
         for( entry& e : c.concat ) {
            process_entry( e );
         }
         if( c.concat.size() < 2 ) {
            return;
         }
         for( auto r = ++c.concat.begin(); r != c.concat.end(); ++r ) {
            auto l = r;
            --l;
            switch( r->kind() ) {
               case entry_kind::value:
                  if( l->kind() == entry_kind::value ) {
                     phase2_concat( l->get_value(), std::move( r->get_value() ) );
                     r = c.concat.erase( r );
                     --r;
                     ++m_changes;
                     continue;
                  }
                  continue;
               case entry_kind::reference:
                  continue;
               case entry_kind::array:
                  assert( l->kind() != entry_kind::array );  // Should already be merged.
                  continue;
               case entry_kind::object:
                  assert( l->kind() != entry_kind::object );  // Should already be merged.
                  continue;
               case entry_kind::remove:
                  c.concat.erase( c.concat.begin(), r );
                  ++m_changes;
                  continue;
            }
            assert( false );  // UNREACHABLE
         }
      }

      void process_entry( entry& e )
      {
         switch( e.kind() ) {
            case entry_kind::value:
               return;
            case entry_kind::reference:
               return;
            case entry_kind::array:
               for( concat& c : e.get_array().array ) {
                  process_concat( c );
               }
               return;
            case entry_kind::object:
               for( auto& p : e.get_object().object ) {
                  process_concat( p.second );
               }
               return;
            case entry_kind::remove:
               return;
         }
         assert( false );  // UNREACHABLE
      }
   };

   [[nodiscard]] inline std::size_t phase2_combine( object& root )
   {
      return phase2_combine_impl( root ).process();
   }

}  // namespace tao::config::internal

#endif
