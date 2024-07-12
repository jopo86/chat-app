cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/)

add_compile_options(-Wno-volatile -Wno-delete-incomplete)

project(chat-app)

add_executable(chat-app ${SOURCE_DIR}/ui/charkeys.cpp ${SOURCE_DIR}/ui/TextButton.cpp ${SOURCE_DIR}/ui/TextBox.cpp ${SOURCE_DIR}/main.cpp)
target_include_directories(chat-app PUBLIC ${CMAKE_SOURCE_DIR}/include/)
target_link_directories(chat-app PUBLIC ${CMAKE_SOURCE_DIR}/lib/)
target_link_libraries(chat-app PUBLIC onyx glfw3 freetype zlib bz2 opengl32 garnet ws2_32)
