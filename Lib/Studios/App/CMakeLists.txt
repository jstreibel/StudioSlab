
set ( APP_SOURCES SafetyNet.cpp	)

add_library(${STUDIOS_LIB_BASE_NAME}-App STATIC ${APP_SOURCES})

target_link_libraries(${STUDIOS_LIB_BASE_NAME}-App Slab-Common)
