add_library(orderbook_lib
    SocketHandler.cpp
    DisplayMetrics.cpp
)

target_include_directories(orderbook_lib PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include
)

target_link_libraries(orderbook_lib PUBLIC
    jwt-cpp::jwt-cpp
    nlohmann_json::nlohmann_json
)

add_executable(orderbook main.cpp)
target_link_libraries(orderbook PRIVATE orderbook_lib)
target_include_directories(orderbook_lib PRIVATE ${MATPLOTLIB_CPP_INCLUDE_DIRS})
target_include_directories(orderbook_lib PUBLIC ${Python3_INCLUDE_DIRS})
target_link_libraries(orderbook_lib PUBLIC ${Python3_LIBRARIES})
