cmake_minimum_required(VERSION 3.10)
project(MyProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCES
    main.cpp
    student.cpp
    tutor.cpp
    AutoAssigner.cpp
    system.cpp
)

add_executable(programa ${SOURCES})


add_executable(chat_server
    chat_server.cpp
)

set(CSV_FILES
    students.csv
    tutors.csv
    area.csv
)


foreach(csv ${CSV_FILES})
configure_file(${csv} ${csv} COPYONLY)
endforeach()
