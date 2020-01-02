// Copyright (c) 2019-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_ENTRY_OBJECT_HPP
#define TAO_CONFIG_INTERNAL_ENTRY_OBJECT_HPP

#include <map>
#include <stdexcept>
#include <string>

#include "concat.hpp"
#include "forward.hpp"
#include "pegtl.hpp"

namespace tao::config::internal
{
   class entry_object
   {
   public:
      explicit entry_object( const pegtl::position& pos )
         : m_position( pos )
      {
      }

      entry_object( entry_object&& ) = delete;
      entry_object( const entry_object& ) = delete;

      void operator=( entry_object&& ) = delete;
      void operator=( const entry_object& ) = delete;

      std::size_t size() const noexcept
      {
         return m_map.size();
      }

      bool empty() const noexcept
      {
         return m_map.empty();
      }

      void clear()
      {
         m_map.clear();
      }

      std::size_t erase( const std::string& k )
      {
         return m_map.erase( k );
      }

      std::pair< const std::string, concat >* find( const std::string& k ) noexcept
      {
         const auto i = m_map.find( k );
         return ( i == m_map.end() ) ? nullptr : ( &*i );
      }

      const std::pair< const std::string, concat >* find( const std::string& k ) const noexcept
      {
         const auto i = m_map.find( k );
         return ( i == m_map.end() ) ? nullptr : ( &*i );
      }

      auto try_emplace( const std::string& k, entry* e, const concat& list )
      {
         return m_map.try_emplace( k, e, list );
      }

      auto try_emplace( const std::string& k, entry* e, const pegtl::position& pos )
      {
         return m_map.try_emplace( k, e, pos );
      }

      const pegtl::position& position() const noexcept
      {
         return m_position;
      }

      std::map< std::string, concat >& private_map() noexcept
      {
         return m_map;
      }

      const std::map< std::string, concat >& map() const noexcept
      {
         return m_map;
      }

   private:
      pegtl::position m_position;
      std::map< std::string, concat > m_map;
   };

}  // namespace tao::config::internal

#endif
