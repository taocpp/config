// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_QUOTED_STATE_HPP
#define TAO_CONFIG_INTERNAL_QUOTED_STATE_HPP

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
         struct quoted_state
         {
            template< typename Input >
            explicit quoted_state( const Input& in, state& )
               : m_position( in.position() )
            {
            }

            quoted_state( const quoted_state& ) = delete;
            quoted_state( quoted_state&& ) = delete;

            ~quoted_state() = default;

            void operator=( const quoted_state& ) = delete;
            void operator=( quoted_state&& ) = delete;

            template< typename Input >
            void success( const Input&, state& st )
            {
               assert( !st.rstack.empty() );

               st.rstack.back()->emplace_back( std::move( unescaped ) );
            }

            std::string unescaped;

            const position m_position;
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
