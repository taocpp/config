// Copyright (c) 2018-2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_CONCAT_HPP
#define TAO_CONFIG_INTERNAL_CONCAT_HPP

#include <cassert>
#include <list>

#include "json.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
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

            void clear()
            {
               m_entries.clear();
            }

            void append( const basic_concat& other )
            {
               for( const auto& i : other.m_entries ) {
                  m_entries.emplace_back( this, i );
               }
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
            }

            E& emplace_back_array( const position& pos )
            {
               auto& e = m_entries.emplace_back( this, pos );
               e.set_array();
               return e;
            }

            E& emplace_back_object( const position& pos )
            {
               auto& e = m_entries.emplace_back( this, pos );
               e.set_object();
               return e;
            }

            json::value& emplace_back_reference( const position& pos, json::value&& v )
            {
               auto& e = m_entries.emplace_back( this, pos );
               e.set_reference( std::move( v ) );
               return e.get_reference();
            }

            position p;

            std::list< E >& private_entries() noexcept
            {
               return m_entries;
            }

            void front_set_clear() noexcept
            {
               assert( !m_entries.empty() );

               m_entries.front().set_clear();
            }

            void post_array_merge()
            {
               //               assert( !m_entries.empty() );
               //               assert( m_entries.back().is_array() );

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
               //               assert( !m_entries.empty() );
               //               assert( m_entries.back().is_object() );

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
            E* m_parent;  // TODO?
            std::list< E > m_entries;

            mutable bool m_temporary = false;
         };

         class entry;

         using concat = basic_concat< entry >;

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
