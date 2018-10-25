// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_KEY_STRING_STATE_HPP
#define TAO_CONFIG_INTERNAL_KEY_STRING_STATE_HPP

#include <cstddef>
#include <utility>
#include <string>

#include "pegtl.hpp"
#include "state.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         struct key_string_state
         {
            template< typename Input >
            explicit key_string_state( const Input& in )
               : m_position( in.position() )
            {
            }

            key_string_state( const key_string_state& ) = delete;
            key_string_state( key_string_state&& ) = delete;

            ~key_string_state() = default;

            void operator=( const key_string_state& ) = delete;
            void operator=( key_string_state&& ) = delete;

            template< typename Input >
            void success( const Input&, state& st )
            {
               st.key.emplace_back( std::move( unescaped ) );
            }

            std::string unescaped;

            const pegtl::position m_position;
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
