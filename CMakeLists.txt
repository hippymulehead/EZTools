## RA Build Template
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
###################################
# Set CMake Policy
###################################
if (POLICY CMP0048)
    CMAKE_POLICY(SET CMP0048 NEW)
endif (POLICY CMP0048)
###################################
# Set build options
###################################
OPTION(RPM OFF)
OPTION(DEB OFF)
###################################
# Version Info (Start Edits)
###################################
SET(CPACK_PACKAGE_VERSION_MAJOR "4")
SET(CPACK_PACKAGE_VERSION_MINOR "6")
SET(CPACK_PACKAGE_VERSION_PATCH "12")
SET(CPACK_PACKAGE_RELEASE 1)
SET(_PACKAGE_NAME "eztools")
SET(CPACK_PACKAGE_CONTACT "mromans@romansaudio.com")
SET(CPACK_PACKAGE_VENDOR "Romans Audio")
SET(CPACK_PACKAGE_DESCRIPTION "C++ Tools")
SET(CMAKE_INSTALL_DIR "/usr/bin")
SET(_INCLUDE_DIR "/usr/include/eztools")
## CentOS 7 Only
SET(CENTOS7_REQUIRED_PACKAGES "poco-foundation >= 1.6.1, poco-devel >= 1.6.1, curl >= 7.29.0, curl-devel >= 7.29.0, libsodium >= 0.9.0, libsodium-devel >= 0.9.0")
## EL 8 & Fedora
SET(RPM_REQUIRED_PACKAGES "curl >= 7.29.0, curl-devel >= 7.29.0, libsodium >= 0.9.0, libsodium-devel >= 0.9.0")
## Deb and Ubuntu based
SET(DEB_REQUIRED_PACKAGES "libcurl4-openssl-dev, libcurl4, libsodium23")
###################################
# Don't Edit Below Here
###################################
SET(PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
MESSAGE("** PACKAGE_VERSION = ${PACKAGE_VERSION}")
PROJECT("${_PACKAGE_NAME}")
###################################
# CentOS 7 experimental support
###################################
if (RPM)
    SET(CMAKE_CXX_STANDARD 11)
    IF(CMAKE_VERSION VERSION_LESS 3.0.0)
        SET(EL_VERSION "7")
        SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fwrapv")
    ELSE()
        SET(EL_VERSION "8")
    ENDIF()
endif ()
if (DEB)
    SET(CMAKE_CXX_STANDARD 11)
endif ()

STRING (REGEX MATCH "el[1-9]|fc[0-9][0-9]" os_version_suffix ${CMAKE_SYSTEM})

if (RPM)
    STRING (REGEX MATCH "el|fc" OS_ID ${os_version_suffix})
    if (OS_ID STREQUAL "el")
        if(EL_VERSION STREQUAL "7")
            SET(REQUIRED_PACKAGES ${CENTOS7_REQUIRED_PACKAGES})
        ELSE()
            SET(REQUIRED_PACKAGES ${RPM_REQUIRED_PACKAGES})
        ENDIF()
    endif()
    if (OS_ID STREQUAL "fc")
        SET(REQUIRED_PACKAGES ${RPM_REQUIRED_PACKAGES})
    endif()
endif ()
if (DEB)
    set(OS_ID "deb")
    SET(REQUIRED_PACKAGES ${DEB_REQUIRED_PACKAGES})
endif ()

###################################
# Files
###################################
FILE(GLOB EZTOOLS EZTools/*.h)
FILE(GLOB CXXURL EZTools/cxxurl/*.h)
###################################
# Build lib versions
###################################
ADD_EXECUTABLE(${_PACKAGE_NAME} main.cpp ${EZTOOLS} ${CXXURL})
TARGET_INCLUDE_DIRECTORIES(${_PACKAGE_NAME} PUBLIC EZTools)
if (OS_ID STREQUAL "el")
    IF(EL_VERSION STREQUAL "7")
        TARGET_LINK_LIBRARIES(${_PACKAGE_NAME} PocoFoundation curl sodium resolv cryptopp pthread)
    ELSE()
        TARGET_LINK_LIBRARIES(${_PACKAGE_NAME} curl sodium resolv cryptopp pthread)
    ENDIF()
else()
    TARGET_LINK_LIBRARIES(${_PACKAGE_NAME} curl sodium resolv cryptopp pthread)
endif()

###################################
# Install methods
###################################
install(DIRECTORY EZTools/ DESTINATION ${_INCLUDE_DIR})
###################################
# CPack Stuff
###################################
SET(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
SET(CPACK_PACKAGING_INSTALL_PREFIX "/usr")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${CPACK_PACKAGE_DESCRIPTION})
SET(CPACK_PACKAGE_RELOCATABLE "false")
###################################
# RPM Stuff
###################################
if (RPM)
    SET(CPACK_RPM_PACKAGE_REQUIRES ${REQUIRED_PACKAGES})
    set(CPACK_PACKAGE_NAME "${_PACKAGE_NAME}")
    SET(CPACK_GENERATOR "RPM")
    MESSAGE("** BUILDING RPM")
    SET(CPACK_RPM_PACKAGE_AUTOREQ "NO")
    SET(CPACK_RPM_PACKAGE_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
    SET(CPACK_RPM_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION_SUMMARY})
    MESSAGE("** CMAKE_SYSTEM ${CMAKE_SYSTEM}")
    SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${os_version_suffix}.${CMAKE_SYSTEM_PROCESSOR}")
    MESSAGE("** ${CPACK_PACKAGE_FILE_NAME}")
    MESSAGE("** OS_ID ${OS_ID}")
endif ()

if (DEB)
    SET(CPACK_DEBIAN_PACKAGE_DEPENDS ${REQUIRED_PACKAGES})
    SET(CPACK_DEBIAN_PACKAGE_RELEASE ${CPACK_PACKAGE_RELEASE})
    set(CPACK_PACKAGE_NAME "${_PACKAGE_NAME}")
    SET(CPACK_GENERATOR "DEB")
    MESSAGE("** BUILDING DEB")
    SET(CPACK_DEBIAN_PACKAGE_MAINTAINER ${CPACK_PACKAGE_CONTACT})
    MESSAGE("** CMAKE_SYSTEM ${CMAKE_SYSTEM}")
    SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")
    SET(CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
endif ()

message("** OS_ID = ${OS_ID}")
if (OS_ID STREQUAL "el")
    if(EL_VERSION STREQUAL "7")
        ADD_DEFINITIONS(-DVERSION="${PACKAGE_VERSION}" -DCENTOS7="ON")
    ELSE()
        ADD_DEFINITIONS(-DVERSION="${PACKAGE_VERSION}")
    ENDIF()
ELSE()
    ADD_DEFINITIONS(-DVERSION="${PACKAGE_VERSION}")
ENDIF()

INCLUDE(CPack)
UNSET(DEB CACHE)
UNSET(RPM CACHE)