// Copyright (c) 2020-2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE2_ASTERISKS_HPP
#define TAO_CONFIG_INTERNAL_PHASE2_ASTERISKS_HPP

#include <cstddef>
#include <list>
#include <set>
#include <stdexcept>
#include <string>

#include "array.hpp"
#include "concat.hpp"
#include "entry.hpp"
#include "forward.hpp"
#include "json.hpp"
#include "object.hpp"

namespace tao::config::internal
{
   struct phase2_asterisks_impl
   {
      explicit phase2_asterisks_impl( object& root )
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
         auto i = c.concat.begin();

         while( i != c.concat.end() ) {
            process_entry( c, i );
         }
      }

      void process_entry( concat& c, std::list< entry >::iterator& i )
      {
         switch( i->kind() ) {
            case entry_kind::NULL_:
            case entry_kind::BOOLEAN:
            case entry_kind::STRING:
            case entry_kind::BINARY:
            case entry_kind::SIGNED:
            case entry_kind::UNSIGNED:
            case entry_kind::DOUBLE:
               ++i;
               return;
            case entry_kind::ARRAY:
               if( c.concat.size() == 1 ) {
                  for( concat& d : i->get_array().array ) {
                     process_concat( d );
                  }
               }
               ++i;
               return;
            case entry_kind::OBJECT:
               if( c.concat.size() == 1 ) {
                  for( auto& p : i->get_object().object ) {
                     process_concat( p.second );
                  }
               }
               ++i;
               return;
            case entry_kind::ASTERISK:
               process_asterisk( c, i );
               return;
            case entry_kind::REFERENCE:
               ++i;
               return;
         }
         throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
      }

      void process_asterisk( concat& c, std::list< entry >::iterator& i )
      {
         const concat star = std::move( i->get_asterisk() );

         std::set< std::string > names;

         for( auto j = c.concat.begin(); j != i; ++j ) {
            switch( j->kind() ) {
               case entry_kind::NULL_:
               case entry_kind::BOOLEAN:
               case entry_kind::STRING:
               case entry_kind::BINARY:
               case entry_kind::SIGNED:
               case entry_kind::UNSIGNED:
               case entry_kind::DOUBLE:
                  continue;
               case entry_kind::ARRAY:
                  if( !j->get_array().function.empty() ) {
                     throw pegtl::parse_error( "please do not use an asterisk inside of a function", j->get_array().position );
                  }
                  process_array_and_asterisk( j->get_array(), star );
                  continue;
               case entry_kind::OBJECT:
                  for( const auto& p : j->get_object().object ) {
                     names.emplace( p.first );
                  }
                  continue;
               case entry_kind::ASTERISK:
               case entry_kind::REFERENCE:
                  continue;
            }
         }
         for( auto j = i; j != c.concat.end(); ++j ) {
            switch( j->kind() ) {
               case entry_kind::NULL_:
               case entry_kind::BOOLEAN:
               case entry_kind::STRING:
               case entry_kind::BINARY:
               case entry_kind::SIGNED:
               case entry_kind::UNSIGNED:
               case entry_kind::DOUBLE:
                  continue;
               case entry_kind::ARRAY:
                  if( !j->get_array().function.empty() ) {
                     throw pegtl::parse_error( "please do not use an asterisk inside of a function", j->get_array().position );
                  }
                  process_asterisk_and_array( star, j->get_array() );
                  continue;
               case entry_kind::OBJECT:
                  for( const auto& p : j->get_object().object ) {
                     names.emplace( p.first );
                  }
                  continue;
               case entry_kind::ASTERISK:
               case entry_kind::REFERENCE:
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

      void process_asterisk_and_array( const concat& s, array& a )
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

      void process_array_and_asterisk( array& a, const concat& s )
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

   [[nodiscard]] inline std::size_t phase2_asterisks( object& root )
   {
      return phase2_asterisks_impl( root ).process();
   }

}  // namespace tao::config::internal

#endif
