
set ( APP_SOURCES CrashPad.cpp AppBase.cpp)

add_library(${STUDIOS_LIB_BASE_NAME}-App STATIC ${APP_SOURCES})

target_link_libraries(${STUDIOS_LIB_BASE_NAME}-App Slab-Common Slab-Studios-Controller )
