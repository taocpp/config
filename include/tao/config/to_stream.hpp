// Copyright (c) 2018-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_TO_STREAM_HPP
#define TAO_CONFIG_TO_STREAM_HPP

#include <ostream>

#include <tao/json/jaxn/events/to_pretty_stream.hpp>
#include <tao/json/jaxn/events/to_stream.hpp>

#include "internal/events_from_value.hpp"

#include "value.hpp"

namespace tao::config
{
   inline void to_stream( std::ostream& os, const value& v )
   {
      json::jaxn::events::to_stream consumer( os );
      internal::events_from_value( consumer, v );
   }

   inline void to_stream( std::ostream& os, const value& v, const std::size_t indent )
   {
      json::jaxn::events::to_pretty_stream consumer( os, indent );
      internal::events_from_value( consumer, v );
   }

}  // namespace tao::config

#endif
