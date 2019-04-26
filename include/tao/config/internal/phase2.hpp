// Copyright (c) 2018-2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE2_HPP
#define TAO_CONFIG_INTERNAL_PHASE2_HPP

#include <iterator>

#include "access.hpp"
#include "json.hpp"
#include "phase2_add.hpp"
#include "phase2_guard.hpp"
#include "phase2_repackage.hpp"
#include "state.hpp"
#include "utility.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         class phase2_impl
         {
         public:
            template< template< typename... > class Traits >
            json::basic_value< Traits > phase2( const entry& root ) const
            {
               return process_object< Traits >( root.get_object() );
            }

         private:
            template< template< typename... > class Traits >
            json::basic_value< Traits > process_entry( const entry& e ) const
            {
               json::basic_value< phase2_traits > r;

               const phase2_guard p2g( e );

               switch( e.type() ) {
                  case entry::atom:
                     r = process_atom< Traits >( e.get_atom() );
                     break;
                  case entry::array:
                     r = process_array< Traits >( e.get_array() );
                     break;
                  case entry::object:
                     r = process_object< Traits >( e.get_object() );
                     break;
                  case entry::nothing:
                     assert( false );
                  case entry::reference:
                     r = process_reference< Traits >( e.position(), e.parent()->parent(), e.get_reference() );
                     break;
                  default:
                     assert( false );
               }
               r.clear = e.clear();
               r.set_position( e.position() );
               return r;
            }

            template< template< typename... > class Traits >
            json::basic_value< Traits > process_list( const concat& l ) const
            {
               assert( !l.entries().empty() );

               json::basic_value< Traits > r = process_entry< Traits >( l.entries().front() );

               for( auto i = std::next( l.entries().begin() ); i != l.entries().end(); ++i ) {
                  phase2_add( r, process_entry< Traits >( *i ), i->position() );
               }
               r.set_position( l.p );
               return r;
            }

            template< template< typename... > class Traits >
            json::basic_value< Traits > process_atom( const atom_t& v ) const
            {
               json::events::to_basic_value< Traits > consumer;
               json::events::from_value( consumer, v );
               return std::move( consumer.value );
            }

            template< template< typename... > class Traits >
            json::basic_value< Traits > process_array( const array_t& a ) const
            {
               json::basic_value< Traits > t( json::empty_array );

               for( const auto& i : a ) {
                  if( !i.is_temporary() ) {
                     t.emplace_back( process_list< Traits >( i ) );
                  }
               }
               return t;
            }

            template< template< typename... > class Traits >
            json::basic_value< Traits > process_object( const object_t& o ) const
            {
               json::basic_value< Traits > t( json::empty_object );

               for( const auto& i : o ) {
                  if( !i.second.is_temporary() ) {
                     t.emplace( i.first, process_list< Traits >( i.second ) );
                  }
               }
               return t;
            }

            const concat& process_reference_impl( const position& pos, const entry& e, const reference_t& r ) const
            {
               config::key k;

               for( auto& i : r.get_array() ) {
                  if( i.is_array() ) {
                     k.emplace_back( part_from_value( pos, process_list< phase2_traits >( process_reference_impl( pos, e, i ) ) ) );
                  }
                  else {
                     k.emplace_back( part_from_value( pos, i ) );
                  }
               }
               return access( pos, e, k );
            }

            template< template< typename... > class Traits >
            json::basic_value< Traits > process_reference( const position& pos, const entry& e, const reference_t& r ) const
            {
               return process_list< Traits >( process_reference_impl( pos, e, r ) );
            }
         };

         template< template< typename... > class Traits >
         json::basic_value< Traits > phase2( state& st )
         {
            assert( st.astack.empty() );
            assert( st.lstack.empty() );
            assert( st.rstack.empty() );
            assert( st.ostack.size() == 1 );

            return phase2_repackage< Traits >( phase2_impl().phase2< phase2_traits >( st.root ) );  // TODO: Eliminate phase2_impl instance if it remains stateless.
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
