// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE1_STATE_HPP
#define TAO_CONFIG_INTERNAL_PHASE1_STATE_HPP

#include <cstddef>
#include <utility>
#include <string>

#include "pegtl.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         struct phase1_state
         {
            template< typename Input >
            explicit phase1_state( const Input& in )
               : m_position( in.position() )
            {
            }

            phase1_state( const phase1_state& ) = delete;
            phase1_state( phase1_state&& ) = delete;

            ~phase1_state() = default;

            void operator=( const phase1_state& ) = delete;
            void operator=( phase1_state&& ) = delete;

            template< typename Input, typename Consumer >
            void success( const Input&, Consumer& consumer )
            {
               consumer.key.emplace_back( std::move( unescaped ) );
            }

            std::string unescaped;

            const pegtl::position m_position;
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
