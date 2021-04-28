#include <stddef.h>
#include <stdexcept>

#include <tao/config.hpp>

// extern "C" int FUZZ_INIT_WITH_ARGS(int *argc, char ***argv) {
extern "C" int FUZZ_INIT()
{
   // Add global setup code here - called once before fuzzing starts

   return 0;  // Non-zero return values are reserved for future use.
}

extern "C" int FUZZ( const char* data, size_t size )
{
   // process fuzzer input (*data) and setup local objects necessary to call the function under test

   try {
      (void)tao::config::from_string( data, size, "FUZZ" );
   }
   catch( const tao::pegtl::parse_error& e ) {
   }
   catch( const std::runtime_error& ) {
   }

   // reset state and free all locally allocated resources

   return 0;  // Non-zero return values are reserved for future use.
}
