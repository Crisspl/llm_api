#common deps
#cpr
FetchContent_Declare(cpr
                    GIT_REPOSITORY https://github.com/libcpr/cpr.git
                    GIT_TAG dd967cb48ea6bcbad9f1da5ada0db8ac0d532c06
                    CMAKE_ARGS -DBUILD_SHARED_LIBS=OFF -DCPR_USE_SYSTEM_CURL=OFF)
FetchContent_MakeAvailable(cpr)

#json
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)

option(LLMAPI_ENABLE_OPENAI "Build with OpenAI backend" ON)

add_subdirectory(common)

if (LLMAPI_ENABLE_OPENAI)
    add_subdirectory(openai)
endif()
# ... other llm api providers

add_subdirectory(util)
