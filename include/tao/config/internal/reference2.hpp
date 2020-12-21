// Copyright (c) 2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_REFERENCE2_HPP
#define TAO_CONFIG_INTERNAL_REFERENCE2_HPP

#include <vector>

#include "reference2_action.hpp"
#include "reference2_part.hpp"
#include "reference2_rules.hpp"

namespace tao::config::internal
{
   struct reference2
      : std::vector< reference2_part >
   {
      reference2() = default;

      reference2( reference2&& ) = default;
      reference2& operator=( reference2&& ) = default;

      ~reference2() = default;

      reference2( const reference2& ) = default;
      reference2& operator=( const reference2& ) = default;

      explicit reference2( const std::string& s )
      {
         assign( s );
      }

      reference2( const std::initializer_list< reference2_part >& l )
         : std::vector< reference2_part >( l )
      {}

      reference2( const std::vector< reference2_part >::const_iterator& begin, const std::vector< reference2_part >::const_iterator& end )
         : std::vector< reference2_part >( begin, end )
      {}

      reference2& operator=( const std::string& s )
      {
         clear();
         assign( s );
         return *this;
      }

      reference2& operator=( const std::initializer_list< reference2_part >& l )
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

      [[nodiscard]] std::vector< reference2_part >& vector() noexcept
      {
         return static_cast< std::vector< reference2_part >& >( *this );
      }

      [[nodiscard]] const std::vector< reference2_part >& vector() const noexcept
      {
         return static_cast< const std::vector< reference2_part >& >( *this );
      }

      void assign( const std::string& s )
      {
         using grammar = pegtl::must< rules::reference2_rule, pegtl::eof >;
         pegtl::memory_input< pegtl::tracking_mode::lazy, pegtl::eol::lf_crlf, const char* > in( s, __FUNCTION__ );
         pegtl::parse< grammar, reference2_action >( in, vector() );
      }
   };

}  // namespace tao::config::internal

#endif
