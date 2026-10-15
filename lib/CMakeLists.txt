add_library(my_lib SHARED
    src/preprocess.cpp
    src/filter.cpp
    src/match.cpp
)
target_include_directories(my_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${OpenCV_INCLUDE_DIRS}
)
target_link_libraries(my_lib PUBLIC ${OpenCV_LIBS})
