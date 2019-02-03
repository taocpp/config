// Copyright (c) 2018-2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_CONCAT_HPP
#define TAO_CONFIG_INTERNAL_CONCAT_HPP

#include <cassert>
#include <deque>

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

            const std::deque< E >& entries() const noexcept
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

            std::deque< E >& private_entries() noexcept
            {
               return m_entries;
            }

         private:
            E* m_parent;  // TODO?
            std::deque< E > m_entries;

            mutable bool m_temporary = false;
         };

         class entry;

         using concat = basic_concat< entry >;

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
