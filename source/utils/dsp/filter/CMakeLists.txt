target_include_directories(utils_dsp PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_sources(utils_dsp PRIVATE "fir_filter.cpp" "biquad_filter.cpp" PUBLIC "fir_filter.hpp" "biquad_filter.hpp")