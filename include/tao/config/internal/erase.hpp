// Copyright (c) 2018-2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_ERASE_HPP
#define TAO_CONFIG_INTERNAL_ERASE_HPP

#include <iterator>
#include <stdexcept>

#include "../key.hpp"

#include "entry.hpp"
#include "format.hpp"
#include "utility.hpp"

namespace tao::config::internal
{
   inline std::size_t erase( const position& pos, concat& l, const key& p, const part& f );

   inline std::size_t erase_name( const position& pos, concat& l, const std::string& k )
   {
      std::size_t r = 0;

      for( auto& i : l.private_entries() ) {
         if( !i.is_object() ) {
            throw pegtl::parse_error( format( __FILE__, __LINE__, "attempt to index non-object with string", { { "string", k }, { "non-object", { &i.position(), i.type() } } } ), pos );
         }
         r += i.get_object().erase( k );
      }
      return r;
   }

   inline std::size_t erase_index( const position& pos, concat& l, std::size_t n )
   {
      for( auto& i : l.private_entries() ) {
         if( !i.is_array() ) {
            throw pegtl::parse_error( format( __FILE__, __LINE__, "attempt to index non-array with integer", { { "integer", n }, { "non-array", { &i.position(), i.type() } } } ), pos );
         }
         auto& a = i.get_array();
         const auto s = a.size();

         if( n < s ) {
            auto j = a.begin();
            std::advance( j, n );
            a.erase( j );
            return 1;
         }
         n -= s;
      }
      throw pegtl::parse_error( format( __FILE__, __LINE__, "array index out of range", { { "integer", n }, { "array", { &l.p } } } ), pos );
   }

   inline std::size_t erase_star( const position& pos, concat& l )
   {
      std::size_t r = 0;

      for( auto& i : l.private_entries() ) {
         if( i.is_array() ) {
            r += i.get_array().size();
            i.get_array().clear();
            continue;
         }
         if( i.is_object() ) {
            r += i.get_object().size();
            i.get_object().clear();
            continue;
         }
         throw pegtl::parse_error( format( __FILE__, __LINE__, "attempt to delete everything from non-container", { { "non-container", { &i.position(), i.type() } } } ), pos );
      }
      return r;
   }

   inline std::size_t erase_minus( const position& pos, concat& l )
   {
      for( auto& i : reverse( l.private_entries() ) ) {
         if( !i.is_array() ) {
            throw pegtl::parse_error( format( __FILE__, __LINE__, "attempt to delete last element from non-array", { { "non-array", { &i.position(), i.type() } } } ), pos );
         }
         auto& a = i.get_array();

         if( !a.empty() ) {
            a.pop_back();
            return 1;
         }
      }
      throw pegtl::parse_error( format( __FILE__, __LINE__, "array has no last element to delete in", { { "array", { &l.p } } } ), pos );
   }

   inline std::size_t erase( const position& pos, concat& l, const part& f )
   {
      switch( f.type() ) {
         case part::name:
            return erase_name( pos, l, f.get_name() );
         case part::index:
            return erase_index( pos, l, f.get_index() );
         case part::star:
            return erase_star( pos, l );
         case part::minus:
            return erase_minus( pos, l );
      }
      assert( false );
   }

   inline std::size_t erase_name( const position& pos, concat& l, const std::string& k, const key& p, const part& f )
   {
      std::size_t r = 0;

      for( auto& i : reverse( l.private_entries() ) ) {
         if( !i.is_object() ) {
            // TODO: Should references be silently ignored (continue)?
            throw pegtl::parse_error( format( __FILE__, __LINE__, "attempt to index non-object with string", { { "string", k }, { "non-object", { &i.position(), i.type() } } } ), pos );
         }
         const auto j = i.get_object().find( k );

         if( j != i.get_object().end() ) {
            r += erase( pos, j->second, p, f );
         }
      }
      return r;
   }

   inline std::size_t erase_index( const position& pos, concat& l, std::size_t n, const key& p, const part& f )
   {
      for( auto& i : l.private_entries() ) {
         if( !i.is_array() ) {
            throw pegtl::parse_error( format( __FILE__, __LINE__, "attempt to index non-array with integer", { { "integer", n }, { "non-array", { &i.position(), i.type() } } } ), pos );
         }
         const auto s = i.get_array().size();

         if( n < s ) {
            auto j = i.get_array().begin();
            std::advance( j, n );
            return erase( pos, *j, p, f );
         }
         n -= s;
      }
      throw pegtl::parse_error( format( __FILE__, __LINE__, "array index out of range", { { "integer", n }, { "array", { &l.p } } } ), pos );
   }

   inline std::size_t erase_star( const position& pos, concat& l, const key& p, const part& f )
   {
      std::size_t r = 0;

      for( auto& i : l.private_entries() ) {
         if( i.is_array() ) {
            for( auto& j : i.get_array() ) {
               r += erase( pos, j, p, f );
            }
            continue;
         }
         if( i.is_object() ) {
            for( auto& j : i.get_object() ) {
               r += erase( pos, j.second, p, f );
            }
            continue;
         }
         throw pegtl::parse_error( format( __FILE__, __LINE__, "attempt to delete in non-container", { { "non-container", { &i.position(), i.type() } } } ), pos );
      }
      return r;
   }

   inline std::size_t erase_minus( const position& pos, concat& l, const key& p, const part& f )
   {
      for( auto& i : reverse( l.private_entries() ) ) {
         if( !i.is_array() ) {
            throw pegtl::parse_error( format( __FILE__, __LINE__, "attempt to delete last element from non-array", { { "non-array", { &i.position(), i.type() } } } ), pos );
         }
         if( !i.get_array().empty() ) {
            return erase( pos, i.get_array().back(), p, f );
         }
      }
      throw pegtl::parse_error( format( __FILE__, __LINE__, "array has no last element to delete", { { "array", { &l.p } } } ), pos );
   }

   inline std::size_t erase( const position& pos, concat& l, const part& t, const key& p, const part& f )
   {
      switch( t.type() ) {
         case part::name:
            return erase_name( pos, l, t.get_name(), p, f );
         case part::index:
            return erase_index( pos, l, t.get_index(), p, f );
         case part::star:
            return erase_star( pos, l, p, f );
         case part::minus:
            return erase_minus( pos, l, p, f );
      }
      assert( false );
   }

   inline std::size_t erase( const position& pos, concat& l, const key& p, const part& f )
   {
      if( p.empty() ) {
         return erase( pos, l, f );
      }
      else {
         return erase( pos, l, p.front(), pop_front( p ), f );
      }
   }

   inline std::size_t erase( const position& pos, object_t& o, const std::string& k, const key& p )
   {
      if( p.empty() ) {
         return o.erase( k );
      }
      const auto i = o.find( k );

      if( i != o.end() ) {
         return erase( pos, i->second, pop_back( p ), p.back() );
      }
      return 0;
   }

   inline std::size_t erase( const position& pos, entry& e, const part& t, const key& p )
   {
      switch( t.type() ) {
         case part::name:
            return erase( pos, e.get_object(), t.get_name(), p );
         case part::index:
            assert( false );
         case part::star:
            assert( false );
         case part::minus:
            assert( false );
      }
      assert( false );
   }

   inline std::size_t erase( const position& pos, entry& e, const key& p )
   {
      assert( !p.empty() );

      return erase( pos, e, p.front(), pop_front( p ) );
   }

}  // namespace tao::config::internal

#endif
