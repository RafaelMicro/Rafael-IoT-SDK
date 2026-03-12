
macro(sdk_generate_component_tag)
  if(${ARGC})
    set(component_name ${ARGV0})
  else()
    get_filename_component(component_name ${CMAKE_CURRENT_LIST_DIR} NAME)
  endif()
  message(STATUS "- ${component_name}")
endmacro()

macro(sdk_generate_library)
  if(${ARGC})
    set(library_name ${ARGV0})
  else()
    get_filename_component(library_name ${CMAKE_CURRENT_LIST_DIR} NAME)
  endif()
  message(STATUS "\t+ ${library_name}")
  set(CURRENT_STATIC_LIBRARY ${library_name})
  add_library(${library_name} STATIC)
  set_property(GLOBAL APPEND PROPERTY SDK_LIBS ${library_name})
  target_link_libraries(${library_name} PUBLIC sdk_intf_lib)
endmacro()

function(sdk_library_add_sources)
  foreach(arg ${ARGV})
    if(IS_DIRECTORY ${arg})
      message(FATAL_ERROR "sdk_library_add_sources() was called on a directory")
    endif()

    if(IS_ABSOLUTE ${arg})
      set(path ${arg})
    else()
      set(path ${CMAKE_CURRENT_SOURCE_DIR}/${arg})
    endif()

    target_sources(${CURRENT_STATIC_LIBRARY} PRIVATE ${path})
  endforeach()
endfunction()

function(sdk_library_add_sources_ifdef feature)
  if(${${feature}})
    sdk_library_add_sources(${ARGN})
  endif()
endfunction()

function(sdk_add_include_directories)
  foreach(arg ${ARGV})
    if(IS_ABSOLUTE ${arg})
      set(path ${arg})
    else()
      set(path ${CMAKE_CURRENT_SOURCE_DIR}/${arg})
    endif()

    target_include_directories(sdk_intf_lib INTERFACE ${path})
  endforeach()
endfunction()

function(sdk_add_private_include_directories)
  foreach(arg ${ARGV})
    if(IS_ABSOLUTE ${arg})
      set(path ${arg})
    else()
      set(path ${CMAKE_CURRENT_SOURCE_DIR}/${arg})
    endif()

    target_include_directories(${CURRENT_STATIC_LIBRARY} PRIVATE ${path})
  endforeach()
endfunction()

function(sdk_use_app_lib)
	add_library(app STATIC)
	target_link_libraries(app sdk_intf_lib)
endfunction()

function(sdk_add_system_include_directories)
  foreach(arg ${ARGV})
    if(IS_ABSOLUTE ${arg})
      set(path ${arg})
    else()
      set(path ${CMAKE_CURRENT_SOURCE_DIR}/${arg})
    endif()

    target_include_directories(sdk_intf_lib SYSTEM INTERFACE ${path})
  endforeach()
endfunction()

function(sdk_add_include_directories_ifdef feature)
  if(${${feature}})
    sdk_add_include_directories(${ARGN})
  endif()
endfunction()

function(sdk_add_private_include_directories_ifdef feature)
  if(${${feature}})
    sdk_add_private_include_directories(${ARGN})
  endif()
endfunction()

function(sdk_add_system_include_directories_ifdef feature)
  if(${${feature}})
    sdk_add_system_include_directories(${ARGN})
  endif()
endfunction()

function(sdk_add_compile_definitions)
  target_compile_definitions(sdk_intf_lib INTERFACE ${ARGV})
endfunction()

function(sdk_add_private_compile_definitions)
  target_compile_definitions(${CURRENT_STATIC_LIBRARY} PRIVATE ${ARGV})
endfunction()

function(sdk_add_compile_definitions_ifdef feature)
  if(${${feature}})
    sdk_add_compile_definitions(${ARGN})
  endif()
endfunction()

function(sdk_add_private_compile_definitions_ifdef feature)
  if(${${feature}})
    sdk_add_private_compile_definitions(${ARGN})
  endif()
endfunction()

function(sdk_add_compile_options)
  target_compile_options(sdk_intf_lib INTERFACE ${ARGV})
endfunction()

function(sdk_add_private_compile_options)
  target_compile_options(${CURRENT_STATIC_LIBRARY} PRIVATE ${ARGV})
