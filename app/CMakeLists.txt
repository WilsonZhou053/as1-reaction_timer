add_executable(reaction_timer
    src/main.c
    src/app_time.c
)

target_include_directories(reaction_timer
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(reaction_timer PRIVATE hal Threads::Threads)


set(MYAPPS_DIR "$ENV{HOME}/ensc351/public/myApps")
if(CMAKE_CROSSCOMPILING AND EXISTS "$ENV{HOME}/ensc351/public")
    add_custom_command(TARGET reaction_timer POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${MYAPPS_DIR}"
        COMMAND "${CMAKE_COMMAND}" -E copy
                "$<TARGET_FILE:reaction_timer>"
                "${MYAPPS_DIR}/reaction_timer"
        COMMENT "Copying reaction_timer to ${MYAPPS_DIR} for NFS access"
        VERBATIM
    )
elseif(NOT CMAKE_CROSSCOMPILING)
    message(STATUS "Native host build: not copying host executable to the NFS target directory")
else()
    message(STATUS "NFS output directory not found; executable remains at: ${CMAKE_CURRENT_BINARY_DIR}/reaction_timer")
endif()
