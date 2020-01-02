// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_STRING_STATE_HPP
#define TAO_CONFIG_INTERNAL_STRING_STATE_HPP

#include <cstddef>
#include <string>
#include <utility>

#include "pegtl.hpp"
#include "state.hpp"

namespace tao::config::internal
{
   struct string_state
   {
      template< typename Input >
      explicit string_state( const Input& in, state& )
         : m_position( in.position() )
      {
      }

      string_state( const string_state& ) = delete;
      string_state( string_state&& ) = delete;

      ~string_state() = default;

      void operator=( const string_state& ) = delete;
      void operator=( string_state&& ) = delete;

      template< typename Input >
      void success( const Input&, state& st )
      {
         assert( !st.lstack.empty() );

         st.lstack.back()->emplace_back_atom( m_position, std::move( unescaped ) );
      }

      std::string unescaped;

      const pegtl::position m_position;
   };

}  // namespace tao::config::internal

#endif
