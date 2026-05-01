cmake_minimum_required(VERSION 4.2)
project(signatures)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory("vendors")
add_executable(signatures
    main.cpp
    core/app.cpp
    core/app.h
    core/gamebuild.cpp
    core/gamebuild.h
    utils/pe.cpp
    utils/pe.h
    utils/sigmaker.cpp
    utils/sigmaker.h
)

target_link_libraries(signatures PUBLIC spdlog mem Zydis lyra )