// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_CONCAT_HPP
#define TAO_CONFIG_INTERNAL_CONCAT_HPP

#include <vector>

#include "pegtl.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         class entry;

         struct concat
         {
            template< typename Input >
            explicit concat( const Input& in )
               : p( in.position() )
            {
            }

            pegtl::position p;
            std::vector< entry > v;
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
