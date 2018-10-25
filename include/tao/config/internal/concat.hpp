// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_CONCAT_HPP
#define TAO_CONFIG_INTERNAL_CONCAT_HPP

#include <vector>

#include "json.hpp"
#include "pegtl.hpp"
#include "traits.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         class entry;

         struct concat
         {
            explicit concat( const position& p )
               : p( p )
            {
            }

            position p;
            std::vector< entry > v;
            mutable bool transient = false;
         };

         template<>
         struct traits< concat >
            : public json::binding::object< TAO_JSON_BIND_REQUIRED( "position", &concat::p ),
                                            TAO_JSON_BIND_REQUIRED( "concat", &concat::v ),
                                            TAO_JSON_BIND_REQUIRED( "transient", &concat::transient ) >
         {
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
