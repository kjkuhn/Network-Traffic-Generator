getSources(sources "sources.txt")
prependPath(sources)

#string(CONCAT my_sources ${my_sources} " " ${sources})
message(STATUS "${sources} added to executable")
set(my_sources ${sources} PARENT_SCOPE)

#add_library(noise ${sources})

#string(CONCAT my_libraries ${my_libraries} " ${CMAKE_CURRENT_SOURCE_DIR}/noise")
#message(STATUS "Library noise added to build")
