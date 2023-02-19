set(library_project_name jsonator)

find_program(PYTHON3 "python3")
find_program(CLANG_FORMAT "clang-format")
if(PYTHON3 AND CLANG_FORMAT)
    get_target_property(library_source_files "${library_project_name}" SOURCES)
    add_custom_command(OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/../single_include/mblet/jsonator.h"
        COMMAND ${PYTHON3} ./etc/script/amalgamate.py -c ./etc/script/config_jsonator.json -s . > /dev/null
        COMMAND ${CLANG_FORMAT} -i ./single_include/mblet/jsonator.h
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..
        DEPENDS
            ${library_source_files}
            "${CMAKE_CURRENT_SOURCE_DIR}/../include/mblet/jsonator.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/../etc/script/config_jsonator.json"
        COMMENT "Generating single_include/mblet/jsonator.h"
    )
    add_custom_target("${library_project_name}_single_include"
        DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/../single_include/mblet/jsonator.h"
    )
    if(BUILD_TESTING)
        add_subdirectory(test)
    endif()
    add_dependencies("${library_project_name}" "${library_project_name}_single_include")
endif()