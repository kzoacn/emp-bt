cmake_minimum_required (VERSION 2.8.11)
project (emp-arith)
set(NAME "emp-arith")
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	if(NOT DEFINED OPENSSL_ROOT_DIR)
		set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl")	
		message(STATUS "OPENSSL_ROOT_DIR set to default: ${OPENSSL_ROOT_DIR}")
	endif()
endif()

find_path(CMAKE_FOLDER NAMES cmake/emp-tool-config.cmake)
include(${CMAKE_FOLDER}/cmake/common.cmake)
include(${CMAKE_FOLDER}/cmake/source_of_randomness.cmake)
include(${CMAKE_FOLDER}/cmake/threading.cmake)

find_package(emp-ot REQUIRED)
include_directories(${EMP-OT_INCLUDE_DIRS})

set(sources
 emp-arith/emp-arith.cpp
)
add_library(${NAME} SHARED ${sources})

# Installation
install(FILES cmake/emp-arith-config.cmake DESTINATION cmake/)
install(DIRECTORY emp-arith DESTINATION include/)
install(TARGETS ${NAME} DESTINATION lib/)

# Test cases
macro (add_test _name)
  add_executable(${_name} "test/${_name}.cpp")
  target_link_libraries(${_name} ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES} ${GMP_LIBRARIES} ${NAME} ${EMP-OT_LIBRARIES})
endmacro()
 
add_test (simple) 
add_test (editdis) 
