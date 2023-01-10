// Copyright (c) 2020-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_KEY1_GUARD_HPP
#define TAO_CONFIG_INTERNAL_KEY1_GUARD_HPP

#include <cassert>

#include "key1.hpp"
#include "pegtl.hpp"

namespace tao::config::internal
{
   class [[nodiscard]] key1_guard
   {
   public:
      key1_guard() = delete;

      template< typename State >
      key1_guard( State& st, key1&& suffix )
         : m_prefix( st.prefix ),
           m_suffix( st.suffix ),
           m_size( m_prefix.size() )
      {
         m_prefix += m_suffix;
         m_suffix = std::move( suffix );
      }

      template< typename State, typename Arg >
      key1_guard( const pegtl_input_t& in, State& st, const Arg& /*unused*/ )
         : m_prefix( st.prefix ),
           m_suffix( st.suffix ),
           m_size( m_prefix.size() )
      {
         m_prefix += m_suffix;
         m_suffix.clear();
         m_suffix.emplace_back( in.position(), ++st.generation );
      }

      key1_guard( key1_guard&& ) = delete;
      key1_guard( const key1_guard& ) = delete;

      ~key1_guard()
      {
         assert( m_prefix.size() >= m_size );

         m_suffix.clear();
         m_suffix.insert( m_suffix.end(), m_prefix.begin() + m_size, m_prefix.end() );
         m_prefix.erase( m_prefix.begin() + m_size, m_prefix.end() );
      }

      void operator=( key1_guard&& ) = delete;
      void operator=( const key1_guard& ) = delete;

   private:
      key1& m_prefix;
      key1& m_suffix;

      const std::size_t m_size;
   };

}  // namespace tao::config::internal

#endif