endfunction()

function(sdk_add_compile_options_ifdef feature)
  if(${${feature}})
    sdk_add_compile_options(${ARGN})
  endif()
endfunction()

function(sdk_add_private_compile_options_ifdef feature)
  if(${${feature}})
    sdk_add_private_compile_options(${ARGN})
  endif()
endfunction()

function(sdk_add_link_options)
  target_link_options(sdk_intf_lib INTERFACE ${ARGV})
endfunction()

function(sdk_add_private_link_options)
  target_link_options(${CURRENT_STATIC_LIBRARY} PRIVATE ${ARGV})
endfunction()

function(sdk_add_link_options_ifdef feature)
  if(${${feature}})
    sdk_add_link_options(${ARGN})
  endif()
endfunction()

function(sdk_add_private_link_options_ifdef feature)
  if(${${feature}})
    sdk_add_private_link_options(${ARGN})
  endif()
endfunction()

function(sdk_add_link_libraries)
  target_link_libraries(sdk_intf_lib INTERFACE ${ARGV})
endfunction()

function(sdk_add_link_libraries_ifdef feature)
  if(${${feature}})
    sdk_add_link_libraries(${ARGN})
  endif()
endfunction()

function(sdk_add_subdirectory_ifdef feature dir)
  if(${${feature}})
    add_subdirectory(${dir})
  endif()
endfunction()

function(sdk_add_subdirectory dir)
  add_subdirectory(${dir})
endfunction()

function(sdk_add_static_library)
  foreach(arg ${ARGV})
    if(IS_DIRECTORY ${arg})
      message(FATAL_ERROR "sdk_add_static_library() was called on a directory")
    endif()

    if(IS_ABSOLUTE ${arg})
      set(path ${arg})
    else()
      set(path ${CMAKE_CURRENT_SOURCE_DIR}/${arg})
    endif()

    get_filename_component(library_name ${path} NAME_WE)
    message(STATUS "\t+ ${library_name}")
    add_library(${library_name} STATIC IMPORTED)
    set(CURRENT_STATIC_LIBRARY ${library_name})
    set_property(GLOBAL APPEND PROPERTY SDK_STATIC_LIBS ${CMAKE_CURRENT_LIST_DIR}/lib${library_name}.a)
  endforeach()
endfunction()

function(sdk_add_static_library_ifdef feature)
  if(${${feature}})
    sdk_add_static_library(${ARGN})
  endif()
endfunction()

function(sdk_value_ifdef define)
  if(NOT DEFINED ${define})
      message( FATAL_ERROR "The ${define} is the required value, please use -D${define} in compiler option. FATAL !!!" )
  endif()
endfunction()

macro(sdk_ifndef define val)
  if(NOT DEFINED ${define})
    set(${define} ${val})
  endif()
endmacro()

function(sdk_set_linker_script ld)
  if(IS_ABSOLUTE ${ld})
    set(path ${ld})
  else()
    set(path ${CMAKE_CURRENT_SOURCE_DIR}/${ld})
  endif()

  set_property(GLOBAL PROPERTY LINKER_SCRIPT ${path})
endfunction()

macro(sdk_set_vscode_dir dir)
  if(IS_ABSOLUTE ${dir})
    set(VSCODE_DIR ${dir})
  else()
    set(VSCODE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${dir})
  endif()
endmacro()

macro(sdk_set_main_file)
  if(IS_ABSOLUTE ${ARGV0})
    set(path ${ARGV0})
  else()
    set(path ${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0})
  endif()
  set(CURRENT_MAIN_FILE ${path})
endmacro()

function(get_all_source_files result)
    file(GLOB_RECURSE all_source_files
        "${CMAKE_SOURCE_DIR}/*.c"
        "${CMAKE_SOURCE_DIR}/*.cpp"
        "${CMAKE_SOURCE_DIR}/*.h"
        "${CMAKE_SOURCE_DIR}/*.hpp"
    )
    set(${result} ${all_source_files} PARENT_SCOPE)
endfunction()

