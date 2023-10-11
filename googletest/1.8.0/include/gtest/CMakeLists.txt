add_subdirectory(internal)

target_sources(gtest
    PRIVATE
        gtest.h
        gtest_pred_impl.h
        gtest_prod.h
        gtest-death-test.h
        gtest-message.h
        gtest-param-test.h
        gtest-printers.h
        gtest-spi.h
        gtest-test-part.h
        gtest-typed-test.h
)
