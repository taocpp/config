// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_REFERENCE_STATE_HPP
#define TAO_CONFIG_INTERNAL_REFERENCE_STATE_HPP

#include <cassert>
#include <utility>
#include <vector>

#include "json.hpp"
#include "pegtl.hpp"
#include "state.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         struct reference_state
         {
            template< typename Input >
            explicit reference_state( const Input& in )
               : value( json::empty_array ),
                 m_position( in.position() )
            {
               rstack.emplace_back( &value );
            }

            reference_state( const reference_state& ) = delete;
            reference_state( reference_state&& ) = delete;

            ~reference_state()
            {
               assert( rstack.size() == 1 );
            }

            void operator=( const reference_state& ) = delete;
            void operator=( reference_state&& ) = delete;

            template< typename Input >
            void success( const Input&, state& st )
            {
               st.reference( m_position, std::move( value ) );
            }

            json::value value;

            std::vector< json::value* > rstack;

            const pegtl::position m_position;
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
