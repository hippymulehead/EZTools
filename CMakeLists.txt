cmake_minimum_required(VERSION 3.12)
project(EZTools)

set(CMAKE_CXX_STANDARD 11)
IF(CMAKE_VERSION VERSION_LESS 3.0.0)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
ENDIF()

add_library(EZTools SHARED src/EZHTTP.cpp include/EZHTTP.h include/EZString.h include/EZHTTPResponse.h
        include/EZLinux.h include/EZLogger.h include/EZProc.h include/EZScreen.h include/EZSystemTime.h
        include/EZUtils.h src/EZHTTPResponse.cpp src/EZLinux.cpp src/EZLogger.cpp src/EZProc.cpp src/EZScreen.cpp
        src/EZString.cpp src/EZSystemTime.cpp src/EZUtils.cpp src/EZURI.cpp include/EZURI.h src/EZFileManager.cpp include/EZFileManager.h src/EZSTATUS.cpp include/EZSTATUS.h)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREAD ON)

target_link_libraries(EZTools boost_regex boost_system boost_filesystem crypto curl)

set(MAJOR "1")
set(MINOR "5")
set(BUILD "0")
set(CPACK_PACKAGE_VERSION "${MAJOR}.${MINOR}.${BUILD}")

execute_process(COMMAND mv libEZTools.so libEZTools.so.${MAJOR}.${MINOR}.${BUILD})
execute_process(COMMAND ln -s libEZTools.so.${MAJOR}.${MINOR}.${BUILD} libEZTools.so.${MAJOR}.${MINOR})
execute_process(COMMAND ln -s libEZTools.so.${MAJOR}.${MINOR}.${BUILD} libEZTools.so.${MAJOR})
execute_process(COMMAND ln -s libEZTools.so.${MAJOR}.${MINOR}.${BUILD} libEZTools.so)

install(TARGETS EZTools DESTINATION /usr/lib/)
execute_process(COMMAND ln -s /usr/lib/libEZTools.so ${CMAKE_BINARY_DIR}/libEZTools.so.${MAJOR})
install(FILES ${CMAKE_BINARY_DIR}/libEZTools.so.${MAJOR} DESTINATION /usr/lib/libEZTools.so.${MAJOR} COMPONENT EZTools)
install(DIRECTORY include/ DESTINATION /usr/include/EZTools)
execute_process(COMMAND ldconfig COMPONENT EZTools)

set(CPACK_RPM_PACKAGE_RELOCATABLE "false")
set(CPACK_PACKAGE_RELOCATABLE "false")
set(CPACK_PACKAGE_VERSION ${VERSION})
#set(CPACK_GENERATOR "RPM")
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "libEZTools")
set(CPACK_PACKAGE_RELEASE 1)
set(CPACK_PACKAGE_CONTACT "Michael Romans")
set(CPACK_PACKAGE_VENDOR "Romans Audio")
set(CPACK_RPM_PACKAGE_AUTOREQ "NO")
set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
set(CPACK_RPM_PACKAGE_REQUIRES "curl >= 7.29.0, boost-system >= 1.53.0, boost-regex >= 1.53.0, jsoncpp >= 0.10.5")
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")
include(CPack)