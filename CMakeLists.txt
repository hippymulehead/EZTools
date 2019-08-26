cmake_minimum_required(VERSION 2.8)
project(eztools)

set(CMAKE_CXX_STANDARD 11)
IF(CMAKE_VERSION VERSION_LESS 3.0.0)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fwrapv")
ENDIF()

set(EZTOOLS_LIBS include/EZConfig.h src/EZConfig.cpp include/EZHTTP.h src/EZHTTP.cpp include/EZLinux.h
        src/EZLinux.cpp include/EZScreen.h src/EZScreen.cpp include/EZTools.h src/EZTools.cpp
        include/EZProc.h src/EZProc.cpp include/EZLogger.h src/EZLogger.cpp include/EZFileManager.h
        src/EZFileManager.cpp src/EZGetOpt.cpp include/EZGetOpt.h)

## DYNAMIC LIB
add_library(eztools SHARED ${EZTOOLS_LIBS})

## STATIC LIB
#add_library(eztools ${EZTOOLS_LIBS})

target_include_directories(eztools PUBLIC include)

target_link_libraries(eztools PocoFoundation PocoCrypto crypto jsoncpp curl)

install(TARGETS eztools DESTINATION /usr/lib/)
install(DIRECTORY include/ DESTINATION /usr/include/EZTools)

# Version Info 1
set(PROJECT_VERSION_MAJOR "2")
<<<<<<< HEAD
set(PROJECT_VERSION_MINOR "1")
set(PROJECT_VERSION_PATCH "10")
=======
set(PROJECT_VERSION_MINOR "2")
set(PROJECT_VERSION_PATCH "2")
>>>>>>> master
set(CPACK_PACKAGE_RELEASE 1)
set(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

# CPACK Stuff
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "EZ Tools for C++ Programming Lib")
set(CPACK_PACKAGE_RELOCATABLE "false")
set(CPACK_PACKAGE_NAME "libeztools")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_CONTACT "Michael Romans")
set(CPACK_PACKAGE_VENDOR "Romans Audio")
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")

# RPM Stuff
set(CPACK_RPM_PACKAGE_RELOCATABLE "false")
set(CPACK_RPM_PACKAGE_AUTOREQ "NO")
set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
set(CPACK_RPM_PACKAGE_REQUIRES "poco-foundation >= 1.6.1, poco-crypto >= 1.6.1, jsoncpp >= 0.10.5, curl >= 7.29.0")
set(CPACK_RPM_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION_SUMMARY})
message(** " " ${CMAKE_SYSTEM})
string (REGEX MATCH "el[1-9]|fc[0-9][0-9]" os_version_suffix ${CMAKE_SYSTEM})
message("** ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${os_version_suffix}.${CMAKE_SYSTEM_PROCESSOR}")

string (REGEX MATCH "el|fc" OS_ID ${os_version_suffix})
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${os_version_suffix}.${CMAKE_SYSTEM_PROCESSOR}")
set(CPACK_GENERATOR "RPM")
add_definitions(-DDISTRO_${OS_ID})

include(CPack)