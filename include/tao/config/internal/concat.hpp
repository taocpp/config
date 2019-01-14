// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_CONCAT_HPP
#define TAO_CONFIG_INTERNAL_CONCAT_HPP

#include <vector>

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
            explicit
            basic_concat( const position& p )
               : p( p )
            {
            }

            basic_concat( basic_concat&& ) = default;
            basic_concat( const basic_concat& ) = default;

            basic_concat& operator=( basic_concat&& ) = default;
            basic_concat& operator=( const basic_concat& ) = default;

            ~basic_concat() = default;

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
               m_entries.insert( m_entries.end(), other.m_entries.begin(), other.m_entries.end() );
            }

            const std::vector< E >& entries() const noexcept
            {
               return m_entries;
            }

            template< typename T >
            void emplace_back_atom( const position& pos, T&& t )
            {
               m_entries.emplace_back( E::make_atom( this, pos, t ) );
            }

            std::vector< basic_concat< E > >& emplace_back_array( const position& pos )
            {
               return m_entries.emplace_back( E::make_array( this, pos ) ).get_array();
            }

            std::map< std::string, basic_concat< E > >& emplace_back_object( const position& pos )
            {
               return m_entries.emplace_back( E::make_object( this, pos ) ).get_object();
            }

            json::value& emplace_back_reference( const position& pos, json::value&& v )
            {
               return m_entries.emplace_back( E::make_reference( this, pos, std::move( v ) ) ).get_reference();
            }

            position p;

            std::vector< E >& private_entries() noexcept
            {
               return m_entries;
            }

         private:
            std::vector< E > m_entries;

            mutable bool m_temporary = false;
         };

         class entry;

         using concat = basic_concat< entry >;

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
