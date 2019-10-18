#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>

int main( int argc, char* argv[] ) {
  setlocale(LC_CTYPE, "C.UTF-8");
  int result = Catch::Session().run( argc, argv );
  return result;
}