add_executable(run_tests
    test_orderbook.cpp
)

target_link_libraries(run_tests
    PRIVATE
        orderbook_lib
)
