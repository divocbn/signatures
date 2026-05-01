add_subdirectory("mem")

add_subdirectory("zydis")
add_subdirectory("lyra")

# cpr definitions, i think i should better use a different lib for that lol
set(BUILD_SHARED_LIBS OFF)
set(CURL_STATICLIB ON)
set(CPR_USE_SYSTEM_CURL OFF)
set(CPR_CURL_USE_LIBPSL OFF)
add_subdirectory("cpr")

add_subdirectory("json")

add_subdirectory("spdlog")
