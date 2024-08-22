target_sources(platform_common PRIVATE assert.hpp assert.cpp)
target_link_libraries(platform_common PRIVATE common)
target_include_directories(platform_common PUBLIC ${CMAKE_CURRENT_LIST_DIR})
