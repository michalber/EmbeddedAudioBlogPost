add_library(utils_audio_pipeline)

target_compile_features(utils_audio_pipeline INTERFACE cxx_std_17)

target_include_directories(utils_audio_pipeline PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_sources(utils_audio_pipeline
    # PUBLIC audio_pipeline.cpp 
    PUBLIC audio_pipeline.hpp
)

target_link_libraries(utils_audio_pipeline PUBLIC utils_thread core_utils utils_audio_processor)
