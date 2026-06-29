add_executable(fallguard_simulator
    src/main.cpp
)

target_link_libraries(fallguard_simulator
    PRIVATE
        fallguard_shared
)

target_compile_features(fallguard_simulator PRIVATE cxx_std_17)
