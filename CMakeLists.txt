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
        Framework/utils.h
        Framework/glad/src/glad.c
        Framework/glad/include/glad/glad.h
        Framework/stbi/stb_image.cpp
        Framework/stbi/stb_image.h
)

# set(PLATFORM_SRC )
# if(APPLE)
# endif()

# if(WIN32)
# endif()

add_library(PBRFramework SHARED ${PROJECT_SOURCES})

target_include_directories(PBRFramework PUBLIC Framework/glad/include)

if(APPLE)
    target_link_libraries(PBRFramework PRIVATE "-framework CoreGraphics" "-framework CoreVideo" "-framework QuartzCore" 
        "-framework GameController" "-framework Metal" "-framework ImageIO" "-framework OpenGL" "-framework AppKit")
    target_compile_options(PBRFramework PRIVATE "-fobjc-arc")
    set_target_properties(PBRFramework PROPERTIES LINK_FLAGS "-fobjc-arc")
endif()

