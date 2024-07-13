cmake_minimum_required(VERSION 3.28)
project(SecureCollaborationQ CXX)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_STANDARD_REQUIRED True)
# 设置编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# 添加源文件目录
aux_source_directory(include SOURCE_FILES)
#定义两个变量，表示头文件路径和库路径
find_package(OpenSSL REQUIRED)
# 显示的包含头文件
include_directories(include)
if(OPENSSL_FOUND)
    add_executable(SecureCollaborationQ test/main.cpp
        test/main.cpp
        include/SecureCollaborationQ.cpp
        include/SecureCollaborationQ.h
    )

    target_include_directories(${PROJECT_NAME} PUBLIC include)
    # 链接 OpenSSL 库
    target_link_libraries(${PROJECT_NAME} OpenSSL::SSL OpenSSL::Crypto)
    # 链接 pthread 库
    target_link_libraries(SecureCollaborationQ pthread)

endif (OPENSSL_FOUND)