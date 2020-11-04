set(SOURCE_FILES
        verifier.h
        verifier.cpp
        parse_tree.h
        parse_tree.cpp
        parser.h
        parser.cpp
        reducer.h
        reducer.cpp
        operators.h
        operators.cpp)

add_library(utils SHARED STATIC ${SOURCE_FILES})