macro(setup_project name)
  # get_all_source_files(SOURCE_FILES)
  # foreach(file ${SOURCE_FILES})
  #     add_custom_command(
  #         OUTPUT ${file}.format
  #         COMMAND clang-format --dry-run --Werror ${file}
  #         DEPENDS ${file}
  #     )
  #     list(APPEND CHECK_STYLE_FILES ${file}.format)
  # endforeach()
  # add_custom_target(verify_list DEPENDS ${CHECK_STYLE_FILES})
  # add_custom_target(verify ALL COMMAND ${CMAKE_COMMAND} --build . --target verify_list)
  # target_link_libraries(sdk_intf_lib INTERFACE verify)
  set(rf_suffix "") 
  if (CONFIG_RF_LABTEST_TOOL) 
   if (CONFIG_RF_POWER_0DBM) 
    set(rf_suffix "_0DBM") 
   elseif (CONFIG_RF_POWER_10DBM) 
    set(rf_suffix "_10DBM") 
   elseif (CONFIG_RF_POWER_14DBM) 
    set(rf_suffix "_14DBM") 
   elseif (CONFIG_RF_POWER_20DBM) 
    set(rf_suffix "_20DBM") 
   endif() 
  endif()
  set(proj_name ${name}_${CONFIG_CHIP}${rf_suffix})
  if((DEFINED CONFIG_RT584H) OR (DEFINED CONFIG_RT584L) OR (DEFINED CONFIG_RT584HA4) OR (DEFINED CONFIG_RT584H_NONE_OS) OR (DEFINED CONFIG_RT584L_NONE_OS) OR (CONFIG_RT584HA4_NONE_OS))
  set(MY_CHIP "rt584")
  elseif((DEFINED CONFIG_RT581) OR (DEFINED CONFIG_RT582) OR (DEFINED CONFIG_RT583) OR (DEFINED CONFIG_RT582_NONE_OS))
  set(MY_CHIP "rt58x")
  elseif((DEFINED CONFIG_RF1301) OR (DEFINED CONFIG_RF1301_NONE_OS))
  set(MY_CHIP "rf1301")
  endif()

  set(HEX_FILE ${CMAKE_BINARY_DIR}/${proj_name}.hex)
  set(BIN_FILE ${CMAKE_BINARY_DIR}/${proj_name}.bin)
  set(MAP_FILE ${CMAKE_BINARY_DIR}/${proj_name}.map)
  set(ASM_FILE ${CMAKE_BINARY_DIR}/${proj_name}.asm)

  add_executable(${proj_name}.elf ${CURRENT_MAIN_FILE})
  target_link_libraries(${proj_name}.elf sdk_intf_lib)
  get_property(LINKER_SCRIPT_PROPERTY GLOBAL PROPERTY LINKER_SCRIPT)

  set(ELF_FILE "${CMAKE_BINARY_DIR}/${proj_name}.elf")
  #message(STATUS "[VSCode] ELF output: ${ELF_FILE}")
  
  if(EXISTS ${LINKER_SCRIPT_PROPERTY})
    set_target_properties(${proj_name}.elf PROPERTIES LINK_FLAGS "-T${LINKER_SCRIPT_PROPERTY} -Wl,-Map=${MAP_FILE}")
    set_target_properties(${proj_name}.elf PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT_PROPERTY})
  endif()

  get_property(SDK_LIBS_PROPERTY GLOBAL PROPERTY SDK_LIBS)
  get_property(SDK_STATIC_LIBS_PROPERTY GLOBAL PROPERTY SDK_STATIC_LIBS)
    
  if (TARGET app)
      target_link_libraries(${proj_name}.elf -Wl,--whole-archive ${SDK_LIBS_PROPERTY} ${SDK_STATIC_LIBS_PROPERTY} app -Wl,--no-whole-archive)
  else()
      target_link_libraries(${proj_name}.elf -Wl,--whole-archive ${SDK_LIBS_PROPERTY} ${SDK_STATIC_LIBS_PROPERTY} -Wl,--no-whole-archive)
  endif()

  if(OUTPUT_DIR)
    add_custom_command(TARGET ${proj_name}.elf POST_BUILD
      COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${proj_name}.elf> ${BIN_FILE}
      COMMAND ${CMAKE_OBJDUMP} -d -S $<TARGET_FILE:${proj_name}.elf> >${ASM_FILE}
      COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${proj_name}.elf> ${HEX_FILE}
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${proj_name}.elf> ${OUTPUT_DIR}/${name}/${proj_name}.elf
      COMMAND ${CMAKE_COMMAND} -E copy ${ASM_FILE} ${OUTPUT_DIR}/${name}/${proj_name}.asm
      COMMAND ${CMAKE_COMMAND} -E copy ${MAP_FILE} ${OUTPUT_DIR}/${name}/${proj_name}.map
      COMMAND ${CMAKE_COMMAND} -E copy ${BIN_FILE} ${OUTPUT_DIR}/${name}/${proj_name}.bin
      COMMAND ${CMAKE_COMMAND} -E copy ${HEX_FILE} ${OUTPUT_DIR}/${name}/${proj_name}.hex
      #COMMAND ${CMAKE_COMMAND} -E copy ${BIN_FILE} ${OUTPUT_DIR}/project.bin
      COMMENT "Generate ${BIN_FILE}\r\n"
    )
  else()
    add_custom_command(TARGET ${proj_name}.elf POST_BUILD
      COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${proj_name}.elf> ${BIN_FILE}
      COMMAND ${CMAKE_OBJDUMP} -d -S $<TARGET_FILE:${proj_name}.elf> >${ASM_FILE}
      COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${proj_name}.elf> ${HEX_FILE}
      COMMENT "Generate ${BIN_FILE}\r\n"
    )
  endif()

   configure_file(
    ${CMAKE_SOURCE_DIR}/.vscode/launch.json.in 
    ${CMAKE_SOURCE_DIR}/.vscode/launch.json @ONLY)

