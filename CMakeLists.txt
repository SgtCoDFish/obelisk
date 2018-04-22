cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(obelisk VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 14)

message("Configuring ${PROJECT_NAME}-${PROJECT_VERSION}")

if (CMAKE_BUILD_TYPE MATCHES Release)
    set(OBELISK_DEBUG FALSE)
else ()
    set(OBELISK_DEBUG TRUE)
endif ()

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

if (EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS_BASE "${CMAKE_CXX_FLAGS_BASE} -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='[\"png\"]' -s USE_SDL_NET=2 -s USE_SDL_TTF=2")
    set(${SDL2_LIBRARY} "-s USE_SDL=2")
    set(${SDL2_TTF_LIBRARIES} "-s USE_SDL_TTF=2")
    set(${SDL2_IMAGE_LIBRARY} "-s USE_SDL_IMAGE=2")
    # set(${SDL2_MIXER_LIBRARY} "-s USE_SDL_MIXER=2")
    set(${SDL2_MIXER_LIBRARY} "")
    set(${SDL2_NET_LIBRARY} "-s USE_SDL_NET=1")
    set(${SDL2_INCLUDE_DIR} "")
    set(${SDL2_TTF_INCLUDE_DIR} "")
    set(${SDL2_IMAGE_INCLUDE_DIR} "")
    set(${SDL2_MIXER_INCLUDE_DIR} "")
    set(${SDL2_NET_INCLUDE_DIR} "")
else ()
    find_package(SDL2 REQUIRED)
    find_package(SDL2_image REQUIRED)
    find_package(SDL2_mixer REQUIRED)
    find_package(SDL2_ttf REQUIRED)
    find_package(SDL2_net REQUIRED)
endif ()

if (EMSCRIPTEN)
    message("Skipping finding OpenGL for emscripten")
    set(CMAKE_CXX_FLAGS_BASE "${CMAKE_CXX_FLAGS_BASE} -s USE_WEBGL2=1")
else ()
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
endif ()

set(PROJECT_VENDOR_DIR "${PROJECT_SOURCE_DIR}/vendor")
if (EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS_BASE "${CMAKE_CXX_FLAGS_BASE} -s USE_ZLIB=1")
    set(ZLIB_LIBRARIES "-s USE_ZLIB=1")
    set(ZLIB_INCLUDE_DIRS "")
else ()
    find_package(ZLIB REQUIRED)
endif ()

set(VENDOR_TINYXML2_FILES ${PROJECT_VENDOR_DIR}/apg/vendor/tinyxml2/tinyxml2.cpp)
set(VENDOR_TINYXML2_INCLUDE_DIRS ${PROJECT_VENDOR_DIR}/apg/vendor/tinyxml2)

