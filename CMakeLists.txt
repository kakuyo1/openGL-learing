cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 先告诉 CMake vcpkg 在哪, 在project() 之前设置
# set(CMAKE_TOOLCHAIN_FILE "B:/vcpkg/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
project(opengltutor)

#if (MSVC)  # 1. fix string literial cannot include unicode characters; 2. rid min and max macros; 3. get back M_PI, M_E macros
#    add_compile_options($<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/utf-8$<SEMICOLON>/DNOMINMAX$<SEMICOLON>/D_USE_MATH_DEFINES>)
#    add_compile_options($<$<COMPILE_LANG_AND_ID:C,MSVC>:/utf-8$<SEMICOLON>/DNOMINMAX$<SEMICOLON>/D_USE_MATH_DEFINES>)
#elseif (CMAKE_COMPILER_IS_GNUCXX)  # Add necessary checks and warnings
#    if (CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
#        add_compile_options($<$<COMPILE_LANG_AND_ID:CXX,GNU>:-Wall$<SEMICOLON>-Wextra>)
#        add_compile_options($<$<COMPILE_LANG_AND_ID:CXX,GNU>:-ftrapv$<SEMICOLON>-fbounds-check$<SEMICOLON>-fsanitize-trap>)
#    endif()
#endif()

if (MSVC)  # 1. fix string literal cannot include unicode characters; 2. rid min and max macros; 3. get back M_PI, M_E macros
    add_compile_options(/wd4005)  # 禁用宏重复定义的警告
    add_compile_options($<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/DNOMINMAX$<SEMICOLON>/D_USE_MATH_DEFINES>)
    add_compile_options($<$<COMPILE_LANG_AND_ID:C,MSVC>:/DNOMINMAX$<SEMICOLON>/D_USE_MATH_DEFINES>)
elseif (CMAKE_COMPILER_IS_GNUCXX)  # Add necessary checks and warnings
    if (CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
        add_compile_options($<$<COMPILE_LANG_AND_ID:CXX,GNU>:-Wall$<SEMICOLON>-Wextra>)
        add_compile_options($<$<COMPILE_LANG_AND_ID:CXX,GNU>:-ftrapv$<SEMICOLON>-fbounds-check$<SEMICOLON>-fsanitize-trap>)
    endif()
endif()

file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE headers CONFIGURE_DEPENDS include/*.h include/*.hpp)

# 添加可执行文件
add_executable(${PROJECT_NAME} ${sources} ${headers} "include/shader.h" "src/shader.cpp" "include/camera.h" "src/camera.cpp" "include/config.h" "include/mesh.h" "src/mesh.cpp" "include/model.h" "src/model.cpp")
target_include_directories(${PROJECT_NAME} PUBLIC include)

# 添加子模块
add_subdirectory(glm)
target_link_libraries(${PROJECT_NAME} PUBLIC glm)
add_subdirectory(glfw)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)
add_subdirectory(glad)
target_link_libraries(${PROJECT_NAME} PUBLIC glad)
add_subdirectory(assimp)
target_link_libraries(${PROJECT_NAME} PUBLIC assimp)

# 使用 vcpkg 的 fmt
# find_package(fmt CONFIG REQUIRED)
# target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)

# 添加宏定义
target_compile_definitions(${PROJECT_NAME} PUBLIC -DOPENGLTUTOR_HOME=\"${CMAKE_CURRENT_SOURCE_DIR}/\")



