#openai deps:
#cpp-tiktoken custom fork
FetchContent_Declare(
  cpp-tiktoken
  GIT_REPOSITORY https://github.com/Crisspl/cpp-tiktoken.git
  GIT_TAG 137e63ef91a71ac0d28f580211f6584f7befed45
)
set(CPP_TIKTOKEN_TESTING OFF CACHE BOOL "Enable testing" FORCE)
set(CPP_TIKTOKEN_INSTALL OFF CACHE BOOL "Generate the install target." FORCE)
set(CPP_TIKTOKEN_EMBED_RESOURCES ON CACHE BOOL "Compile BPEs into executable" FORCE)
FetchContent_MakeAvailable(cpp-tiktoken)

file(GLOB_RECURSE SRC_FILES "src/*.cpp")
file(GLOB_RECURSE HEADER_FILES "*.h")

add_library(llm_api_openai STATIC ${HEADER_FILES} ${SRC_FILES})

# tiktoken
target_link_libraries(tiktoken llm_api_common)
target_include_directories(tiktoken PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../..")
if(MSVC)
    target_compile_options(tiktoken PRIVATE /FI"${CMAKE_CURRENT_SOURCE_DIR}/tiktoken-integr/tiktoken_stl.h")
    target_compile_definitions(tiktoken PRIVATE NOMINMAX)
else()
    # GCC or Clang
    target_compile_options(tiktoken PRIVATE -include "${CMAKE_CURRENT_SOURCE_DIR}/tiktoken-integr/tiktoken_stl.h")
endif()
target_link_libraries(llm_api_openai PUBLIC tiktoken)

target_link_libraries(llm_api_openai PUBLIC llm_api_common)