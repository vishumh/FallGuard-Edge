add_library(fallguard_shared
    src/alert_manager.cpp
    src/camera.cpp
    src/config.cpp
    src/decision_engine.cpp
    src/frame.cpp
    src/image/bounding_box.cpp
    src/image/connected_components.cpp
    src/image/image_processor.cpp
    src/image/morphology.cpp
    src/logger.cpp
    src/tinyml_engine.cpp
    src/vision_engine.cpp
)

target_include_directories(fallguard_shared
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(fallguard_shared PUBLIC cxx_std_17)

if(MSVC)
    target_compile_options(fallguard_shared PRIVATE /W4)
else()
    target_compile_options(fallguard_shared PRIVATE -Wall -Wextra -Wpedantic)
endif()
