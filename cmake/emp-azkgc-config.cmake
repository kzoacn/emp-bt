find_package(emp-ot)

find_path(emp-azkgc_INCLUDE_DIR emp-azkgc/emp-azkgc.h)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(emp-azkgc DEFAULT_MSG emp-azkgc_INCLUDE_DIR)

find_library(EMP-AZKGC_LIBRARY NAMES emp-azkgc)

if(emp-azkgc_FOUND)
	set(emp-azkgc_INCLUDE_DIRS ${emp-azkgc_INCLUDE_DIR}/include/ ${EMP-OT_INCLUDE_DIRS})
	set(EMP-AZKGC_LIBRARIES ${EMP-OT_LIBRARIES} ${EMP-AZKGC_LIBRARY}$)
endif()
