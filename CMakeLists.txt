CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(simbox_test)

SET(INCL_FOLDER "./include")


## UNCOMMENT THIS FOR STAMPEDE COMPILE ############
#SET(HDF5_DIR ${TACC_HDF5_DIR})
#SET(CMAKE_CXX_COMPILER "/opt/apps/intel15/mvapich2/2.1/bin/mpicxx")
#SET(CMAKE_CXX_FLAGS "-std=c++11 -O2 -w -Wall -Werror")
#LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules/")
#MESSAGE(STATUS "  *************** ")
#FOREACH(item ${CMAKE_MODULE_PATH})
#    MESSAGE(STATUS "  path: " ${item})
#ENDFOREACH()
#MESSAGE(STATUS "  *************** ")
#LIST(APPEND ALL_LIBRARIES "$ENV{TACC_HDF5_LIB}/libsz.so")
##LIST(APPEND INCL_FOLDER $ENV{TACC_T3PIO_INC})
##LIST(APPEND ALL_LIBRARIES "$ENV{TACC_T3PIO_LIB}/libt3pio.so")
#LIST(APPEND INCL_FOLDER $ENV{TACC_HDF5_INC})
#SET(ALL_LIBRARIES ${ALL_LIBRARIES} "$ENV{TACC_HDF5_LIB}/libhdf5.so")
##########################################




## UNCOMMENT THIS FOR DESKTOP COMPILE #############
SET(HDF5_DIR "~/GitProjects/plasmeta/libs/HDF5-1.8.16-Linux/HDF_Group/HDF5/1.8.16/share/cmake")
SET(CMAKE_CXX_COMPILER "/usr/bin/mpicxx")
SET(CMAKE_CXX_FLAGS "-std=c++11 -O2 -w -Wall -Werror")
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules/")
MESSAGE(STATUS "  *************** ")
FOREACH(item ${CMAKE_MODULE_PATH})
    MESSAGE(STATUS "  path: " ${item})
ENDFOREACH()
MESSAGE(STATUS "  *************** ")
# package config
FIND_PACKAGE( PkgConfig REQUIRED )
# HDF5
FIND_PACKAGE(HDF5 NAMES hdf5 COMPONENTS C static)
MESSAGE(STATUS "HDF5 C libs:${HDF5_FOUND} static:${HDF5_static_C_FOUND} and shared:${HDF5_shared_C_FOUND}")
LIST(APPEND INCL_FOLDER ${HDF5_INCLUDE_DIR}})
SET(ALL_LIBRARIES ${ALL_LIBRARIES} ${HDF5_C_STATIC_LIBRARY})
# ZLib (required for HDF5)
# LIST(APPEND ALL_LIBRARIES "../../libs/HDF5-1.8.16-Linux/HDF_Group/HDF5/1.8.16/lib/libz.a"
# FIND_PACKAGE(ZLIB REQUIRED)
# SET(ALL_LIBRARIES ${ALL_LIBRARIES} ${ZLIB_LIBRARIES})
#####################################################



INCLUDE_DIRECTORIES(${INCL_FOLDER})
ADD_EXECUTABLE(simbox_test simbox_test.cpp)

FOREACH(item ${INCL_FOLDER})
    MESSAGE(STATUS "  including: " ${item})
ENDFOREACH()
FOREACH(item ${ALL_LIBRARIES})
    MESSAGE(STATUS "  using lib: " ${item})
ENDFOREACH()

TARGET_LINK_LIBRARIES( simbox_test ${ALL_LIBRARIES})
