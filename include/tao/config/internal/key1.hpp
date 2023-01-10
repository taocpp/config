// Copyright (c) 2020-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_KEY1_HPP
#define TAO_CONFIG_INTERNAL_KEY1_HPP

#include <vector>

#include "key1_action.hpp"
#include "key1_grammar.hpp"
#include "key1_part.hpp"
#include "pegtl.hpp"

namespace tao::config::internal
{
   struct key1
      : std::vector< key1_part >
   {
      key1() = default;

      key1( key1&& ) = default;
      key1& operator=( key1&& ) = default;

      ~key1() = default;

      key1( const key1& ) = default;
      key1& operator=( const key1& ) = default;

      explicit key1( const std::string& s )
      {
         assign( s );
      }

      key1( const std::initializer_list< key1_part >& l )
         : std::vector< key1_part >( l )
      {}

      key1( const std::vector< key1_part >::const_iterator& begin, const std::vector< key1_part >::const_iterator& end )
         : std::vector< key1_part >( begin, end )
      {}

      key1& operator=( const std::string& s )
      {
         clear();
         assign( s );
         return *this;
      }

      key1& operator=( const std::initializer_list< key1_part >& l )
      {
         vector() = l;
         return *this;
      }

      [[nodiscard]] std::vector< key1_part >& vector() noexcept
      {
         return static_cast< std::vector< key1_part >& >( *this );
      }

      [[nodiscard]] const std::vector< key1_part >& vector() const noexcept
      {
         return static_cast< const std::vector< key1_part >& >( *this );
      }

      void assign( const std::string& s )
      {
         using grammar = pegtl::must< rules::key1_rule, pegtl::eof >;
         pegtl::memory_input< pegtl::tracking_mode::lazy, pegtl::eol::lf_crlf, const char* > in( s, __FUNCTION__ );
         pegtl::parse< grammar, key1_action >( in, vector() );
      }
   };

   inline key1 pop_front( const key1& p )
   {
      assert( !p.empty() );

      return key1( p.begin() + 1, p.end() );
   }

   inline key1 pop_back( const key1& p )
   {
      assert( !p.empty() );

      return key1( p.begin(), p.end() - 1 );
   }

   inline key1& operator+=( key1& l, const key1& r )
   {
      l.insert( l.end(), r.begin(), r.end() );
      return l;
   }

   inline key1& operator+=( key1& l, const key1_part& p )
   {
      l.emplace_back( p );
      return l;
   }

   [[nodiscard]] inline key1 operator+( const key1& l, const key1& r )
   {
      key1 t( l );
      t += r;
      return t;
   }

   [[nodiscard]] inline key1 operator+( const key1& l, const key1_part& r )
   {
      key1 t( l );
      t += r;
      return t;
   }

}  // namespace tao::config::internal

#endif
