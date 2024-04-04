// Copyright (c) 2020-2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE2_ADDITIONS_HPP
#define TAO_CONFIG_INTERNAL_PHASE2_ADDITIONS_HPP

#include <cassert>
#include <cstddef>
#include <iterator>
#include <stdexcept>

#include "array.hpp"
#include "concat.hpp"
#include "entry.hpp"
#include "forward.hpp"
#include "json.hpp"
#include "object.hpp"
#include "string_utility.hpp"

namespace tao::config::internal
{
   struct phase2_additions_impl
   {
      explicit phase2_additions_impl( object& root )
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
         for( entry& e : c.concat ) {
            process_entry( e );
         }
         if( c.concat.size() < 2 ) {
            return;
         }
         for( auto r = ++c.concat.begin(); r != c.concat.end(); ++r ) {
            const auto l = std::prev( r );
            switch( r->kind() ) {
               case entry_kind::NULL_:
               case entry_kind::BOOLEAN:
                  throw_type_error( *l, *r );

               case entry_kind::SIGNED:
               case entry_kind::UNSIGNED:
                  if( ignore_entry( *l ) || ignore_entry( *r ) ) {
                     continue;
                  }
                  if( ( r->kind() == entry_kind::SIGNED ) ) {
                     if( l->kind() == entry_kind::SIGNED ) {
                        r->get_signed_atom().value += l->get_signed();
                        break;
                     }
                     else if( l->kind() == entry_kind::UNSIGNED ) {
                        r->get_signed_atom().value += std::int64_t( l->get_unsigned() );
                        break;
                     }
                  }
                  else {  // r->kind() == entry_kind::UNSIGNED
                     if( l->kind() == entry_kind::UNSIGNED ) {
                        r->get_unsigned_atom().value += l->get_unsigned();
                        break;
                     }
                     else if( l->kind() == entry_kind::SIGNED ) {
                        l->get_signed_atom().value += std::int64_t( r->get_unsigned() );
                        ( *r ) = ( *l );
                        break;
                     }
                  }
                  throw_type_error( *l, *r );

               case entry_kind::DOUBLE:
                  if( !throw_type_error_if( *l, *r, entry_kind::DOUBLE ) ) {
                     continue;
                  }
                  r->get_double_atom().value += l->get_double();
                  break;

               case entry_kind::STRING:
                  if( !throw_type_error_if( *l, *r, entry_kind::STRING ) ) {
                     continue;
                  }
                  r->get_string_atom().value = l->get_string() + r->get_string();
                  break;

               case entry_kind::BINARY:
                  if( !throw_type_error_if( *l, *r, entry_kind::BINARY ) ) {
                     continue;
                  }
                  r->get_binary_atom().value.insert( r->get_binary().begin(), l->get_binary().begin(), l->get_binary().end() );
                  break;

               case entry_kind::ARRAY:
                  if( !throw_type_error_if( *l, *r, entry_kind::ARRAY ) ) {
                     continue;
                  }
                  r->get_array().array.splice( r->get_array().array.begin(), l->get_array().array );  // throw_type_error_if returns false if l and/or r are functions.
                  break;

               case entry_kind::OBJECT:
                  if( !throw_type_error_if( *l, *r, entry_kind::OBJECT ) ) {
                     continue;
                  }
                  process_object( std::move( l->get_object() ), r->get_object() );
                  break;

               case entry_kind::ASTERISK:
               case entry_kind::REFERENCE:
                  continue;
            }
            [[maybe_unused]] const auto t = c.concat.erase( l );
            assert( t == r );
            ++m_changes;
         }
      }

      void process_entry( entry& e )
      {
         switch( e.kind() ) {
            case entry_kind::NULL_:
            case entry_kind::BOOLEAN:
            case entry_kind::STRING:
            case entry_kind::BINARY:
            case entry_kind::SIGNED:
            case entry_kind::UNSIGNED:
            case entry_kind::DOUBLE:
               return;
            case entry_kind::ARRAY:
               for( concat& c : e.get_array().array ) {
                  process_concat( c );
               }
               return;
            case entry_kind::OBJECT:
               for( auto& p : e.get_object().object ) {
                  process_concat( p.second );
               }
               return;
            case entry_kind::ASTERISK:
               process_concat( e.get_asterisk() );
               return;
            case entry_kind::REFERENCE:
               return;
         }
         throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
      }

      [[nodiscard]] static bool ignore_entry( const entry& e ) noexcept
      {
         if( ( e.kind() == entry_kind::ASTERISK ) || ( e.kind() == entry_kind::REFERENCE ) ) {
            return true;
         }
         if( ( e.kind() == entry_kind::ARRAY ) && ( !e.get_array().function.empty() ) ) {
            return true;
         }
         return false;
      }

      [[nodiscard]] static bool throw_type_error_if( const entry& l, const entry& r, const entry_kind k )
      {
         if( ignore_entry( l ) ) {
            return false;
         }
         if( ignore_entry( r ) ) {
            return false;
         }
         if( l.kind() == k ) {
            return true;
         }
         throw_type_error( l, r );
      }

      [[noreturn]] static void throw_type_error( const entry& l, const entry& r )
      {
         throw pegtl::parse_error( strcat( "incompatible or invalid type(s) in addition ", l.kind(), "@", l.get_position(), " and ", r.kind(), "@", r.get_position() ), pegtl::position( 0, 0, 0, "TODO: location of '+'" ) );
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

   [[nodiscard]] inline std::size_t phase2_additions( object& root )
   {
      return phase2_additions_impl( root ).process();
   }

}  // namespace tao::config::internal

#endif
