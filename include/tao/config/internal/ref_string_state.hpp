// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_REF_STRING_STATE_HPP
#define TAO_CONFIG_INTERNAL_REF_STRING_STATE_HPP

#include <cstddef>
#include <utility>
#include <string>

#include "pegtl.hpp"
#include "reference_state.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         struct ref_string_state
         {
            template< typename Input >
            explicit ref_string_state( const Input& in )
               : m_position( in.position() )
            {
            }

            ref_string_state( const ref_string_state& ) = delete;
            ref_string_state( ref_string_state&& ) = delete;

            ~ref_string_state() = default;

            void operator=( const ref_string_state& ) = delete;
            void operator=( ref_string_state&& ) = delete;

            template< typename Input >
            void success( const Input&, reference_state& st )
            {
               assert( !st.rstack.empty() );
               assert( st.rstack.back()->is_array() );

               st.rstack.back()->emplace_back( std::move( unescaped ) );
            }

            std::string unescaped;

            const pegtl::position m_position;
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
