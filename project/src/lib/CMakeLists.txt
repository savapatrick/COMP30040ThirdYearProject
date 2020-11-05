set(SOURCE_FILES
        verifier.h
        verifier.cpp
        parse_tree.h
        parse_tree.cpp
        reducer.h
        reducer.cpp
        operators.h
        operators.cpp
        tokenizer.h
        tokenizer.cpp)

add_library(utils SHARED STATIC ${SOURCE_FILES})