endmacro()

function(get_git_hash OUT_VAR)

  # 1. VERSION file path
  set(VERSION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/VERSION")

  # 2. read VERSION file
  file(STRINGS ${VERSION_FILE} VERSION_CONTENTS)

  # 3. loop process 
  foreach(line ${VERSION_CONTENTS})
    if(line MATCHES "^\\s*#")
      continue()
    endif()

    if(line MATCHES "^([^=]+)=(.*)$")
      set(KEY "${CMAKE_MATCH_1}")
      set(VALUE "${CMAKE_MATCH_2}")

      string(STRIP "${KEY}" KEY)
      string(STRIP "${VALUE}" VALUE)

      set(${KEY} "${VALUE}" CACHE INTERNAL "")
    endif()
  endforeach()

  set(_default_hash "${SHA}" CACHE INTERNAL "")

  find_package(Git QUIET)

  if(GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} log -1 --pretty=format:%h
        OUTPUT_VARIABLE _out
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE _ret
        ERROR_QUIET
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )

    if(_ret EQUAL 0 AND NOT "${_out}" STREQUAL "")
        set(${OUT_VAR} "${_out}" PARENT_SCOPE)
        return()
    endif()
  endif()

  # fallback — PARENT_SCOPE
  set(${OUT_VAR} "${_default_hash}" PARENT_SCOPE)

endfunction()


