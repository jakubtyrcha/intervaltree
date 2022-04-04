project(IntervalTree)

Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.0.0-preview3
)
FetchContent_MakeAvailable(Catch2)

add_executable(Tests AvlTreeTests.cpp IntervalMultiTreeTests.cpp)
target_link_libraries(Tests PRIVATE Catch2::Catch2WithMain)
target_compile_features(Tests PUBLIC cxx_std_20)

add_executable(Benchmarks IntervalMultiTreeBenchmarks.cpp)
target_link_libraries(Benchmarks PRIVATE Catch2::Catch2WithMain)
target_compile_features(Benchmarks PUBLIC cxx_std_20)