cmake_minimum_required(VERSION 3.10)
project(BoatRenderer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(libs/glfw)

# GLAD
add_library(glad libs/glad/src/glad.c)
target_include_directories(glad PUBLIC libs/glad/include)

# GLM (header-only)
include_directories(libs/glm)

# Incluir diretórios
include_directories(src)
include_directories(libs/glfw/include)

# Ficheiros fonte
set(SOURCES
    src/main.cpp
)

# Executável
add_executable(BoatRenderer ${SOURCES})

# Link de bibliotecas
target_link_libraries(BoatRenderer glfw glad)

# Copiar shaders e modelos para a pasta build
add_custom_command(TARGET BoatRenderer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:BoatRenderer>/shaders
    COMMENT "Copying shaders...")

add_custom_command(TARGET BoatRenderer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/models $<TARGET_FILE_DIR:BoatRenderer>/models
    COMMENT "Copying models...")