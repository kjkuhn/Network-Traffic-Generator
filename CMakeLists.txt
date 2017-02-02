cmake_minimum_required(VERSION 2.6)

### functions ###

function(getSources dest fileName)
	file(READ ${fileName} fileContent)
	string(REGEX REPLACE "#.*\n" "" fileContent ${fileContent})
	string(REGEX REPLACE " |\n$" "" fileContent ${fileContent})
	string(REGEX REPLACE "\n" " " fileContent ${fileContent})
	set(${dest} ${fileContent} PARENT_SCOPE)
endfunction(getSources)

function(prependPath src)
	string(REGEX REPLACE " " ";" lst ${${src}})
	foreach(x ${lst})
		list(APPEND tmp ${CMAKE_CURRENT_SOURCE_DIR}/${x})
	endforeach(x)
	#string(REGEX REPLACE ";" " " tmp "${tmp}")
	set(${src} ${tmp} PARENT_SCOPE)
endfunction(prependPath)

### build ###

project(nwn)


include_directories(include)

set(my_libraries "")
set(my_sources "")

add_subdirectory(src)

message("srcs:${my_sources}")

add_executable(${PROJECT_NAME} ${my_sources})
#add_executable(nwn src/main.c src/sniffer.c src/sender.c)
target_link_libraries(${PROJECT_NAME} 
pthread
${my_libraries}
)
