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

            basic_concat( basic_concat&& ) = default;
            basic_concat( const basic_concat& ) = default;

            basic_concat& operator=( basic_concat&& ) = default;
            basic_concat& operator=( const basic_concat& ) = default;

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
               const std::size_t s = m_entries.size();
               m_entries.insert( m_entries.end(), other.m_entries.begin(), other.m_entries.end() );
               for( std::size_t i = s; i < m_entries.size(); ++i ) {
                  m_entries[ i ].fix_parents( this );
               }
            }

            const std::deque< E >& entries() const noexcept
            {
               return m_entries;
            }

            template< typename T >
            void emplace_back_atom( const position& pos, T&& t )
            {
               m_entries.emplace_back( E::make_atom( this, pos, t ) );
            }

            E& emplace_back_array( const position& pos )
            {
               return m_entries.emplace_back( E::make_array( this, pos ) );
            }

            E& emplace_back_object( const position& pos )
            {
               return m_entries.emplace_back( E::make_object( this, pos ) );
            }

            json::value& emplace_back_reference( const position& pos, json::value&& v )
            {
               return m_entries.emplace_back( E::make_reference( this, pos, std::move( v ) ) ).get_reference();
            }

            position p;

            std::deque< E >& private_entries() noexcept
            {
               return m_entries;
            }

            void fix_parents( E* parent ) noexcept
            {
               for( auto& i : m_entries ) {
                  i.fix_parents( this );
               }
               m_parent = parent;
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
