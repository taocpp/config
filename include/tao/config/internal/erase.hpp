// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_ERASE_HPP
#define TAO_CONFIG_INTERNAL_ERASE_HPP

#include <stdexcept>

#include "format.hpp"
#include "pointer.hpp"
#include "utility.hpp"
#include "entry.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         inline std::size_t erase( const position& pos, concat& l, const pointer& p, const token& f );

         inline std::size_t erase_name( const position& pos, concat& l, const std::string& k )
         {
            std::size_t r = 0;

            for( auto& i : l.v ) {
               if( !i.is_object() ) {
                  throw std::runtime_error( format( "attempt to index non-object with string", { &pos, { "string", k }, { "non-object", { &i.position(), i.type() } } } ) );
               }
               r += i.get_object().erase( k );
            }
            return r;
         }

         inline std::size_t erase_index( const position& pos, concat& l, std::size_t n )
         {
            for( auto& i : l.v ) {
               if( !i.is_array() ) {
                  throw std::runtime_error( format( "attempt to index non-array with integer", { &pos, { "integer", n }, { "non-object", { &i.position(), i.type() } } } ) );
               }
               auto& a = i.get_array();
               const auto s = a.size();

               if( n < s ) {
                  a.erase( a.begin() + n );
                  return 1;
               }
               n -= s;
            }
            throw std::runtime_error( format( "array has no last element to delete", { &pos, { "array", { &l.p } } } ) );
         }

         inline std::size_t erase_star( const position& pos, concat& l )
         {
            std::size_t r = 0;

            for( auto& i : l.v ) {
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
               throw std::runtime_error( format( "attempt to delete everything from non-container", { &pos, { "non-container", { &i.position(), i.type() } } } ) );
            }
            return r;
         }

         inline std::size_t erase_minus( const position& pos, concat& l )
         {
            for( auto& i : reverse( l.v ) ) {
               if( !i.is_array() ) {
                  throw std::runtime_error( format( "attempt to delete last element from non-array", { &pos, { "non-array", { &i.position(), i.type() } } } ) );
               }
               auto& a = i.get_array();

               if( !a.empty() ) {
                  a.pop_back();
                  return 1;
               }
            }
            throw std::runtime_error( format( "array has no last element to delete in", { &pos, { "array", { &l.p } } } ) );
         }

         inline std::size_t erase( const position& pos, concat& l, const token& f )
         {
            switch( f.type() ) {
               case token::name:
                  return erase_name( pos, l, f.get_name() );
               case token::index:
                  return erase_index( pos, l, f.get_index() );
               case token::star:
                  return erase_star( pos, l );
               case token::minus:
                  return erase_minus( pos, l );
            }
            assert( false );
         }

         inline std::size_t erase_name( const position& pos, concat& l, const std::string& k, const pointer& p, const token& f )
         {
            std::size_t r = 0;

            for( auto& i : reverse( l.v ) ) {
               if( !i.is_object() ) {
                  throw std::runtime_error( format( "attempt to index non-object with string", { &pos, { "string", k }, { "non-object", { &i.position(), i.type() } } } ) );
               }
               const auto j = i.get_object().find( k );

               if( j != i.get_object().end() ) {
                  r += erase( pos, j->second, p, f );
               }
            }
            return r;
         }

         inline std::size_t erase_index( const position& pos, concat& l, std::size_t n, const pointer& p, const token& f )
         {
            for( auto& i : l.v ) {
               if( !i.is_array() ) {
                  throw std::runtime_error( format( "attempt to index non-array with integer", { &pos, { "integer", n }, { "non-array", { &i.position(), i.type() } } } ) );
               }
               const auto s = i.get_array().size();

               if( n < s ) {
                  return erase( pos, i.get_array()[ n ], p, f );
               }
               n -= s;
            }
            throw std::runtime_error( format( "array index out of range", { &pos, { "integer", n }, { "array", { &l.p } } } ) );
         }

         inline std::size_t erase_star( const position& pos, concat& l, const pointer& p, const token& f )
         {
            std::size_t r = 0;

            for( auto& i : l.v ) {
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
               throw std::runtime_error( format( "attempt to delete in non-container", { &pos, { "non-container", { &i.position(), i.type() } } } ) );
            }
            return r;
         }

         inline std::size_t erase_minus( const position& pos, concat& l, const pointer& p, const token& f )
         {
            for( auto& i : reverse( l.v ) ) {
               if( !i.is_array() ) {
                  throw std::runtime_error( format( "attempt to delete last element from non-array", { &pos, { "non-array", { &i.position(), i.type() } } } ) );
               }
               if( !i.get_array().empty() ) {
                  return erase( pos, i.get_array().back(), p, f );
               }
            }
            throw std::runtime_error( format( "array has no last element to delete", { &pos, { "array", { &l.p } } } ) );
         }

         inline std::size_t erase( const position& pos, concat& l, const token& t, const pointer& p, const token& f )
         {
            switch( t.type() ) {
               case token::name:
                  return erase_name( pos, l, t.get_name(), p, f );
               case token::index:
                  return erase_index( pos, l, t.get_index(), p, f );
               case token::star:
                  return erase_star( pos, l, p, f );
               case token::minus:
                  return erase_minus( pos, l, p, f );
            }
            assert( false );
         }

         inline std::size_t erase( const position& pos, concat& l, const pointer& p, const token& f )
         {
            if( p.empty() ) {
               return erase( pos, l, f );
            }
            else {
               return erase( pos, l, p.front(), pop_front( p ), f );
            }
         }

         inline std::size_t erase( const position& pos, object_t& o, const std::string& k, const pointer& p )
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

         inline std::size_t erase( const position& pos, object_t& o, const token& t, const pointer& p )
         {
            switch( t.type() ) {
               case token::name:
                  return erase( pos, o, t.get_name(), p );
               case token::index:
                  assert( false );
               case token::star:
                  assert( false );
               case token::minus:
                  assert( false );
            }
            assert( false );
         }

         inline std::size_t erase( const position& pos, object_t& o, const pointer& p )
         {
            assert( !p.empty() );

            return erase( pos, o, p.front(), pop_front( p ) );
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
