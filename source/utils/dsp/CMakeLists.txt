add_library(utils_dsp)

target_compile_features(utils_dsp INTERFACE cxx_std_17)

target_include_directories(utils_dsp PUBLIC ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(common)
add_subdirectory(filter)
add_subdirectory(QFormat)
target_include_directories(utils_dsp INTERFACE ${CMAKE_CURRENT_LIST_DIR}/qformat/include)

add_subdirectory(simd)
add_subdirectory(audio_processors)
add_subdirectory(audio_pipeline)
add_subdirectory(${APP_PLATFORM})

target_link_libraries(utils_dsp PRIVATE core_utils qformat::qformat)