function(get_fw_version MAC_OUT_VAR BLE_OUT_VAR MULTI_OUT_VAR)
    # 預設回傳空字串
    set(_mac_ver "")
    set(_ble_ver "")
    set(_multi_ver "")

    # -------------------------------
    # 1) Decide FW header paths by chip
    # -------------------------------
    if(
        DEFINED CONFIG_RT581
        OR DEFINED CONFIG_RT582
        OR DEFINED CONFIG_RT583
        OR DEFINED CONFIG_RT582_NONE_OS
    )
        message(STATUS "CONFIG_RT58x doesn't have FW version")

    else()
        if(DEFINED CONFIG_RF1301 OR DEFINED CONFIG_RF1301_NONE_OS)
            if(DEFINED CONFIG_RF_FW_INCLUDE_PCI)
                set(MAC_FW_HEADER ${CMAKE_SOURCE_DIR}/components/network/rt569-fw/rt584/include/prg_rf1301_mpa_asic_pci_fw.h)
            endif()
            if(DEFINED CONFIG_RF_FW_INCLUDE_BLE)
                set(BLE_FW_HEADER ${CMAKE_SOURCE_DIR}/components/network/rt569-fw/rt584/include/prg_rf1301_mpa_asic_ble_fw.h)
            endif()
            if(DEFINED CONFIG_RF_FW_INCLUDE_MULTI_2P4G)
                message(STATUS "RF1301 doesn't have Multi FW")
            endif()

        elseif(DEFINED CONFIG_RT584H OR DEFINED CONFIG_RT584HA4 OR DEFINED CONFIG_RT584H_NONE_OS OR DEFINED CONFIG_RT584HA4_NONE_OS)
            if(DEFINED CONFIG_RF_FW_INCLUDE_PCI)
                set(MAC_FW_HEADER ${CMAKE_SOURCE_DIR}/components/network/rt569-fw/rt584/include/prg_rt584h_mpa_asic_pci_fw.h)
            endif()
            if(DEFINED CONFIG_RF_FW_INCLUDE_BLE)
                set(BLE_FW_HEADER ${CMAKE_SOURCE_DIR}/components/network/rt569-fw/rt584/include/prg_rt584h_mpa_asic_ble_fw.h)
            endif()
            if(DEFINED CONFIG_RF_FW_INCLUDE_MULTI_2P4G)
                set(MULTI_FW_HEADER ${CMAKE_SOURCE_DIR}/components/network/rt569-fw/rt584/include/prg_rt584h_mpa_asic_multi_fw.h)
            endif()

        elseif(DEFINED CONFIG_RT584L OR DEFINED CONFIG_RT584L_NONE_OS)
            if(DEFINED CONFIG_RF_FW_INCLUDE_PCI)
                set(MAC_FW_HEADER ${CMAKE_SOURCE_DIR}/components/network/rt569-fw/rt584/include/prg_rt584l_mpa_asic_pci_fw.h)
            endif()
            if(DEFINED CONFIG_RF_FW_INCLUDE_BLE)
                set(BLE_FW_HEADER ${CMAKE_SOURCE_DIR}/components/network/rt569-fw/rt584/include/prg_rt584l_mpa_asic_ble_fw.h)
            endif()
            if(DEFINED CONFIG_RF_FW_INCLUDE_MULTI_2P4G)
                set(MULTI_FW_HEADER ${CMAKE_SOURCE_DIR}/components/network/rt569-fw/rt584/include/prg_rt584l_mpa_asic_multi_fw.h)
            endif()
        endif()
    

    # -------------------------------
    # 2) Parse versions from FW headers
    # -------------------------------

    # --- MAC ---
    if(DEFINED CONFIG_RF_FW_INCLUDE_PCI)
        if(NOT DEFINED MAC_FW_HEADER)
            message(FATAL_ERROR "CONFIG_RF_FW_INCLUDE_PCI is enabled but MAC_FW_HEADER is not set")
        endif()

        file(READ ${MAC_FW_HEADER} _mac_hdr)
        string(REGEX MATCH "#define[ \t]+FW_MAC_VERSION_NO[ \t]+\\(([^)]+)\\)" _mac_match "${_mac_hdr}")
        if(NOT _mac_match)
            message(STATUS "FW_MAC_VERSION_NO not found in ${MAC_FW_HEADER}")
        endif()

        set(_mac_raw "${CMAKE_MATCH_1}")
        string(REGEX REPLACE "[uUlL]+" "" _mac_ver "${_mac_raw}")
        message(STATUS "FW_MAC_VERSION = ${_mac_ver}")
    endif()

    # --- BLE ---
    if(DEFINED CONFIG_RF_FW_INCLUDE_BLE)
        if(NOT DEFINED BLE_FW_HEADER)
            message(FATAL_ERROR "CONFIG_RF_FW_INCLUDE_BLE is enabled but BLE_FW_HEADER is not set")
        endif()

        file(READ ${BLE_FW_HEADER} _ble_hdr)
        string(REGEX MATCH "#define[ \t]+FW_BLE_VERSION_NO[ \t]+\\(([^)]+)\\)" _ble_match "${_ble_hdr}")
        if(NOT _ble_match)
          message(STATUS "FW_BLE_VERSION_NO not found in ${BLE_FW_HEADER}")
        endif()

        set(_ble_raw "${CMAKE_MATCH_1}")
        string(REGEX REPLACE "[uUlL]+" "" _ble_ver "${_ble_raw}")
        message(STATUS "FW_BLE_VERSION = ${_ble_ver}")
    endif()

    # --- MULTI ---
    if(DEFINED CONFIG_RF_FW_INCLUDE_MULTI_2P4G)
        if(DEFINED CONFIG_RF1301 OR DEFINED CONFIG_RF1301_NONE_OS)
            
        else()
            if(NOT DEFINED MULTI_FW_HEADER)
                message(FATAL_ERROR "CONFIG_RF_FW_INCLUDE_MULTI_2P4G is enabled but MULTI_FW_HEADER is not set")
            endif()

            file(READ ${MULTI_FW_HEADER} _multi_hdr)
            string(REGEX MATCH "#define[ \t]+FW_MULTI_VERSION_NO[ \t]+\\(([^)]+)\\)" _multi_match "${_multi_hdr}")
            if(NOT _multi_match)
                message(STATUS "FW_MULTI_VERSION_NO not found in ${MULTI_FW_HEADER}")
            endif()

            set(_multi_raw "${CMAKE_MATCH_1}")
            string(REGEX REPLACE "[uUlL]+" "" _multi_ver "${_multi_raw}")
            message(STATUS "FW_MULTI_VERSION = ${_multi_ver}")
        endif()
    endif()

    # -------------------------------
    # 3) Return values to caller
    # -------------------------------
    set(${MAC_OUT_VAR}   "${_mac_ver}"   PARENT_SCOPE)
    set(${BLE_OUT_VAR}   "${_ble_ver}"   PARENT_SCOPE)
    set(${MULTI_OUT_VAR} "${_multi_ver}" PARENT_SCOPE)

    message(STATUS "MAC_VER   = ${_mac_ver}")
    message(STATUS "BLE_VER   = ${_ble_ver}")
    message(STATUS "MULTI_VER = ${_multi_ver}")
  endif()
