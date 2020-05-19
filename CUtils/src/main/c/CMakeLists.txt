
#include in the build all the content inside the directory
include_directories("../include")
#you might want to add the sources via the following command: set(SOURCES src/mainapp.cpp src/Student.cpp)
#but with GLOB is all much easier; include in the build all the content filtered by the pattern
file(GLOB SOURCES "*.c")
file(GLOB HEADERS "../include/*.h")
#We add automatically generate sources
list(APPEND ${SOURCES} "cliParser.c" "cliLexer.c")
list(APPEND ${HEADERS} "../include/tokensDefinitions.h")

message("sources are ${SOURCES}")
message("headers are ${HEADERS}")

# Add Bison outputs

set(bisonOutput "src/main/include/tokensDefinitions.h" "src/main/c/cliParser.c")
add_custom_command(
	COMMAND bison --defines="../../src/main/include/tokensDefinitions.h" -o "../../src/main/c/cliParser.c" "../../src/main/resources/cli.y"
    OUTPUT ${bisonOutput}
    COMMENT "Generate bison files"
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
)

#Add Flex outputs

set(flexOutput "src/main/c/cliLexer.c" "flexOutput")
add_custom_command(
	DEPENDS "src/main/include/tokensDefinitions.h" "src/main/c/cliParser.c"
	COMMAND flex -o "../../src/main/c/cliLexer.c" "../../src/main/resources/cli.l"
	OUTPUT ${flexOutput}
	COMMENT "Generate flex files"
	WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
)

############## ADD EXECUTABLE/LIBRARY ####################

if(${THEPROJECT_OUTPUT} STREQUAL "EXE")
    add_executable(${THEPROJECT_NAME} ${SOURCES} ${bisonOutput} ${flexOutput})
endif()

if(${THEPROJECT_OUTPUT} STREQUAL "SO")
    if(${THEPROJECT_POSITION_INDEPENDENT_CODE} STREQUAL "true")
        set(POSITION_INDEPENDENT_CODE True)
    else()
        set(POSITION_INDEPENDENT_CODE False)
    endif()
    
    add_library(${THEPROJECT_NAME} SHARED ${SOURCES} "flexOutput")   
endif()

if(${THEPROJECT_OUTPUT} STREQUAL "AO")   
     
    if(${THEPROJECT_POSITION_INDEPENDENT_CODE} STREQUAL "true")
        set(POSITION_INDEPENDENT_CODE True)
    else(${THEPROJECT_POSITION_INDEPENDENT_CODE} STREQUAL "true")
        set(POSITION_INDEPENDENT_CODE False)
    endif(${THEPROJECT_POSITION_INDEPENDENT_CODE} STREQUAL "true")
     
    add_library(${THEPROJECT_NAME} STATIC ${SOURCES})
endif()

target_link_libraries(${THEPROJECT_NAME} ${THEPROJECT_REQUIRED_SHARED_LIBRARIES})
set_target_properties(${THEPROJECT_NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    VERSION ${THEPROJECT_VERSION}
)

#copy the contents of src/main/resources inside build/XXX
add_custom_command(
    TARGET ${THEPROJECT_NAME}
    POST_BUILD COMMAND
    ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/../../src/main/resources $<TARGET_FILE_DIR:${THEPROJECT_NAME}>
)

add_custom_command(TARGET ${THEPROJECT_NAME}
	DEPENDS ${bisonOutput} ${flexOutput}
	COMMENT "generate CLI CUtils API"
)

#************** SUDO MAKE INSTALL ****************

#include new cmake variables representing GNU default installation locations
include(GNUInstallDirs)

if(${THEPROJECT_OUTPUT} STREQUAL "EXE")
    install(TARGETS ${THEPROJECT_NAME} DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})
endif()

if(${THEPROJECT_OUTPUT} STREQUAL "SO")
    #when user do "make install" this line will be used. Determine where the library will be placed
    install(TARGETS ${THEPROJECT_NAME} DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})
    install(FILES ${HEADERS} DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/${THEPROJECT_NAME}")
    
    #run ldconfig to update the cache with the new installed library. We use a cache different from the one in /etc/ld.so.cache
    #because we might want to install our software without root access
    install(CODE "execute_process(COMMAND ldconfig -n -C ld.so.cache)")
endif()

if (${THEPROJECT_OUTPUT} STREQUAL "AO")
    #when user do "make install" this line will be used. Determine where the library will be placed
    install(TARGETS ${THEPROJECT_NAME} DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})
    install(FILES ${HEADERS} DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/${THEPROJECT_NAME}")
endif ()

# ******************** SUDO MAKE UNINSTALL ********************* 
add_custom_target(uninstall
    COMMAND xargs rm -fv < install_manifest.txt
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}" 
    DEPENDS "${CMAKE_BINARY_DIR}/install_manifest.txt"
    COMMENT "Removes everything installed by sudo make install"
    VERBATIM
)
