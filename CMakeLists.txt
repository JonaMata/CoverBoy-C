cmake_minimum_required(VERSION 3.27)
project(coverboy)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_INSTALL_PREFIX "/usr")

# === Packging ===
# these are cache variables, so they could be overwritten with -D,
set(CPACK_PACKAGE_NAME ${PROJECT_NAME}
        CACHE STRING "The resulting package name"
)
# which is useful in case of packing only selected components instead of the whole thing
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "RasPi Album cover display"
        CACHE STRING "Package description for the package metadata"
)
set(CPACK_PACKAGE_VENDOR "Jonathan Matarazzi")

set(CPACK_VERBATIM_VARIABLES YES)

set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_PACKAGE_NAME})
SET(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_SOURCE_DIR}/_packages")

# https://unix.stackexchange.com/a/11552/254512
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr/bin")

set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})

set(CPACK_PACKAGE_CONTACT "git@jonathanm.nl")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Jonathan Matarazzi")

#set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
#set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "armhf")

# package name for deb. If set, then instead of some-application-0.9.2-Linux.deb
# you'll get some-application_0.9.2_amd64.deb (note the underscores too)
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
# that is if you want every group to have its own package,
# although the same will happen if this is not set (so it defaults to ONE_PER_GROUP)
# and CPACK_DEB_COMPONENT_INSTALL is set to YES
set(CPACK_COMPONENTS_GROUPING ALL_COMPONENTS_IN_ONE)#ONE_PER_GROUP)
# without this you won't be able to pack only specified component
set(CPACK_DEB_COMPONENT_INSTALL YES)

include(CPack)



# === boost ===
#find_package(Boost REQUIRED COMPONENTS filesystem system thread regex)
#find_package(Boost 1.75 REQUIRED CONFIG)
find_package(Boost 1.75 REQUIRED)

# To use PKG_CHECK_MODULES to find some optional packages
find_package(PkgConfig)

# Flags to enable native support for JPEG image files, using the JPEG library.
# ( http://www.ijg.org/ )
SET(CIMG_JPEG_CCFLAGS  -Dcimg_use_jpeg)

# Flags to enable native support for PNG image files, using the PNG library.
# ( http://www.libpng.org/ )
SET(CIMG_PNG_CCFLAGS  -Dcimg_use_png)

# Flags to enable native support of most classical image file formats, using the Magick++ library.
# ( http://www.imagemagick.org/Magick++/ )
SET(CIMG_MAGICK_CCFLAGS -Dcimg_use_magick)

FIND_PACKAGE(JPEG REQUIRED)
FIND_PACKAGE(PNG REQUIRED)
FIND_PACKAGE(ZLIB REQUIRED)

find_package(cimg REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${cimg_INCLUDE_DIRS})


PKG_CHECK_MODULES(MAGICK Magick++)

if(NOT WIN32)
    FIND_PACKAGE(Threads REQUIRED)
endif()

if(JPEG_FOUND)
    get_filename_component(JPEG_LIB_DIRS ${JPEG_LIBRARIES} PATH)
    SET(CIMG_CFLAGS "${CIMG_CFLAGS} ${CIMG_JPEG_CCFLAGS}")
    link_directories(${JPEG_LIB_DIRS})
    include_directories(${JPEG_INCLUDE_DIR})
    SET(SYSTEM_LIBS ${SYSTEM_LIBS} ${JPEG_LIBRARIES})
endif()




if (ZLIB_FOUND)
    SET(CIMG_CFLAGS "${CIMG_CFLAGS} ${CIMG_ZLIB_CCFLAGS}")
    link_directories(${ZLIB_LIB_DIRS})
    include_directories(${ZLIB_INCLUDE_DIR})
    SET(SYSTEM_LIBS ${SYSTEM_LIBS} ${ZLIB_LIBRARIES})

    # PNG requires ZLIB
    if(PNG_FOUND)
        SET(CIMG_CFLAGS "${CIMG_CFLAGS} ${CIMG_PNG_CCFLAGS}")
        link_directories(${PNG_LIB_DIRS})
        include_directories(${PNG_INCLUDE_DIR} )
        SET( SYSTEM_LIBS ${SYSTEM_LIBS} ${PNG_LIBRARIES} )
    endif()
endif()



if(MAGICK_FOUND)
    SET(CIMG_CFLAGS "${CIMG_CFLAGS} ${CIMG_MAGICK_CCFLAGS}")
    link_directories( ${MAGICK_LIBRARY_DIRS} )
    include_directories( ${MAGICK_INCLUDE_DIRS} )
    SET( SYSTEM_LIBS ${SYSTEM_LIBS} ${MAGICK_LIBRARIES} )
endif()


if (NOT WIN32)
    SET( SYSTEM_LIBS ${SYSTEM_LIBS} ${CMAKE_THREAD_LIBS_INIT} )
endif()

add_library(rgb_matrix STATIC IMPORTED)
set_target_properties(rgb_matrix PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/rpi-rgb-led-matrix/lib/librgbmatrix.a)

add_executable(coverboy src/main.cpp)
target_link_libraries(coverboy INTERFACE ${SYSTEM_LIBS} Boost::json cimg::cimg)
target_link_libraries(coverboy PRIVATE rgb_matrix)

install(TARGETS coverboy DESTINATION ${BINDIR})