endfunction()

function(config_parse config_file)
    file(READ "${config_file}" file_contents)
    string(REPLACE "\n" ";" file_lines "${file_contents}")

    foreach(line IN LISTS file_lines)
        string(STRIP "${line}" line)
        if(line MATCHES "^#.*" OR line STREQUAL "")
            continue()
        endif()
        if(line MATCHES "^# CONFIG_[A-Za-z0-9_]+ is not set")
            string(REGEX REPLACE "^# (CONFIG_[A-Za-z0-9_]+) is not set" "\\1" var_name "${line}")
            set(${var_name} "n" PARENT_SCOPE)
            #add_compile_options(-D${var_name}=n)
            continue()
        endif()
        if(line MATCHES "^CONFIG_[A-Za-z0-9_]+=y")
            string(REGEX REPLACE "^(CONFIG_[A-Za-z0-9_]+)=y" "\\1" var_name "${line}")
            set(${var_name} "y" PARENT_SCOPE)
            add_compile_options(-D${var_name}=1)
            continue()
        endif()
        if(line MATCHES "^CONFIG_[A-Za-z0-9_]+=\".*\"")
            string(REGEX REPLACE "^(CONFIG_[A-Za-z0-9_]+)=\"(.*)\"" "\\1" var_name "${line}")
            string(REGEX REPLACE "^(CONFIG_[A-Za-z0-9_]+)=\"(.*)\"" "\\2" var_value "${line}")
            set(${var_name} "${var_value}" PARENT_SCOPE)
            add_compile_options(-D${var_name}="${var_value}")
            continue()
        endif()
        if(line MATCHES "^CONFIG_[A-Za-z0-9_]+=[0-9]+")
            string(REGEX REPLACE "^(CONFIG_[A-Za-z0-9_]+)=([0-9]+)" "\\1" var_name "${line}")
            string(REGEX REPLACE "^(CONFIG_[A-Za-z0-9_]+)=([0-9]+)" "\\2" var_value "${line}")
            set(${var_name} "${var_value}" PARENT_SCOPE)
            add_compile_options(-D${var_name}=${var_value})
            continue()
        endif()
        if(line MATCHES "^CONFIG_[A-Za-z0-9_]+=[A-Za-z0-9_]+")
            string(REGEX REPLACE "^(CONFIG_[A-Za-z0-9_]+)=([A-Za-z0-9_]+)" "\\1" var_name "${line}")
            string(REGEX REPLACE "^(CONFIG_[A-Za-z0-9_]+)=([A-Za-z0-9_]+)" "\\2" var_value "${line}")
            set(${var_name} "${var_value}" PARENT_SCOPE)
            add_compile_options(-D${var_name}=${var_value})
            continue()
        endif()
    endforeach()
