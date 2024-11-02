cmake_minimum_required(VERSION 2.8)

project(main)
set(EXECUTABLE_NAME "main")
add_executable(${PROJECT_NAME} "main.cpp" "ray.cpp" "block-intersector.cpp"
                               "player.cpp" "raycaster.cpp")

#set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2.5 COMPONENTS system window graphics network audio)
if (SFML_FOUND)
    message("SFML was found! Hahahaha")
    message("Libraries (auto): ${SFML_LIBRARIES}")
    message("Includes (auto): ${SFML_INCLUDES}")

    if (NOT SFML_LIBRARIES)
        message("Bad, man...")
        set (SFML_LIBRARIES sfml-system sfml-window sfml-graphics)

    endif()

endif()

target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES})

