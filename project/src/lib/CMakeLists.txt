set(SOURCE_FILES
        verifier.h
        verifier.cpp
        modifier.h
        modifier.cpp
        parse_tree.h
        parse_tree.cpp
        parser.h
        parser.cpp
        reducer.h
        reducer.cpp
        operators.h
        operators.cpp
        tokenizer.h
        tokenizer.cpp)

add_library(utils SHARED STATIC ${SOURCE_FILES})