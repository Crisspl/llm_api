cmake_minimum_required(VERSION 3.24)

project(llm_api LANGUAGES CXX)

set(LLMAPI_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(LLMAPI_INCL_DIR "${LLMAPI_ROOT_DIR}")

add_subdirectory(llm_api)
