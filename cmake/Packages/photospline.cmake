find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        if(NOT EXISTS "${PROJECT_SOURCE_DIR}/vendor/photospline/CMakeLists.txt")
            message(STATUS "Submodule update")
            execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init ./
                            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/vendor/photospline
                            RESULT_VARIABLE GIT_SUBMOD_RESULT)
            if(NOT GIT_SUBMOD_RESULT EQUAL "0")
                message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
            endif()
        endif()
    endif()
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/vendor/photospline/CMakeLists.txt")
    message(FATAL_ERROR "The photospline submodule was not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

add_subdirectory(${PROJECT_SOURCE_DIR}/vendor/photospline EXCLUDE_FROM_ALL)
if(DEFINED SKBUILD)
    set_target_properties(photospline PROPERTIES
            BUILD_WITH_INSTALL_RPATH FALSE
            LINK_FLAGS "-Wl,-rpath,$ORIGIN")
    install(TARGETS photospline
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/leptoninjector.libs
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
    #set_target_properties(spglam PROPERTIES
    #        BUILD_WITH_INSTALL_RPATH FALSE
    #        LINK_FLAGS "-Wl,-rpath,$ORIGIN")
    #install(TARGETS spglam
    #    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/leptoninjector.libs
    #    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
else()
    install(TARGETS photospline DESTINATION ${CMAKE_INSTALL_LIBDIR})
    #install(TARGETS spglam DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()
