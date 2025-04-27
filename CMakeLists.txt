cmake_minimum_required(VERSION 3.22)
project(MTGEssentiaPlugins VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(COPY_PLUGIN_AFTER_BUILD "Copy plugins after build" ON)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -Wall -Wextra -Wpedantic
        -Werror
        -Wreorder -Wdeprecated-declarations
        -Wunused -Wswitch -Wreturn-type
    )
elseif(MSVC)
    add_definitions(-DNOMINMAX -D_USE_MATH_DEFINES)
endif()


set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


find_package(JUCE CONFIG REQUIRED)
set(JUCE_ENABLE_MODULE_SOURCE_GROUPS ON)


add_subdirectory(plugins/energy)

# enable_testing()
# add_subdirectory(test)