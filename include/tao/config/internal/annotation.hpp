// Copyright (c) 2019-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_ANNOTATION_HPP
#define TAO_CONFIG_INTERNAL_ANNOTATION_HPP

#include <optional>
#include <ostream>
#include <utility>

#include "pegtl.hpp"

namespace tao::config::internal
{
   struct annotation
   {
      bool clear = false;

      std::optional< pegtl::position > position;

      annotation() = default;

      annotation( annotation&& ) = default;
      annotation( const annotation& ) = default;

      annotation& operator=( annotation&& ) = default;
      annotation& operator=( const annotation& ) = default;

      annotation( pegtl::position&& pos ) noexcept
         : position( std::move( pos ) )
      {
      }

      annotation( const pegtl::position& pos )
         : position( pos )
      {
      }

      void append_message_extension( std::ostream& o ) const
      {
         if( position ) {
            o << " (" << position->source << ':' << position->line << ':' << position->column << ')';
         }
      }
   };

}  // namespace tao::config::internal

#endif
