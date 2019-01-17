// Copyright (c) 2018-2019Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_ANNOTATION_HPP
#define TAO_CONFIG_ANNOTATION_HPP

#include "external/json.hpp"

#include "key.hpp"

namespace tao
{
   namespace config
   {
      struct annotation
      {
         config::key key;
         json::position position;  // TODO: json::position, pegtl::position or TBD config::position?

         annotation() noexcept = default;

         annotation( annotation&& ) noexcept = default;
         annotation& operator=( annotation&& ) noexcept = default;

         annotation( const annotation& ) = default;
         annotation& operator=( const annotation& ) = default;

         std::size_t line() const noexcept
         {
            return position.line();
         }

         std::size_t byte_in_line() const noexcept
         {
            return position.byte_in_line();
         }

         const std::string& source() const noexcept
         {
            return position.source();
         }

         void set_key( config::key&& k ) noexcept
         {
            key = std::move( k );
         }

         void set_key( const config::key& k )
         {
            key = k;
         }

         void set_position( const json_pegtl::position& pos )
         {
            position.set_position( pos );
         }

         void append_message_extension( std::ostream& o ) const
         {
            o << ' ';
            to_stream( o, key );
            o << " (" << source() << ':' << line() << ':' << byte_in_line() << ')';
         }
      };

   }  // namespace config

}  // namespace tao

#endif
