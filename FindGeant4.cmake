# - Try to find GEANT4
# Once done this will define
#
#  GEANT4_FOUND - system has GEANT4
#  GEANT4_INC_DIR - the GEANT4 include directory
#  GEANT4_LIBRARIES - The libraries needed to use GEANT4
#  GEANT4_DEFINITIONS - Compiler switches required for using GEANT4
#
# IMPORTANT if you want to use granular libraries:
# answer the question "Do you want to use granular libraries in your applications?" with "yes"

MESSAGE(STATUS "    Looking for Geant4")

#
# Find GEANT4 installation directory from G4INSTALL environmental variable 
#
FIND_PATH (GEANT4_DIR NAMES env.sh
  PATHS $ENV{G4INSTALL} $ENV{G4INSTALL}/source
  NO_DEFAULT_PATH
)

IF(NOT ${GEANT4_DIR} MATCHES ".*NOTFOUND$")
  #
  # Read the GEANT4 environment
  #
  EXECUTE_PROCESS (COMMAND ${GEANT4_DIR}/env.sh OUTPUT_VARIABLE GEANT4_ENV)
  #
  # Extract the CLHEP_BASE_DIR
  #
  STRING (REGEX REPLACE ".*CLHEP_BASE_DIR=([^ ]+)\n.*" "\\1" CLHEP_BASE_DIR "${GEANT4_ENV}")
  IF(NOT ${CLHEP_BASE_DIR} MATCHES ".*NOTFOUND$")
    #
    # Extract the G4SYSTEM
    #
    STRING (REGEX REPLACE ".*G4SYSTEM=([^ ]+)\n.*" "\\1" GEANT4_SYSTEM "${GEANT4_ENV}")
    #
    # Extract the use of granular libraries option
    #
    STRING( REGEX REPLACE ".*G4LIB_USE_GRANULAR=([0-1]+)\n.*" "\\1" GEANT4_LIB_USE_GRANULAR "${GEANT4_ENV}")
    #
    # Find include directory
    #
    FIND_PATH (GEANT4_INC_DIR NAMES G4RunManager.hh
      PATHS $ENV{G4INSTALL}/include
      NO_DEFAULT_PATH
    )
    IF(NOT ${GEANT4_INC_DIR} MATCHES ".*NOTFOUND$")
      #
      # Find lib directory
      #
      FIND_PATH (GEANT4_LIB_DIR NAMES libG4event.so
        PATHS $ENV{G4INSTALL}/lib/${GEANT4_SYSTEM}
        NO_DEFAULT_PATH
      )
      IF(NOT ${GEANT4_LIB_DIR} MATCHES ".*NOTFOUND$")
        #
        # Check if liblist and libname.map exist and if the flag G4LIB_USE_GRANULAR is set --> granular libraries
        #
        FIND_PROGRAM (GEANT4_LIBLIST liblist
          PATHS ${GEANT4_LIB_DIR}
          DOC "liblist program distributed with the Geant4 libraries"
        )
        IF (GEANT4_LIB_USE_GRANULAR STREQUAL 1)
          SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DG4LIB_USE_GRANULAR")
        ENDIF (GEANT4_LIB_USE_GRANULAR STREQUAL 1)
        
        FIND_FILE (GEANT4_LIB_MAP libname.map ${GEANT4_LIB_DIR})
        SET(GEANT4_LIB_GRANULAR FALSE)
        IF (GEANT4_LIB_MAP AND GEANT4_LIB_USE_GRANULAR STREQUAL 1)
          SET(GEANT4_LIB_GRANULAR TRUE)
        ENDIF(GEANT4_LIB_MAP AND GEANT4_LIB_USE_GRANULAR STREQUAL 1)
        #
        # Find libraries
        #
        IF(GEANT4_LIB_GRANULAR)
          #
          # Using granular libraries
          #
          EXECUTE_PROCESS(COMMAND cat ${GEANT4_LIB_MAP}
            COMMAND ${GEANT4_LIBLIST} -m ${GEANT4_LIB_DIR}
            OUTPUT_VARIABLE LIBLIST_OUTPUT OUTPUT_STRIP_TRAILING_WHITESPACE
          )
          #
          # Extract library names
          #
          STRING(REGEX REPLACE "-l([^ ]*)" "${GEANT4_LIB_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}\\1${CMAKE_SHARED_LIBRARY_SUFFIX}"
            GEANT4_LIBRARIES ${LIBLIST_OUTPUT})
          SEPARATE_ARGUMENTS(GEANT4_LIBRARIES)
        ELSE(GEANT4_LIB_GRANULAR)
          #
          # Using compound libraries
          #
          FILE(GLOB GEANT4_LIBRARIES "${GEANT4_LIB_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}*${CMAKE_SHARED_LIBRARY_SUFFIX}")
          SEPARATE_ARGUMENTS(GEANT4_LIBRARIES)
        ENDIF(GEANT4_LIB_GRANULAR)
        
        IF (GEANT4_DIR AND GEANT4_INC_DIR AND GEANT4_LIB_DIR)
          SET(GEANT4_FOUND TRUE)
        ENDIF (GEANT4_DIR AND GEANT4_INC_DIR AND GEANT4_LIB_DIR)
        
        IF (GEANT4_FOUND)
          SET(GEANT4_CONFIG_ERROR FALSE)
          
          INCLUDE_DIRECTORIES(${GEANT4_INC_DIR})
          LINK_DIRECTORIES(${GEANT4_LIB_DIR})
          LINK_LIBRARIES(${GEANT4_LIBRARIES})
          
          IF(GEANT4_LIB_GRANULAR)
            MESSAGE(STATUS "    + Found granular libraries at ${GEANT4_LIB_DIR}")
          ELSE(GEANT4_LIB_GRANULAR)
            MESSAGE(STATUS "    + Found compound libraries at ${GEANT4_LIB_DIR}")
          ENDIF(GEANT4_LIB_GRANULAR)
          
        ENDIF (GEANT4_FOUND)
      ENDIF(NOT ${GEANT4_LIB_DIR} MATCHES ".*NOTFOUND$")
    ENDIF(NOT ${GEANT4_INC_DIR} MATCHES ".*NOTFOUND$")
  ENDIF(NOT ${CLHEP_BASE_DIR} MATCHES ".*NOTFOUND$")
ENDIF(NOT ${GEANT4_DIR} MATCHES ".*NOTFOUND$")

IF (GEANT4_FOUND)
  MESSAGE(STATUS "    Looking for Geant4 -- found")
ELSE (GEANT4_FOUND)
  MESSAGE(STATUS "    Looking for Geant4 -- not found")
ENDIF (GEANT4_FOUND)
