// Copyright (c) 2019-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_ENTRY_ARRAY_HPP
#define TAO_CONFIG_INTERNAL_ENTRY_ARRAY_HPP

#include <iterator>
#include <list>
#include <stdexcept>

#include "concat.hpp"
#include "forward.hpp"
#include "pegtl.hpp"

namespace tao::config::internal
{
   class entry_array
   {
   public:
      explicit entry_array( const pegtl::position& pos )
         : m_position( pos )
      {
      }

      entry_array( entry_array&& ) = delete;
      entry_array( const entry_array& ) = delete;

      void operator=( entry_array&& ) = delete;
      void operator=( const entry_array& ) = delete;

      std::size_t size() const noexcept
      {
         return m_list.size();
      }

      bool empty() const noexcept
      {
         return m_list.empty();
      }

      void clear()
      {
         m_list.clear();
      }

      void erase( const std::size_t n )
      {
         assert( n < m_list.size() );

         auto i = m_list.begin();
         std::advance( i, n );
         m_list.erase( i );
      }

      void pop_back()
      {
         assert( !m_list.empty() );

         m_list.pop_back();
      }

      concat& back() noexcept
      {
         assert( !m_list.empty() );

         return m_list.back();
      }

      const concat& back() const noexcept
      {
         assert( !m_list.empty() );

         return m_list.back();
      }

      concat& operator[]( const std::size_t n ) noexcept
      {
         assert( n < m_list.size() );

         auto i = m_list.begin();
         std::advance( i, n );
         return *i;
      }

      const concat& operator[]( const std::size_t n ) const noexcept
      {
         assert( n < m_list.size() );

         auto i = m_list.begin();
         std::advance( i, n );
         return *i;
      }

      concat& emplace_back( entry* e, const concat& list )
      {
         return m_list.emplace_back( e, list );
      }

      concat& emplace_back( entry* e, const pegtl::position& pos )
      {
         return m_list.emplace_back( e, pos );
      }

      const pegtl::position& position() const noexcept
      {
         return m_position;
      }

      std::list< concat >& private_list() noexcept
      {
         return m_list;
      }

      const std::list< concat >& list() const noexcept
      {
         return m_list;
      }

   private:
      pegtl::position m_position;
      std::list< concat > m_list;
   };

}  // namespace tao::config::internal

#endif
