// Copyright (c) 2018-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_ANNOTATION_HPP
#define TAO_CONFIG_ANNOTATION_HPP

#include <ostream>

#include <tao/json.hpp>
#include <tao/json/contrib/position.hpp>

#include <tao/pegtl/position.hpp>

#include "key.hpp"

namespace tao::config
{
   struct annotation
   {
      config::key key;
      json::position position;  // TODO: json::position, pegtl::position or TBD config::position?

      annotation() = default;

      annotation( annotation&& ) = default;
      annotation& operator=( annotation&& ) = default;

      annotation( const annotation& ) = default;
      annotation& operator=( const annotation& ) = default;

      void set_key( config::key k ) noexcept
      {
         key = std::move( k );
      }

      void set_position( json::position p ) noexcept
      {
         position = std::move( p );
      }

      void set_position( const pegtl::position& pos )
      {
         position.set_position( pos );
      }

      void append_message_extension( std::ostream& o ) const
      {
         o << ' ';
         to_stream( o, key );
         o << ' ';
         position.append_message_extension( o );
      }
   };

}  // namespace tao::config

#endif