configure_file(${PROJECT_VENDOR_DIR}/apg/vendor/tmxparser/src/Tmx.h.in ${PROJECT_BINARY_DIR}/Tmx.h)
file(GLOB_RECURSE VENDOR_TMXPARSER_SOURCES ${PROJECT_VENDOR_DIR}/apg/vendor/tmxparser/src/*.cpp)
file(GLOB_RECURSE VENDOR_TMXPARSER_HEADERS ${PROJECT_VENDOR_DIR}/apg/vendor/tmxparser/src/*.h ${PROJECT_BINARY_DIR}/Tmx.h)
set(VENDOR_TMXPARSER_INCLUDE_DIRS ${PROJECT_VENDOR_DIR}/apg/vendor/tmxparser/src)

set(VENDOR_GLM_INCLUDE_DIRS ${PROJECT_VENDOR_DIR}/apg/vendor/glm)

set(VENDOR_RAPIDJSON_INCLUDE_DIRS ${PROJECT_VENDOR_DIR}/apg/vendor/rapidjson/include)

set(VENDOR_EASYLOGGINGPP_FILES
        "${PROJECT_VENDOR_DIR}/apg/vendor/easyloggingpp/src/easylogging++.cc"
        )
set(VENDOR_EASYLOGGINGPP_INCLUDE_DIRS ${PROJECT_VENDOR_DIR}/apg/vendor/easyloggingpp/src)

file(GLOB_RECURSE VENDOR_APG_FILES ${PROJECT_VENDOR_DIR}/apg/src/*.cpp)
set(VENDOR_APG_INCLUDE_DIRS ${PROJECT_VENDOR_DIR}/apg/include)

set(VENDOR_ASHLEYCPP_INCLUDE_DIRS ${PROJECT_VENDOR_DIR}/ashleycpp/include)
file(GLOB_RECURSE VENDOR_ASHLEYCPP_FILES ${PROJECT_VENDOR_DIR}/ashleycpp/src/*.cpp)

set(VENDOR_FILES
        ${VENDOR_APG_FILES}
        ${VENDOR_EASYLOGGINGPP_FILES}
        ${VENDOR_TMXPARSER_SOURCES}
        ${VENDOR_TINYXML2_FILES}
        ${VENDOR_ASHLEYCPP_FILES}
        )
set(VENDOR_INCLUDE_DIRS
        ${VENDOR_APG_INCLUDE_DIRS}
        ${VENDOR_TINYXML2_INCLUDE_DIRS}
        ${VENDOR_TMXPARSER_INCLUDE_DIRS}
        ${VENDOR_GLM_INCLUDE_DIRS}
        ${VENDOR_RAPIDJSON_INCLUDE_DIRS}
        ${VENDOR_EASYLOGGINGPP_INCLUDE_DIRS}
        ${VENDOR_ASHLEYCPP_INCLUDE_DIRS}
        )

set(OBELISK_HEADERS include/obelisk.hpp include/components/PositionComponent.hpp include/components/DeckComponent.hpp include/components/RenderableComponent.hpp include/systems/RenderSystem.hpp include/ObeliskCard.hpp include/components/TowerComponent.hpp include/systems/PlayerInputSystem.hpp include/components/ClickableComponent.hpp include/ObeliskState.hpp include/components/CarriedComponent.hpp include/components/CarryableComponent.hpp include/systems/CarrySystem.hpp include/components/DeathComponent.hpp include/systems/DeathSystem.hpp include/components/WalkerComponent.hpp include/components/ColorComponent.hpp include/ObeliskMap.hpp include/systems/EntitySpawnSystem.hpp include/systems/TowerAttackSystem.hpp include/systems/TowerUpgradeSystem.hpp include/components/TowerUpgradeComponent.hpp include/components/TrashComponent.hpp include/UpgradeType.hpp include/systems/ToastSystem.hpp include/components/ToastComponent.hpp)
set(OBELISK_SOURCES src/obelisk.cpp src/ObeliskMap.cpp src/components/PositionComponent.cpp src/components/RenderableComponent.cpp src/systems/RenderSystem.cpp src/ObeliskCard.cpp src/systems/PlayerInputSystem.cpp src/systems/CarrySystem.cpp src/systems/DeathSystem.cpp include/systems/WalkingSystem.hpp src/systems/WalkingSystem.cpp src/systems/EntitySpawnSystem.cpp src/systems/TowerAttackSystem.cpp src/systems/TowerUpgradeSystem.cpp src/systems/ToastSystem.cpp)

set(FULL_SOURCES ${OBELISK_SOURCES} ${OBELISK_HEADERS} ${VENDOR_FILES})
add_definitions(-DELPP_NO_LOG_TO_FILE)
add_definitions(-DELPP_NO_DEFAULT_LOG_FILE)
add_definitions(-DGLM_FORCE_RADIANS)

if (DEFINED WIN32)
    if (MSVC)
        set(OS_FLAGS "")
        set(COMPILER_FLAGS "/GR /EHsc")
        set(OS_LIBS "")

        # VS 2015 has a linker issue when using the official SDL2 libs, this should fix it
        if (MSVC_VERSION GREATER 1800)
            message("Using legacy STDIO fix for MSVC 15.")
            set(OS_LIBS ${OS_LIBS} legacy_stdio_definitions)
            set(EXTRA_SOURCES ${PROJECT_SOURCE_DIR}/MSVC/vs2015_io_fix.cpp include/components/PositionComponent.hpp src/ObeliskMap.cpp)
        endif ()
    else ()
        # MinGW
        set(OS_FLAGS "-mwindows")
        set(OS_LIBS Version Imm32 winmm vorbisfile ogg vorbis winsock32 mingw32)
    endif ()
elseif (DEFINED EMSCRIPTEN)
    set(OS_FLAGS "-s WASM=1")
    set(OS_LIBS "")
    add_definitions(-DELPP_OS_UNIX)
else ()
    set(OS_FLAGS "-fPIC")
    set(OS_LIBS "")
endif ()

if (NOT MSVC)
    set(COMPILER_FLAGS "-Wall -Wextra -Wno-unused-parameter -Wno-unused-variable ${OS_FLAGS}")
    set(OBELISK_DEBUG_FLAGS "-g3 -O0")
    set(OBELISK_RELEASE_FLAGS "-g0 -O3")
else ()
    set(OBELISK_DEBUG_FLAGS "/Od /Gs")
    set(OBELISK_RELEASE_FLAGS "/Ox")
endif ()

set(CMAKE_CXX_FLAGS_BASE "${CMAKE_CXX_FLAGS_BASE} ${COMPILER_FLAGS}")
message("${CMAKE_CXX_FLAGS_BASE}")

include_directories(
        "include"
        ${PROJECT_BINARY_DIR}
        ${VENDOR_INCLUDE_DIRS}
        ${ZLIB_INCLUDE_DIRS}
        ${SDL2_INCLUDE_DIR}
        ${SDL2_TTF_INCLUDE_DIR}
        ${SDL2_IMAGE_INCLUDE_DIR}
        ${SDL2_MIXER_INCLUDE_DIR}
        ${SDL2_NET_INCLUDE_DIR}
        ${OPENGL_INCLUDE_DIR}
        ${GLEW_INCLUDE_DIRS}
)

if (OBELISK_DEBUG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_BASE} ${OBELISK_DEBUG_FLAGS}")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_BASE} ${OBELISK_RELEASE_FLAGS}")
endif ()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${AUDIOTEST_FLAGS}")
set(PROJECT_LIBS ${SDL2_LIBRARY}
        ${SDL2_TTF_LIBRARIES}
        ${SDL2_IMAGE_LIBRARY}
        ${SDL2_MIXER_LIBRARY}
        ${SDL2_NET_LIBRARY}
        ${GLEW_LIBRARIES}
        ${OPENGL_LIBRARIES}
        ${TINYXML2_LIBRARIES}
        ${OS_LIBS}
        ${TMXPARSER_LIBRARY}
        ${ZLIB_LIBRARIES}
        )

if (EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif ()

set(BASE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS})

if (EMSCRIPTEN)
    set(BASE_LINKER_FLAGS "${BASE_LINKER_FLAGS} -s ALLOW_MEMORY_GROWTH=1 -O2")
endif ()

if (EMSCRIPTEN)
    set(CMAKE_EXE_LINKER_FLAGS "${BASE_LINKER_FLAGS} --embed-file assets/towerDefense_tilesheet.png assets/map.tmx")
endif ()

file(GLOB_RECURSE OBELISK_ASSETS ${PROJECT_SOURCE_DIR}/assets/*)
file(COPY ${OBELISK_ASSETS} DESTINATION assets)

add_executable(${PROJECT_NAME} ${FULL_SOURCES} ${EXTRA_SOURCES})
target_link_libraries(${PROJECT_NAME} ${PROJECT_LIBS})
