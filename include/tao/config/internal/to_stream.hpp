// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_TO_STREAM_HPP
#define TAO_CONFIG_INTERNAL_TO_STREAM_HPP

#include <ostream>

#include "traits.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         template< typename T >
         inline void to_stream( std::ostream& os, const T& t )
         {
            json::jaxn::events::to_stream consumer( os );
            json::events::produce< traits >( consumer, t );
         }

         template< typename T >
         inline void to_stream( std::ostream& os, const T& t, const std::size_t indent )
         {
            json::jaxn::events::to_pretty_stream consumer( os, indent );
            json::events::produce< traits >( consumer, t );
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
