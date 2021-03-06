PROJECT(sequential)

CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")
SET(SEQUENTIAL_STATIC TRUE CACHE BOOL "Enable static build of Sequential.")

SET(SEQUENTIAL_SOURCE_FILES
	"src/seq/seq-api.c"
	"src/seq/seq-list.c"
	# "src/seq/seq-map.c"
)

SET(SEQUENTIAL_HEADER_FILES
	"src/sequential.h"
	"src/seq/seq-api.h"
)

INCLUDE_DIRECTORIES(src)

SOURCE_GROUP("Header Files" FILES ${SEQUENTIAL_HEADER_FILES})
SOURCE_GROUP("Source Files" FILES ${SEQUENTIAL_SOURCE_FILES})

SET(SEQUENTIAL_LIBRARY_TYPE SHARED)

IF(SEQUENTIAL_STATIC)
	SET(SEQUENTIAL_LIBRARY_TYPE STATIC)
ENDIF()

IF(NOT WIN32)
	SET(CMAKE_C_FLAGS "-W -Wall -Wno-unused-parameter -std=c90 -pedantic")
ENDIF(NOT WIN32)

ADD_LIBRARY(sequential ${SEQUENTIAL_LIBRARY_TYPE}
	${SEQUENTIAL_HEADER_FILES}
	${SEQUENTIAL_SOURCE_FILES}
)

IF(NOT WIN32)
	TARGET_LINK_LIBRARIES(sequential dl)
ENDIF()

# ADD_EXECUTABLE(seq-test "test/seq-test.h" "test/seq-test.c")
# TARGET_LINK_LIBRARIES(seq-test sequential)

ADD_EXECUTABLE(seq-tmp "test/seq-tmp.c")
TARGET_LINK_LIBRARIES(seq-tmp sequential)

ADD_EXECUTABLE(seq-test-string "test/seq-test-string.c")
TARGET_LINK_LIBRARIES(seq-test-string sequential)
