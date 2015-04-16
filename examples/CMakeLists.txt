# for g++ and clang++
IF ( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" )
    IF ( CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.7.0" )
	   ADD_DEFINITIONS ( -std=c++0x )
    ELSE ()
        ADD_DEFINITIONS ( -std=c++11 )
    ENDIF ()
ELSEIF ( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" )
    # will have to adjust this... older clang versions may need -std=c++0x
    # but I could not find an exact version number :-/
    ADD_DEFINITIONS ( -std=c++11 )
ENDIF ()

INCLUDE_DIRECTORIES ( ../include )

ADD_EXECUTABLE ( create_patch create_patch.cxx )
TARGET_LINK_LIBRARIES ( create_patch xdiff )

ADD_EXECUTABLE ( apply_patch apply_patch.cxx )
TARGET_LINK_LIBRARIES ( apply_patch xdiff )
