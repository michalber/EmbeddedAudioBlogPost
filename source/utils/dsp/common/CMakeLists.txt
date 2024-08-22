target_include_directories(utils_dsp PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_sources(utils_dsp PUBLIC "dsp_common.hpp" "dsp_types.hpp" "dsp_self_test.hpp")