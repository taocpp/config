// Copyright (c) 2020-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE2_REPLACE_HPP
#define TAO_CONFIG_INTERNAL_PHASE2_REPLACE_HPP

#include <set>
#include <stdexcept>

#include "array.hpp"
#include "concat.hpp"
#include "entry.hpp"
#include "forward.hpp"
#include "json.hpp"
#include "json_traits.hpp"
#include "object.hpp"

namespace tao::config::internal
{
   struct phase2_replace_impl
   {
      explicit phase2_replace_impl( object& root )
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
         auto i = c.concat.begin();

         while( i != c.concat.end() ) {
            process_entry( c, i );
         }
      }

      void process_entry( concat& c, std::list< entry >::iterator& i )
      {
         switch( i->kind() ) {
            case entry_kind::value:
               ++i;
               return;
            case entry_kind::reference:
               ++i;
               return;
            case entry_kind::array:
               if( c.concat.size() == 1 ) {
                  for( concat& d : i->get_array().array ) {
                     process_concat( d );
                  }
               }
               ++i;
               return;
            case entry_kind::object:
               if( c.concat.size() == 1 ) {
                  for( auto& p : i->get_object().object ) {
                     process_concat( p.second );
                  }
               }
               ++i;
               return;
            case entry_kind::concat:
               process_concat_entry( c, i );
               return;
         }
         throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
      }

      void process_concat_entry( concat& c, std::list< entry >::iterator& i )
      {
         const concat star = std::move( i->get_concat() );

         std::set< std::string > names;

         for( auto j = c.concat.begin(); j != i; ++j ) {
            switch( j->kind() ) {
               case entry_kind::value:
                  continue;
               case entry_kind::reference:
                  continue;
               case entry_kind::array:
                  process_array_concat_entry( j->get_array(), star );
                  continue;
               case entry_kind::object:
                  for( const auto& p : j->get_object().object ) {
                     names.emplace( p.first );
                  }
                  continue;
               case entry_kind::concat:
                  continue;
            }
         }
         for( auto j = i; j != c.concat.end(); ++j ) {
            switch( j->kind() ) {
               case entry_kind::value:
                  continue;
               case entry_kind::reference:
                  continue;
               case entry_kind::array:
                  process_concat_entry_array( star, j->get_array() );
                  continue;
               case entry_kind::object:
                  for( const auto& p : j->get_object().object ) {
                     names.emplace( p.first );
                  }
                  continue;
               case entry_kind::concat:
                  continue;
            }
         }
         ++m_changes;

         if( names.empty() ) {
            i = c.concat.erase( i );
            return;
         }
         i->set_object( star.position );

         for( const auto& name : names ) {
            [[maybe_unused]] const auto [ j, b ] = i->get_object().object.try_emplace( name, star );
            assert( b && ( j->second.temporary == star.temporary ) );
         }
         ++i;
      }

      void process_concat_entry_array( const concat& s, array& a )
      {
         for( concat& c : a.array ) {
            if( c.remove ) {
               continue;
            }
            c.remove = s.remove;
            c.temporary |= s.temporary;
            c.concat.insert( c.concat.begin(), s.concat.begin(), s.concat.end() );
         }
      }

      void process_array_concat_entry( array& a, const concat& s )
      {
         for( concat& c : a.array ) {
            if( s.remove ) {
               c.remove = true;
               c.concat.clear();
            }
            c.temporary |= s.temporary;
            c.concat.insert( c.concat.end(), s.concat.begin(), s.concat.end() );
         }
      }
   };

   [[nodiscard]] inline std::size_t phase2_replace( object& root )
   {
      return phase2_replace_impl( root ).process();
   }

}  // namespace tao::config::internal

#endif
