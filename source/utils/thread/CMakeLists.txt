add_library(utils_thread)

target_compile_features(utils_thread INTERFACE cxx_std_17)

target_include_directories(utils_thread PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_sources(utils_thread
    PUBLIC thread.cpp 
    PUBLIC thread.hpp
)

target_link_libraries(utils_thread PUBLIC osal)
