target_compile_features(platform INTERFACE cxx_std_17)

target_include_directories(platform PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_sources(platform
    PUBLIC platform.cpp
    PUBLIC platform.hpp logger.hpp
)

target_link_libraries(platform PUBLIC core_utils)

# logger.hpp assert.cpp 
