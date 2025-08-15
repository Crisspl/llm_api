file(GLOB_RECURSE INC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB_RECURSE DEP_FILES "${CMAKE_CURRENT_SOURCE_DIR}/deps/*.*")
file(GLOB_RECURSE SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.*")

add_library(llm_api_common STATIC ${INC_FILES} ${SRC_FILES} ${DEP_FILES})
target_include_directories(llm_api_common PUBLIC "${LLMAPI_INCL_DIR}")
if(WIN32)
    target_compile_definitions(llm_api_common PUBLIC LLM_API_PLATFORM_WINDOWS_DEF_=1)
elseif(UNIX)
    target_compile_definitions(llm_api_common PUBLIC LLM_API_PLATFORM_UNIX_DEF_=1)
endif()
if(MSVC)
    target_compile_definitions(llm_api_common PUBLIC NOMINMAX)
endif()
# cpr
target_link_libraries(llm_api_common PUBLIC cpr::cpr)
# json
target_link_libraries(llm_api_common PUBLIC nlohmann_json::nlohmann_json)
