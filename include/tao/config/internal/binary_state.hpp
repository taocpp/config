// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_BINARY_STATE_HPP
#define TAO_CONFIG_INTERNAL_BINARY_STATE_HPP

#include <cstddef>
#include <utility>
#include <vector>

#include "pegtl.hpp"
#include "state.hpp"

namespace tao::config::internal
{
   struct binary_state
   {
      template< typename Input >
      explicit binary_state( const Input& in, state& )
         : m_position( in.position() )
      {
      }

      binary_state( const binary_state& ) = delete;
      binary_state( binary_state&& ) = delete;

      ~binary_state() = default;

      void operator=( const binary_state& ) = delete;
      void operator=( binary_state&& ) = delete;

      template< typename Input >
      void success( const Input&, state& st )
      {
         assert( !st.lstack.empty() );

         st.lstack.back()->emplace_back_atom( m_position, std::move( value ) );
      }

      std::vector< std::byte > value;

      const pegtl::position m_position;
   };

}  // namespace tao::config::internal

#endif
