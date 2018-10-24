// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_UTILITY_HPP
#define TAO_CONFIG_INTERNAL_UTILITY_HPP

#include <cassert>
#include <stdexcept>

#include "json.hpp"
#include "token.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         template< typename Container >
         struct reverse
         {
            explicit
            reverse( Container& l )
               : m_c( l )
            {
            }

            reverse( reverse&& ) = delete;
            reverse( const reverse& ) = delete;

            void operator=( reverse&& ) = delete;
            void operator=( const reverse& ) = delete;

            auto begin()
            {
               return m_c.rbegin();
            }

            auto end()
            {
               return m_c.rend();
            }

         private:
            Container& m_c;
         };

         template< typename T >
         reverse( T& )->reverse< T >;

         template< typename T >
         class phase2_guard
         {
         public:
            explicit
            phase2_guard( const T& v )
               : m_v( v )
            {
               m_v.set_recursion_marker();
            }

            ~phase2_guard()
            {
               m_v.clear_recursion_marker();
            }

            phase2_guard( phase2_guard&& ) = delete;
            phase2_guard( const phase2_guard& ) = delete;

            void operator=( phase2_guard&& ) = delete;
            void operator=( const phase2_guard& ) = delete;

         private:
            const T& m_v;
         };

         template< typename T >
         phase2_guard( const T& )->phase2_guard< T >;

         inline token token_from_value( const json::value& v )
         {
            switch( v.type() ) {
               case json::type::BOOLEAN:
                  return token( v.unsafe_get_boolean() ? token::STAR : token::MINUS );
               case json::type::STRING:
               case json::type::STRING_VIEW:
                  return token( v.as< std::string >() );
               case json::type::SIGNED:
               case json::type::UNSIGNED:
                  return token( v.as< std::size_t >() );
               default:
                  assert( false );
            }
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
