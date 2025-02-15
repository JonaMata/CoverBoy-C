cmake_minimum_required(VERSION 3.18)
project(coverboy)

set(CMAKE_CXX_STANDARD 20)
# === boost ===
#find_package(Boost REQUIRED COMPONENTS filesystem system thread regex)
find_package(Boost REQUIRED CONFIG)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/CImg)
include_directories(${PROJECT_SOURCE_DIR}/easywsclient)

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

FIND_PACKAGE(JPEG)
FIND_PACKAGE(PNG)
FIND_PACKAGE(ZLIB)


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

add_custom_command(
        OUTPUT ${PROJECT_SOURCE_DIR}/rpi-rgb-led-matrix/lib/librgbmatrix.a
        COMMAND make -C ${PROJECT_SOURCE_DIR}/rpi-rgb-led-matrix/lib
        VERBATIM
)

add_custom_target(rgb_matrix_library ALL
        DEPENDS ${PROJECT_SOURCE_DIR}/rpi-rgb-led-matrix/lib/librgbmatrix.a
)

add_library(rgb_matrix STATIC IMPORTED)
set_target_properties(rgb_matrix PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/rpi-rgb-led-matrix/lib/librgbmatrix.a)

add_executable(coverboy src/main.cpp)
target_link_libraries(coverboy INTERFACE ${SYSTEM_LIBS} Boost::json Boost::Container)
target_link_libraries(coverboy PRIVATE rgb_matrix)
add_dependencies(coverboy rgb_matrix_library)

install(TARGETS coverboy DESTINATION bin)
