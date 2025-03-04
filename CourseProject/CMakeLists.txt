set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED COMPONENTS program_options)

add_executable(classifier 
    src/main.cpp 
    src/naive_bayes/NaiveBayes.hpp
    src/naive_bayes/NaiveBayes.cpp)
target_link_libraries(classifier Boost::program_options)

add_executable(cp_for_3 src/task_for_3.cpp)
target_link_libraries(cp_for_3 Boost::program_options)