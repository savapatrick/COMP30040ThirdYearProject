set(SOURCE_FILES
        verifier.h
        verifier.cpp
        evaluator.h
        evaluator.cpp
        parser.h
        parser.cpp
        reducer.h
        reducer.cpp)

add_library(utils SHARED STATIC ${SOURCE_FILES})