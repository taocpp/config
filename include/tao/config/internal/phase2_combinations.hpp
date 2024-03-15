// Copyright (c) 2020-2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE2_COMBINATIONS_HPP
#define TAO_CONFIG_INTERNAL_PHASE2_COMBINATIONS_HPP

#include <cassert>
#include <cstddef>
#include <iterator>
#include <stdexcept>

#include "array.hpp"
#include "concat.hpp"
#include "entry.hpp"
#include "forward.hpp"
#include "json.hpp"
#include "json_traits.hpp"
#include "object.hpp"
#include "phase2_additions.hpp"

namespace tao::config::internal
{
   struct phase2_combinations_impl
   {
      explicit phase2_combinations_impl( object& root )
         : m_root( root )
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
      std::size_t m_changes = 0;

      void process_concat( concat& c )
      {
         if( c.concat.empty() ) {
            return;
         }
         for( entry& e : c.concat ) {
            process_entry( e );
         }
         if( c.concat.size() == 1 ) {
            return;
         }
         // assert( c.concat.size() >= 2 );

         for( auto r = ++c.concat.begin(); r != c.concat.end(); ++r ) {
            const auto l = std::prev( r );
            switch( r->kind() ) {
               case entry_kind::value:
                  if( l->kind() == entry_kind::value ) {
                     phase2_additions( std::move( l->get_value() ), r->get_value() );
                     [[maybe_unused]] const auto t = c.concat.erase( l );
                     assert( t == r );
                     ++m_changes;
                  }
                  continue;
               case entry_kind::reference:
                  continue;
               case entry_kind::array:
                  if( ( l->kind() == entry_kind::array ) && ( l->get_array().function.empty() ) && ( r->get_array().function.empty() ) ) {
                     process_array( std::move( l->get_array() ), r->get_array() );
                     [[maybe_unused]] const auto t = c.concat.erase( l );
                     assert( t == r );
                     ++m_changes;
                  }
                  continue;
               case entry_kind::object:
                  if( l->kind() == entry_kind::object ) {
                     process_object( std::move( l->get_object() ), r->get_object() );
                     [[maybe_unused]] const auto t = c.concat.erase( l );
                     assert( t == r );
                     ++m_changes;
                  }
                  continue;
               case entry_kind::concat:
                  continue;
            }
            throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
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
            case entry_kind::concat:
               process_concat( e.get_concat() );
               return;
         }
         throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
      }

      static void process_array( array&& l, array& r )
      {
         r.array.splice( r.array.begin(), l.array );
      }

      static void process_object( object&& l, object& r )
      {
         for( std::pair< const std::string, concat >& m : l.object ) {
            const auto pair = r.object.try_emplace( m.first, m.second );
            if( !pair.second ) {
               if( pair.first->second.remove ) {
                  continue;
               }
               if( m.second.remove ) {
                  pair.first->second.remove = true;
               }
               if( m.second.temporary ) {
                  pair.first->second.temporary = true;
               }
               pair.first->second.concat.splice( pair.first->second.concat.begin(), m.second.concat );
            }
         }
      }
   };

   [[nodiscard]] inline std::size_t phase2_combinations( object& root )
   {
      return phase2_combinations_impl( root ).process();
   }

}  // namespace tao::config::internal

#endif
