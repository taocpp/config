// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE2_STATE_HPP
#define TAO_CONFIG_INTERNAL_PHASE2_STATE_HPP

#include <cassert>
#include <utility>
#include <vector>

#include "json.hpp"
#include "pegtl.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         struct phase2_state
         {
            template< typename Input >
            explicit phase2_state( const Input& in )
               : value( json::empty_array ),
                 m_position( in.position() )
            {
               rstack.emplace_back( &value );
            }

            phase2_state( const phase2_state& ) = delete;
            phase2_state( phase2_state&& ) = delete;

            ~phase2_state()
            {
               assert( rstack.size() == 1 );
            }

            void operator=( const phase2_state& ) = delete;
            void operator=( phase2_state&& ) = delete;

            template< typename Input, typename Consumer >
            void success( const Input&, Consumer& consumer )
            {
               consumer.indirect( m_position, std::move( value ) );
            }

            json::value value;

            std::vector< json::value* > rstack;

            const pegtl::position m_position;
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
