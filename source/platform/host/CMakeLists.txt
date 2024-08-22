target_include_directories(platform INTERFACE ${CMAKE_CURRENT_LIST_DIR})

target_sources(platform PRIVATE host_platform.cpp PUBLIC host_platform.hpp)