endfunction()

function(app_git_version OUT_VAR)

    #
    find_package(Git QUIET)

    if(GIT_FOUND)
        execute_process(
            COMMAND git describe --dirty=-test --always --tags --long --match "${CONFIG_BUILD_PORJECT}*"
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_REV OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )

        if(NOT "${GIT_REV}" STREQUAL "")
            message(STATUS "Using git describe: ${GIT_REV}")
            #set(${OUT_VAR} "${GIT_REV}")
            set(${OUT_VAR} "${GIT_REV}" PARENT_SCOPE)
            return()
        endif()
    endif()

    #message(STATUS "Using git commit hash: ${_default_hash}")

    # Step3: return value
    set(${OUT_VAR} "${_default_hash}" PARENT_SCOPE)
endfunction()

function(show_banner) 

  sdk_ifndef(_git_hash "")
  get_git_hash(_git_hash)

  string(SUBSTRING "${CONFIG_CHIP}" 0 8 BUILD_CHIP8)
  message(STATUS "BUILD_CHIP8: ${BUILD_CHIP8}")

  #
  string(SUBSTRING "${_git_hash}" 0 8 _git_hash8)
  add_compile_options(-DLIB_VER=0x${_git_hash8})
  message(STATUS "_git_hash8: ${_git_hash8}")
  # 
  set(RAFAEL_SDK_VER "v${VERSION_MAJOR}.${VERSION_MINOR}.${PATCHLEVEL}" CACHE INTERNAL "")
  add_compile_options(-DRAFAEL_SDK_VER="${RAFAEL_SDK_VER}")
  math(EXPR _version_hex "((${VERSION_MAJOR} << 24) | (${VERSION_MINOR} << 16) | (${PATCHLEVEL} << 8) | ${EXTRAVERSION})")

  message(STATUS "Firmware Version = ${_version_hex} (V${VERSION_MAJOR}.${VERSION_MINOR}.${PATCHLEVEL}.${EXTRAVERSION})")
  # 

  string(TIMESTAMP _build_date "%Y%m%d")
  message(STATUS "CMake build date raw = '${_build_date}'")

  get_fw_version(_mac_ver _ble_ver _multi_ver)

  if(_mac_ver)
    add_compile_definitions(BUILD_MAC_FW_INFO=${_mac_ver})
  endif()

  if(_ble_ver)
      add_compile_definitions(BUILD_BLE_FW_INFO=${_ble_ver})
  endif()

  if(_multi_ver)
      add_compile_definitions(BUILD_MULTI_FW_INFO=${_multi_ver})
  endif()

  add_compile_options(
  -DBUILD_CHIP_INFO=${BUILD_CHIP8}
  -DBUILD_VERSION_INFO=${_version_hex}
  -DBUILD_HASH_INFO=0x${_git_hash8}  
  -DBUILD_DATE_INFO=0x${_build_date}
  )



  message("   _____       ______         _   _____   _______    _____ _____  _  __")
  message("  |  __ \\     |  ____|       | | |_   _| |__   __|  / ____|  __ \\| |/ /")
  message("  | |__) |__ _| |__ __ _  ___| |   | |  ___ | |    | (___ | |  | | ' /")
  message("  |  _  // _` |  __/ _` |/ _ \\ |   | | / _ \\| |     \\___ \\| |  | |  |")
  message("  | | \\ \\ (_| | | | (_| |  __/ |  _| || (_) | |     ____) | |__| | . \\")
  message("  |_|  \\_\\__,_|_|  \\__,_|\\___|_| |_____\\___/|_|    |_____/|_____/|_|\\_\\")
  message("")

  # 
  message(STATUS "Current SDK version: ${RAFAEL_SDK_VER}")
  message(STATUS "Build Project: ${CONFIG_BUILD_PORJECT}")

endfunction()
