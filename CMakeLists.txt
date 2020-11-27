cmake_minimum_required(VERSION 3.0.0)
project(imma VERSION 0.1.0)

include(CTest)
enable_testing()


set(SRC src/main.cpp src/Time.cpp src/Node.cpp src/Genetic.cpp)
find_package(Boost COMPONENTS thread system REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
add_executable(imma ${SRC})
target_link_libraries(${PROJECT_NAME} ${framework_LIBRARIES} ${Boost_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS})


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
add_compile_options(-DBoost_USE_MULTITHREADED=OFF)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
