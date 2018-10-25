// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_POINTER_HPP
#define TAO_CONFIG_INTERNAL_POINTER_HPP

#include <vector>

#include "token.hpp"
#include "traits.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         using pointer = std::vector< token >;

         inline pointer pop_front( const pointer& p )
         {
            assert( !p.empty() );

            return pointer( p.begin() + 1, p.end() );
         }

         inline pointer pop_back( const pointer& p )
         {
            assert( !p.empty() );

            return pointer( p.begin(), p.end() - 1 );
         }

         template<>
         struct traits< const pointer* >
         {
            template< template< typename... > class Traits >
            static void assign( json::basic_value< Traits >& v, const pointer* p )
            {
               v.unsafe_assign_opaque_ptr( p );
            }
         };

         template<>
         struct traits< pointer* >
            : public traits< const pointer* >
         {
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
