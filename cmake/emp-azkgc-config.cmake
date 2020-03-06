find_package(emp-ot)

find_path(emp-arith_INCLUDE_DIR emp-arith/emp-arith.h)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(emp-arith DEFAULT_MSG emp-arith_INCLUDE_DIR)

find_library(EMP-arith_LIBRARY NAMES emp-arith)

if(emp-arith_FOUND)
	set(emp-arith_INCLUDE_DIRS ${emp-arith_INCLUDE_DIR}/include/ ${EMP-OT_INCLUDE_DIRS})
	set(EMP-arith_LIBRARIES ${EMP-OT_LIBRARIES} ${EMP-arith_LIBRARY}$)
endif()
