cmake_minimum_required(VERSION 3.15)

project(toy)

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
set(stb_INCLUDE_DIR "stb_INCLUDE_DIR_NOT_FOUND" CACHE PATH "Path to stb folder")

find_package(glm REQUIRED)

find_package(pybind11 REQUIRED)


add_subdirectory(toy)
