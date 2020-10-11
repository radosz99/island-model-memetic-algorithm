cmake_minimum_required(VERSION 3.0.0)
project(imma VERSION 0.1.0)

include(CTest)
enable_testing()


set(SRC src/main.cpp src/Time.cpp src/Node.cpp src/Genetic.cpp)

add_executable(imma ${SRC})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(imma PRIVATE Threads::Threads)