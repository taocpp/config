// Copyright (c) 2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_ANNOTATION_HPP
#define TAO_CONFIG_INTERNAL_ANNOTATION_HPP

#include <ostream>

#include "json.hpp"
#include "pegtl.hpp"

namespace tao
{
   namespace config
   {
      struct annotation
      {
         bool clear;
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

         void set_position( const pegtl::position& pos )
         {
            position.set_position( pos );
         }

         void append_message_extension( std::ostream& o ) const
         {
            o << " (" << source() << ':' << line() << ':' << byte_in_line() << ')';
         }
      };

   }  // namespace config

}  // namespace tao

#endif
