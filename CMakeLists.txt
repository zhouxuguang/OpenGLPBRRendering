cmake_minimum_required(VERSION 3.20)

project(OpenGLPBRRendering VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
  add_compile_options(/wd4819)
  add_compile_options(/wd4828)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

set(PROJECT_SOURCES
        Framework/BoxCapture.cpp
        Framework/BoxCapture.h
        Framework/Camera.h
        Framework/Camera.cpp
        Framework/framebufferobject.cpp
        Framework/framebufferobject.h
        Framework/GameObject.cpp
        Framework/GameObject.h
        Framework/Material.cpp
        Framework/Material.h
        Framework/StaticMesh.cpp
        Framework/StaticMesh.h
        Framework/utils.h
        Framework/utils.cpp
        Framework/scene.h
        Framework/scene.cpp
        Framework/glad/src/glad.c
        Framework/glad/include/glad/glad.h
        Framework/stbi/stb_image.cpp
        Framework/stbi/stb_image.h
)

add_library(PBRFramework SHARED ${PROJECT_SOURCES})

target_include_directories(PBRFramework PUBLIC Framework/glad/include)

set(PLATFORM_SRC )
if(APPLE)
set(PLATFORM_SRC Framework/main_mac.mm)
endif()

if(WIN32)
set(PLATFORM_SRC Framework/main_win32.cpp)
endif()
add_executable(OpenGLPBRRendering ${PLATFORM_SRC})
target_link_libraries(OpenGLPBRRendering PRIVATE PBRFramework)

if(APPLE)
    target_link_libraries(OpenGLPBRRendering PRIVATE "-framework CoreGraphics" "-framework CoreVideo" "-framework QuartzCore" 
        "-framework GameController" "-framework Metal" "-framework ImageIO" "-framework OpenGL" "-framework AppKit")
    target_compile_options(OpenGLPBRRendering PRIVATE "-fobjc-arc")
    add_definitions(-DGL_SILENCE_DEPRECATION)
    set_source_files_properties(Framework/main_mac.mm PROPERTIES COMPILE_FLAGS "-fno-objc-arc")
endif()

# 复制资源
add_custom_command(TARGET OpenGLPBRRendering POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Framework/Res"
        "$<TARGET_FILE_DIR:OpenGLPBRRendering>/Res"
)

