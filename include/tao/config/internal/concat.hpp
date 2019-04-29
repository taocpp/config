// Copyright (c) 2018-2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_CONCAT_HPP
#define TAO_CONFIG_INTERNAL_CONCAT_HPP

#include <cassert>
#include <list>

#include "json.hpp"

namespace tao::config::internal
{
   template< typename E >
   class basic_concat
   {
   public:
      basic_concat( E* parent, const position& pos )
         : p( pos ),
           m_parent( parent )
      {
         assert( parent );
      }

      basic_concat( E* parent, const basic_concat& r )
         : p( r.p ),
           m_parent( parent )
      {
         for( const auto& e : r.m_entries ) {
            m_entries.emplace_back( this, e );
         }
         assert( parent );
      }

      basic_concat( basic_concat&& ) = delete;
      basic_concat( const basic_concat& ) = delete;

      basic_concat& operator=( basic_concat&& ) = delete;
      basic_concat& operator=( const basic_concat& ) = delete;

      ~basic_concat() = default;

      E& parent() const noexcept
      {
         return *m_parent;
      }

      bool is_temporary() const noexcept
      {
         return m_temporary;
      }

      void set_temporary( const bool t = true ) noexcept
      {
         m_temporary = t;
      }

      void clear( bool& c )
      {
         if( c ) {
            c = false;
            m_clear = true;
            m_entries.clear();
         }
      }

      bool empty() const noexcept
      {
         return m_entries.empty();
      }

      std::size_t size() const noexcept
      {
         return m_entries.size();
      }

      void append( const basic_concat& other )
      {
         if( !other.m_entries.empty() ) {
            m_entries.emplace_back( this, other.m_entries.front() );
            m_entries.back().set_clear( m_clear );
            // Assumption is that one of the two following functions might do something, and that's enough to keep an array/non-array or object/non-object alternation.
            post_array_merge();
            post_object_merge();
            for( auto i = ++other.m_entries.begin(); i != other.m_entries.end(); ++i ) {
               m_entries.emplace_back( this, *i );
            }
         }
         m_clear = false;
      }

      const std::list< E >& entries() const noexcept
      {
         return m_entries;
      }

      template< typename T >
      void emplace_back_atom( const position& pos, T&& t )
      {
         auto& e = m_entries.emplace_back( this, pos );
         e.set_atom( std::forward< T >( t ) );
         back_set_clear();
      }

      E& emplace_back_array( const position& pos )
      {
         auto& e = m_entries.emplace_back( this, pos );
         e.set_array();
         back_set_clear();
         return e;
      }

      E& emplace_back_object( const position& pos )
      {
         auto& e = m_entries.emplace_back( this, pos );
         e.set_object();
         back_set_clear();
         return e;
      }

      json::value& emplace_back_reference( const position& pos, json::value&& v )
      {
         auto& e = m_entries.emplace_back( this, pos );
         e.set_reference( std::move( v ) );
         back_set_clear();
         return e.get_reference();
      }

      position p;

      std::list< E >& private_entries() noexcept
      {
         return m_entries;
      }

      void post_array_merge()
      {
         if( m_entries.size() >= 2 ) {
            if( m_entries.back().clear() ) {
               m_entries.erase( m_entries.begin(), --m_entries.end() );
               return;
            }
            if( E& e = *----m_entries.end(); e.is_array() && m_entries.back().is_array() ) {
               for( const basic_concat& j : m_entries.back().get_array() ) {
                  e.get_array().emplace_back( &e, j );
               }
               m_entries.pop_back();
            }
         }
      }

      void post_object_merge()
      {
         if( m_entries.size() >= 2 ) {
            if( m_entries.back().clear() ) {
               m_entries.erase( m_entries.begin(), --m_entries.end() );
               return;
            }
            if( E& e = *----m_entries.end(); e.is_object() && m_entries.back().is_object() ) {
               for( const auto& j : m_entries.back().get_object() ) {
                  if( const auto p = e.get_object().try_emplace( j.first, &e, j.second ); !p.second ) {
                     for( const E& k : j.second.m_entries ) {
                        p.first->second.m_entries.emplace_back( &p.first->second, k );
                        p.first->second.post_object_merge();
                     }
                  }
               }
               m_entries.pop_back();
            }
         }
      }

   private:
      void back_set_clear()
      {
         assert( !m_entries.empty() );

         if( m_clear ) {
            m_entries.back().set_clear();
            m_clear = false;
         }
      }

      E* m_parent;  // TODO?
      std::list< E > m_entries;

      bool m_clear = false;
      mutable bool m_temporary = false;
   };

   class entry;

   using concat = basic_concat< entry >;

}  // namespace tao::config::internal

#endif
