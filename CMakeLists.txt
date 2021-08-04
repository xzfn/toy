cmake_minimum_required(VERSION 3.15)

project(toy)

set(CMAKE_CXX_STANDARD 20)

# Set a suitable platform define.
if(CMAKE_SYSTEM_NAME STREQUAL Windows)
  set(VULKAN_PLATFORM_DEFINES VK_USE_PLATFORM_WIN32_KHR)
elseif(CMAKE_SYSTEM_NAME STREQUAL Linux)
  set(VULKAN_PLATFORM_DEFINES VK_USE_PLATFORM_XLIB_KHR)
elseif(CMAKE_SYSTEM_NAME STREQUAL Darwin)
  set(VULKAN_PLATFORM_DEFINES VK_USE_PLATFORM_MACOS_MVK)
endif()
set(WIN32_UNICODE_DEFINES _UNICODE UNICODE)

find_package(volk CONFIG REQUIRED)

find_path(stb_INCLUDE_DIR stb_image.h DOC "Path to stb folder" REQUIRED)

find_package(glm REQUIRED)

find_package(pybind11 REQUIRED)

# more warnings
if (MSVC)
  add_compile_options(/W4)  # /WX
  add_compile_options(/wd4201)
else()
  add_compile_options(-Wall -Wextra -pedantic)  # /-Werror
endif()

add_subdirectory(toy)
