find_package(emp-ot)

find_path(emp-bt_INCLUDE_DIR emp-bt/emp-bt.h)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(emp-bt DEFAULT_MSG emp-bt_INCLUDE_DIR)

find_library(emp-bt_LIBRARY NAMES emp-bt)

if(emp-bt_FOUND)
	set(emp-bt_INCLUDE_DIRS ${emp-bt_INCLUDE_DIR}/include/ ${EMP-OT_INCLUDE_DIRS})
	set(emp-bt_LIBRARIES ${EMP-OT_LIBRARIES} ${emp-bt_LIBRARY}$)
endif()
