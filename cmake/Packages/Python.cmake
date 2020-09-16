IF(CMAKE_VERSION VERSION_LESS 3.12.0)
    IF(Python_ROOT_DIR)
        MESSAGE(WARNING "Python_ROOT_DIR is set but will be ignored by this version of CMake; set PYTHON_EXECUTABLE instead")
    ENDIF(Python_ROOT_DIR)
    find_package(PythonInterp)
    find_package(PythonLibs)
    set(PYTHON_FOUND PYTHONLIBS_FOUND AND PYTHONINTERP_FOUND)
ELSE()
    IF(PYTHON_EXECUTABLE)
        MESSAGE(WARNING "PYTHON_EXECUTABLE is set but will be ignored by this version of CMake; set Python_ROOT_DIR instead")
    ENDIF(PYTHON_EXECUTABLE)
    FIND_PACKAGE(Python COMPONENTS Interpreter Development)
    SET(PYTHON_FOUND "${Python_FOUND}")
    SET(PYTHON_EXECUTABLE "${Python_EXECUTABLE}")
    SET(PYTHON_INCLUDE_DIRS "${Python_INCLUDE_DIRS}")
    SET(PYTHON_LIBRARIES "${Python_LIBRARIES}")
ENDIF()

IF(PYTHON_FOUND)
    # Search for numpy
    EXECUTE_PROCESS(COMMAND ${PYTHON_EXECUTABLE} -c "import numpy; print(numpy.__version__)"
      RESULT_VARIABLE NUMPY_FOUND OUTPUT_VARIABLE NUMPY_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
#    IF(NUMPY_FOUND EQUAL 0 AND NOT NUMPY_VERSION VERSION_LESS 1.7)
#      SET(NUMPY_FOUND TRUE)
#    ELSEIF(NUMPY_VERSION VERSION_LESS 1.7)
#      SET(NUMPY_FOUND FALSE)
#      MESSAGE (STATUS "  x numpy:    (version ${NUMPY_VERSION} < 1.7 is too old)")
    IF(NUMPY_FOUND EQUAL 0)
      SET(NUMPY_FOUND TRUE)
    ELSE()
      SET(NUMPY_FOUND FALSE)
    ENDIF()
  
    IF(NUMPY_FOUND)
      SET(NUMPY_FOUND TRUE CACHE BOOL "Numpy found successfully" FORCE)
      EXECUTE_PROCESS(COMMAND ${PYTHON_EXECUTABLE} -c
        "import numpy; print(numpy.get_include())"
        OUTPUT_VARIABLE NUMPY_INCLUDE_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE)
      SET(NUMPY_INCLUDE_DIR ${NUMPY_INCLUDE_DIR} CACHE STRING "Numpy directory")
      STRING(REGEX REPLACE "^([0-9]+)\\.[0-9]+\\.[0-9]+.*" "\\1" NUMPY_VERSION_MAJOR "${NUMPY_VERSION}")
      STRING(REGEX REPLACE "^[0-9]+\\.([0-9]+)\\.[0-9]+.*" "\\1" NUMPY_VERSION_MINOR "${NUMPY_VERSION}")
      STRING(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" NUMPY_VERSION_PATCH "${NUMPY_VERSION}")
      MESSAGE(STATUS "  * numpy:    version ${NUMPY_VERSION}; headers ${NUMPY_INCLUDE_DIR}")
    ELSE()
      SET(NUMPY_FOUND FALSE CACHE BOOL "Numpy found successfully" FORCE)
    ENDIF()
ENDIF(PYTHON_FOUND)