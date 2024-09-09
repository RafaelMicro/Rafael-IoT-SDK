
sdk_add_include_directories(${CMAKE_CURRENT_LIST_DIR}/crypto_hmac_drbg/Include)

sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/crypto_hmac_drbg/main.c)
setup_project(crypto_hmac_drbg)