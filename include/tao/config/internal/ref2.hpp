// Copyright (c) 2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_REF2_HPP
#define TAO_CONFIG_INTERNAL_REF2_HPP

#include <vector>

#include "ref2_action.hpp"
#include "ref2_part.hpp"
#include "ref2_rules.hpp"

namespace tao::config::internal
{
   struct ref2
      : public std::vector< ref2_part >
   {
      ref2() = default;

      ref2( ref2&& ) = default;
      ref2& operator=( ref2&& ) = default;

      ref2( const ref2& ) = default;
      ref2& operator=( const ref2& ) = default;

      ~ref2() = default;

      explicit ref2( const std::string& s )
      {
         assign( s );
      }

      ref2( const std::initializer_list< ref2_part >& l )
         : std::vector< ref2_part >( l )
      {}

      ref2( const std::vector< ref2_part >::const_iterator& begin, const std::vector< ref2_part >::const_iterator& end )
         : std::vector< ref2_part >( begin, end )
      {}

      ref2& operator=( const std::string& s )
      {
         clear();
         assign( s );
         return *this;
      }

      ref2& operator=( const std::initializer_list< ref2_part >& l )
      {
         vector() = l;
         return *this;
      }

      void pop_front()
      {
         assert( !empty() );
         pop_front();
      }

      void pop_back()
      {
         assert( !empty() );
         pop_back();
      }

      [[nodiscard]] std::vector< ref2_part >& vector() noexcept
      {
         return static_cast< std::vector< ref2_part >& >( *this );
      }

      [[nodiscard]] const std::vector< ref2_part >& vector() const noexcept
      {
         return static_cast< const std::vector< ref2_part >& >( *this );
      }

      void assign( const std::string& s )
      {
         using grammar = pegtl::must< rules::ref2_must, pegtl::eof >;
         pegtl::memory_input< pegtl::tracking_mode::lazy, pegtl::eol::lf_crlf, const char* > in( s, __FUNCTION__ );
         pegtl::parse< grammar, ref2_action >( in, vector(), 0 );
      }
   };

}  // namespace tao::config

#endif
