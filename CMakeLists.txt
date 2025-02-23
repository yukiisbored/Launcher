cmake_minimum_required(VERSION 3.1)

if(WIN32)
    # In Qt 5.1+ we have our own main() function, don't autolink to qtmain on Windows
    cmake_policy(SET CMP0020 OLD)
endif()

project(Launcher)
enable_testing()

string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_BUILD_DIR}" IS_IN_SOURCE_BUILD)
if(IS_IN_SOURCE_BUILD)
    message(FATAL_ERROR "You are building the Launcher in-source. Please separate the build tree from the source tree.")
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    if(CMAKE_HOST_SYSTEM_VERSION MATCHES ".*[Mm]icrosoft.*" OR
        CMAKE_HOST_SYSTEM_VERSION MATCHES ".*WSL.*"
    )
        message(FATAL_ERROR "Building the Launcher is not supported in Linux-on-Windows distributions.")
    endif()
endif()


##################################### Set CMake options #####################################
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")

# Output all executables and shared libs in the main build folder, not in subfolders.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
if(UNIX)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
endif()
set(CMAKE_JAVA_TARGET_OUTPUT_DIR ${PROJECT_BINARY_DIR}/jars)

######## Set compiler flags ########
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_C_STANDARD_REQUIRED true)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)
include(GenerateExportHeader)
set(CMAKE_CXX_FLAGS " -Wall -pedantic -Werror -Wno-deprecated-declarations -D_GLIBCXX_USE_CXX11_ABI=0 -fstack-protector-strong --param=ssp-buffer-size=4 -O3 -D_FORTIFY_SOURCE=2 ${CMAKE_CXX_FLAGS}")
if(UNIX AND APPLE)
    set(CMAKE_CXX_FLAGS " -stdlib=libc++ ${CMAKE_CXX_FLAGS}")
endif()
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Werror=return-type")

##################################### Set Application options #####################################

######## Set version numbers ########
set(Launcher_VERSION_MAJOR    0)
set(Launcher_VERSION_MINOR    6)
set(Launcher_VERSION_HOTFIX   14)

# Build number
set(Launcher_VERSION_BUILD -1 CACHE STRING "Build number. -1 for no build number.")

# Build platform.
set(Launcher_BUILD_PLATFORM "" CACHE STRING "A short string identifying the platform that this build was built for. Only used by the notification system and to display in the about dialog.")

# Notification URL
set(Launcher_NOTIFICATION_URL "" CACHE STRING "URL for checking for notifications.")

# The metadata server
set(Launcher_META_URL "https://meta.multimc.org/v1/" CACHE STRING "URL to fetch Launcher's meta files from.")

# paste.ee API key
set(Launcher_PASTE_EE_API_KEY "uYoTzjDW4SwwOc3ttjSNqh67VuwHZDYrCLdFXrhhz" CACHE STRING "API key you can get from paste.ee when you register an account")

# Imgur API Client ID
set(Launcher_IMGUR_CLIENT_ID "99337d09c06f6e5" CACHE STRING "Client ID you can get from Imgur when you register an application")

# Bug tracker URL
set(Launcher_BUG_TRACKER_URL "" CACHE STRING "URL for the bug tracker.")

# Discord URL
set(Launcher_DISCORD_URL "" CACHE STRING "URL for the Discord guild.")

# Subreddit URL
set(Launcher_SUBREDDIT_URL "" CACHE STRING "URL for the subreddit.")

# System LWJGL2 Path
set(BGL_SYSTEM_LWJGL2_PATH "" CACHE STRING "Location for system installed LWJGL 2 library")

# System LWJGL3 Path
set(BGL_SYSTEM_LWJGL3_PATH "" CACHE STRING "Location for system installed LWJGL 3 library")

# Microsoft Client ID
set(BGL_MSA_CLIENT_ID "" CACHE STRING "Microsoft Client ID to support Microsoft/Xbox accounts")

#### Check the current Git commit and branch
include(GetGitRevisionDescription)
get_git_head_revision(Launcher_GIT_REFSPEC Launcher_GIT_COMMIT)

message(STATUS "Git commit: ${Launcher_GIT_COMMIT}")
message(STATUS "Git refspec: ${Launcher_GIT_REFSPEC}")

set(Launcher_RELEASE_VERSION_NAME "${Launcher_VERSION_MAJOR}.${Launcher_VERSION_MINOR}.${Launcher_VERSION_HOTFIX}")

#### Custom target to just print the version.
add_custom_target(version echo "Version: ${Launcher_RELEASE_VERSION_NAME}")
add_custom_target(tcversion echo "\\#\\#teamcity[setParameter name=\\'env.LAUNCHER_VERSION\\' value=\\'${Launcher_RELEASE_VERSION_NAME}\\']")

################################ 3rd Party Libs ################################

# Find the required Qt parts
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Concurrent REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Test REQUIRED)
find_package(Qt5Xml REQUIRED)

# The Qt5 cmake files don't provide its install paths, so ask qmake.
include(QMakeQuery)
query_qmake(QT_INSTALL_PLUGINS QT_PLUGINS_DIR)
query_qmake(QT_INSTALL_IMPORTS QT_IMPORTS_DIR)
query_qmake(QT_INSTALL_LIBS QT_LIBS_DIR)
query_qmake(QT_INSTALL_LIBEXECS QT_LIBEXECS_DIR)
query_qmake(QT_HOST_DATA QT_DATA_DIR)
set(QT_MKSPECS_DIR ${QT_DATA_DIR}/mkspecs)

if (Qt5_POSITION_INDEPENDENT_CODE)
    SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

####################################### Secrets #######################################

add_subdirectory(notsecrets)

####################################### Install layout #######################################

set(BINARY_DEST_DIR "bin")
set(LIBRARY_DEST_DIR "lib")
set(JARS_DEST_DIR "share/bgl/jars")

set(INSTALL_BUNDLE "nodeps")

################################ Included Libs ################################

include(ExternalProject)
set_directory_properties(PROPERTIES EP_BASE External)

option(NBT_BUILD_SHARED "Build NBT shared library" ON)
option(NBT_USE_ZLIB "Build NBT library with zlib support" OFF)
option(NBT_BUILD_TESTS "Build NBT library tests" OFF) #FIXME: fix unit tests.
set(NBT_NAME Launcher_nbt++)
set(NBT_DEST_DIR ${LIBRARY_DEST_DIR})
add_subdirectory(libraries/libnbtplusplus)

add_subdirectory(libraries/systeminfo) # system information library
add_subdirectory(libraries/hoedown) # markdown parser
add_subdirectory(libraries/launcher) # java based launcher part for Minecraft
add_subdirectory(libraries/javacheck) # java compatibility checker
add_subdirectory(libraries/xz-embedded) # xz compression
add_subdirectory(libraries/quazip) # zip manipulation library
add_subdirectory(libraries/rainbow) # Qt extension for colors
add_subdirectory(libraries/iconfix) # fork of Qt's QIcon loader
add_subdirectory(libraries/LocalPeer) # fork of a library from Qt solutions
add_subdirectory(libraries/classparser) # parse java classes
add_subdirectory(libraries/optional-bare)
add_subdirectory(libraries/tomlc99) # toml parser
add_subdirectory(libraries/katabasis) # An OAuth2 library that tried to do too much

############################### Built Artifacts ###############################

add_subdirectory(buildconfig)

# NOTE: this must always be last to appease the CMake deity of quirky install command evaluation order.
add_subdirectory(launcher)
