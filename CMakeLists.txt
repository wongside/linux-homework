
cmake_minimum_required(VERSION 3.10)

project(linux_homework)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(test)

aux_source_directory(thread_related thread_related_source_file)

aux_source_directory(file_manager file_manager_source_file)

aux_source_directory(data_table data_table_source_file)

aux_source_directory(data_field data_field_source_file)

aux_source_directory(b_plus_index b_plus_index_source_file)

add_library(linux_homework
        ${thread_related_source_file}
        ${file_manager_source_file}
        ${data_table_source_file}
        ${data_field_source_file}
        ${b_plus_index_source_file})

target_link_libraries(linux_homework pthread)