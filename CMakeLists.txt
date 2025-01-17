cmake_minimum_required(VERSION 3.14)

set(CMAKE_TOOLCHAIN_FILE "/home/jdzialo/Documents/gitlib/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "Vcpkg toolchain file")
set(VCPKG_TARGET_TRIPLET "x64-linux" CACHE STRING "Vcpkg target triplet")

project(orderbook VERSION 1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
find_package(Threads REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(jwt-cpp CONFIG REQUIRED)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
)

# Add subdirectories
add_subdirectory(src)
add_subdirectory(tests)
