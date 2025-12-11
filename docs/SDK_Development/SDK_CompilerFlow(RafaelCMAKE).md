# Compilier Flow (Rafael CMake)
CMake is a versatile cross-platform build system generator that simplifies the management of project build processes in a compiler-independent manner. It supports a wide range of compilers and operating systems, making it an essential tool for developers who must maintain consistent build environments across different platforms. One of the key advantages of using CMake is its ability to generate native build tool files, such as Ninja or Makefiles, which can then be used to perform efficient builds. This flexibility allows developers to choose the most suitable tool for their workflow.

The IoT SDK utilizes CMake as its primary build tool, specifically tailored to support the Rafael Micro compilation environment. This setup includes a range of CMake functions and macros and a specific compilation process to efficiently build IoT SDK firmware. By leveraging CMake, developers can easily create makefiles and project files, ensuring a seamless and consistent compilation experience across multiple platforms. This enhances the portability and robustness of their applications, making it easier to manage complex build configurations and dependencies.

Additionally, IoT SDK provides recommended examples and components that serve as templates, enabling developers to quickly get started with their projects. These examples and components are designed to showcase best practices and typical use cases, helping developers understand and utilize the full capabilities of the SDK. With CMake's robust build management and tailored support for Rafael Micro's environment, developers can streamline their development process, reduce setup times, and focus more on creating innovative IoT solutions.

This guide is divided into two main sections: instructions on creating an application example and detailed steps for developing components within the IoT SDK.

---
# SDK CMake API
This document provides an overview of the custom CMake functions and macros used in the Rafael IoT SDK for managing component tagging, library creation, source file addition, and various other build-related tasks

## Project basic APIs
The Project Basic category contains essential functions and macros for setting up and managing projects. These include generating component tags, creating libraries, and defining main source files.

### sdk_generate_component_tag
Generates a component tag for the current directory or a specified component name.

example: 
```CMake
sdk_generate_component_tag()
sdk_generate_component_tag($<component_name>)
```

### sdk_generate_library
Generates a static library for the current directory or a specified library name.

example: 
```CMake
sdk_generate_library()
sdk_generate_library($<library_name>)
```

### sdk_set_main_file
Sets the main file for the project.

example: 
```CMake
sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/main.c)
```

### setup_project
Sets up a project with specified configurations and links it with the SDK interface library.

example: 
```CMake
setup_project($<project_name>)
```

### sdk_use_app_lib
Adds and links the application library to the SDK interface library.

example: 
```CMake
sdk_use_app_lib()
```

### Public APIs
The Public category includes functions for adding sources, including directories, and compiling definitions that are accessible across the SDK. These functions help manage public-facing components and dependencies.

### sdk_ifndef
Sets a default value for a variable if it is not already defined.

example: 
```CMake
sdk_ifndef(CONFIG_SAMPLE false)
```

### sdk_add_compile_definitions
Adds compile definitions to the SDK interface library.

example: 
```CMake
sdk_add_compile_definitions(-DSIMPLE=1)
```

### sdk_add_compile_definitions_ifdef
Adds compile definitions to the SDK interface library if a specified feature is enabled.

example: 
```CMake
sdk_add_compile_definitions(CONFIG_SAMPLE -DSIMPLE=1)
```

### sdk_library_add_sources
Adds source files to the current static library.

example: 
```CMake
sdk_library_add_sources(src/main.c)
```

### sdk_library_add_sources_ifdef
Adds source files to the current static library if a specified feature is enabled

example: 
```CMake
sdk_library_add_sources_ifdef(CONFIG_SAMPLE src/main.c)
```

### sdk_add_include_directories
Adds include directories to the SDK interface library.

example: 
```CMake
sdk_add_include_directories(inc)
```

### sdk_add_system_include_directories
Adds system include directories to the SDK interface library.

example: 
```CMake
sdk_add_system_include_directories(inc)
```

### sdk_add_include_directories_ifdef
Adds include directories to the SDK interface library if a specified feature is enabled.

example: 
```CMake
sdk_add_include_directories_ifdef(CONFIG_SAMPLE inc)
```

## Private APIs 
The Private category features functions for handling internal components, such as private including directories and compile definitions. These functions ensure that internal project details are kept encapsulated.

### sdk_add_private_include_directories
Adds private include directories to the current static library.

example: 
```CMake
sdk_add_private_include_directories(inc)
```

### sdk_add_private_include_directories_ifdef
Adds private include directories to the current static library if a specified feature is enabled.

example: 
```CMake
sdk_add_private_include_directories_ifdef(CONFIG_SAMPLE inc)
```

### sdk_add_private_compile_definitions
Adds private compile definitions to the current static library.

example: 
```CMake
sdk_add_private_compile_definitions(-DSIMPLE=1)
```

### sdk_add_private_compile_definitions_ifdef
Adds private compile definitions to the current static library if a specified feature is enabled.

example: 
```CMake
sdk_add_private_compile_definitions_ifdef(CONFIG_SAMPLE -DSIMPLE=1)
```

---
# Application Example Guide

- Example Project

example: examples/helloworld/CMakeLists.txt
```CMake
sdk_add_include_directories(${CMAKE_CURRENT_LIST_DIR}/helloworld/Include)

sdk_use_app_lib()
target_sources(app PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/helloworld/app.c
)

sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/helloworld/main.c)
setup_project(helloworld)
```


---
# Components Guide

- Middle CMakefile

example: components/CMakeLists.txt
```CMake
message(STATUS "Apply Component List:")
sdk_add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libc)
sdk_add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/utility)
sdk_add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/network)
sdk_add_subdirectory_ifdef(CONFIG_BUILD_COMPONENT_CPC ${CMAKE_CURRENT_LIST_DIR}/cpc)
sdk_add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/platform)
```

example: components/utility/CMakeLists.txt
```CMake
sdk_generate_component_tag()
sdk_add_subdirectory_ifdef(CONFIG_BUILD_COMPONENT_UTILITY ${CMAKE_CURRENT_LIST_DIR}/utility)
sdk_add_subdirectory_ifdef(CONFIG_BUILD_COMPONENT_LOG ${CMAKE_CURRENT_LIST_DIR}/log)
sdk_add_subdirectory_ifdef(CONFIG_BUILD_COMPONENT_UART_STDIO ${CMAKE_CURRENT_LIST_DIR}/uart_stdio)
sdk_add_subdirectory_ifdef(CONFIG_BUILD_COMPONENT_CLI ${CMAKE_CURRENT_LIST_DIR}/cli)
sdk_add_subdirectory_ifdef(CONFIG_BUILD_COMPONENT_ENHANCED_FLASH_DATASET ${CMAKE_CURRENT_LIST_DIR}/EnhancedFlashDataset)
```

- Library CMakefile

example: components/utility/log/CMakeLists.txt
```CMake
sdk_generate_library(log)
sdk_add_compile_options(-DLOG_LEVEL=${CONFIG_LOG_LEVEL})
sdk_add_include_directories(.)
sdk_library_add_sources(log.c)
```

example: components/utility/uart_stdio/CMakeLists.txt
```CMake
sdk_generate_library(uart_stdio)
sdk_add_include_directories(Inc)
sdk_library_add_sources(uart_stdio.c)
```

- Release static Library CMakefile

example: components/utility/log/CMakeLists.txt.in
```
sdk_add_static_library(log)
sdk_add_compile_options( -DLOG_LEVEL=${CONFIG_LOG_LEVEL} )
sdk_add_include_directories( . )
```

example: components/utility/uart_stdio/CMakeLists.txt.in
```CMake
sdk_add_static_library(uart_stdio)
sdk_add_include_directories(Inc)
```
