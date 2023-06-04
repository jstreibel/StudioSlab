
set ( APP_SOURCES CrashPad.cpp AppBase.cpp)

set ( LIB_LOCAL_NAME ${STUDIOS_LIB_BASE_NAME}-App )

add_library(${LIB_LOCAL_NAME} STATIC ${APP_SOURCES})

target_link_libraries(${LIB_LOCAL_NAME} Slab-Utils Slab-Base-Controller )

message(NOTICE "Added library " ${LIB_LOCAL_NAME})

