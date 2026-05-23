# User header file paths
set(USER_Include_Dirs
    ${CMAKE_CURRENT_SOURCE_DIR}/Application/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Peripherals/inc
    $<TARGET_PROPERTY:stm32cubemx,INTERFACE_INCLUDE_DIRECTORIES>  # Auto-import CubeMX include paths
)

# Auto-collect Application sources
file(GLOB_RECURSE USER_Application_Src
    "${CMAKE_CURRENT_SOURCE_DIR}/Application/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/Application/src/*.cpp"
)

# Auto-collect Peripherals sources
file(GLOB_RECURSE USER_Peripherals_Src
    "${CMAKE_CURRENT_SOURCE_DIR}/Peripherals/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/Peripherals/src/*.cpp"
)

# Merge all user sources
set(USER_Sources
    ${USER_Application_Src}
    ${USER_Peripherals_Src}
)

# Interface library for sharing headers and defines
add_library(user_interface INTERFACE)
target_include_directories(user_interface INTERFACE
    ${USER_Include_Dirs}
)

# Object library for compiling user code
add_library(user_objects OBJECT)
target_sources(user_objects PRIVATE
    ${USER_Sources}
)

target_link_libraries(user_objects PUBLIC user_interface stm32cubemx STM32_Drivers)

# Expose variables to parent CMakeLists.txt
set(USER_Sources ${USER_Sources} PARENT_SCOPE)
set(USER_Include_Dirs ${USER_Include_Dirs} PARENT_SCOPE)
