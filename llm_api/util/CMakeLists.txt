file(GLOB_RECURSE INC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB_RECURSE SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.*")

add_library(llm_api_util STATIC ${INC_FILES} ${SRC_FILES})

if (LLMAPI_ENABLE_OPENAI)
    target_link_libraries(llm_api_util PUBLIC llm_api_openai)
    target_compile_definitions(llm_api_util PRIVATE LLM_API_UTIL_WITH_OPENAI=1)
endif()
