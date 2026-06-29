add_executable(fallguard_foundation_tests
    foundation_tests.cpp
)

target_link_libraries(fallguard_foundation_tests
    PRIVATE
        fallguard_shared
)

target_compile_features(fallguard_foundation_tests PRIVATE cxx_std_17)

add_test(NAME fallguard_foundation_tests COMMAND fallguard_foundation_tests)
