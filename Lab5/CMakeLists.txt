set(CMAKE_CXX_STANDARD 20)
add_library(suftree INTERFACE SuffTree.hpp)

add_executable(lab5_demo demo.cpp)
target_link_libraries(suftree)

add_executable(lab5 main.cpp)
