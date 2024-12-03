set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED COMPONENTS program_options)

add_library(naive_bayes
    src/naive_bayes/NaiveBayes.hpp
    src/naive_bayes/NaiveBayes.cpp)

add_executable(classifier src/main.cpp)
target_link_libraries(classifier naive_bayes Boost::program_options)

add_executable(cp_for_3 src/task_for_3.cpp)