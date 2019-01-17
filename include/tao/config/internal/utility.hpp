// Copyright (c) 2018-2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_UTILITY_HPP
#define TAO_CONFIG_INTERNAL_UTILITY_HPP

#include <cassert>
#include <stdexcept>

#include "../key.hpp"

#include "format.hpp"
#include "json.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         template< typename Container >
         struct reverse
         {
            explicit reverse( Container& l )
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
            explicit phase2_guard( const T& v )
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

         inline part part_from_value( const position& pos, const json::value& v )
         {
            switch( v.type() ) {
               case json::type::BOOLEAN:
                  return part( v.unsafe_get_boolean() ? part::star : part::minus );
               case json::type::STRING:
               case json::type::STRING_VIEW:
                  return part( v.as< std::string >() );
               case json::type::SIGNED:
               case json::type::UNSIGNED:
                  return part( v.as< std::size_t >() );
               default:
                  throw std::runtime_error( format( "invalid json for part -- expected string or integer (or bool)", { &pos, v.type() } ) );
            }
         }

         inline key key_from_value( const position& pos, json::value& v )
         {
            key p;

            if( v.is_string() ) {
               p.emplace_back( part( v.unsafe_get_string() ) );
               return p;
            }
            if( !v.is_array() ) {
               throw std::runtime_error( format( "invalid json for key -- expected array", { &pos, v.type() } ) );
            }
            for( const auto& t : v.unsafe_get_array() ) {
               p.emplace_back( part_from_value( pos, t ) );
            }
            v.discard();
            return p;
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
