// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_TOKEN_HPP
#define TAO_CONFIG_INTERNAL_TOKEN_HPP

#include <cstddef>
#include <string>

namespace tao
{
   namespace config
   {
      namespace internal
      {
         struct token
         {
            enum type
            {
               NAME,
               INDEX,
               STAR,
               MINUS
            };

            explicit
            token( const type t )
               : t( t )
            {
            }

            token( const std::size_t index )
               : t( type::INDEX ),
                 i( index )
            {
            }

            token( const std::string& name )
               : t( type::NAME ),
                 k( name )
            {
            }

            type t;
            // Only one of the following is valid, but that is accepatble for
            // the moment because this is a config reader, not HPC, and it is
            // only in the internal representation, not exposed to the user.
            std::size_t i;
            std::string k;
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
