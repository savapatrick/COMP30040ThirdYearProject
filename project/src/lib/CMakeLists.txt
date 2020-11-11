#set(SOURCE_FILES
#        entity.h
#        entity.cpp
#        verifier.h
#        verifier.cpp
#        parse_tree.h
#        parse_tree.cpp
#        reducer.h
#        reducer.cpp
#        operators.h
#        operators.cpp
#        tokenizer.h
#        tokenizer.cpp
#        literal.h
#        literal.cpp)
# TODO: consider to replace the above or not

file(GLOB_RECURSE SOURCE_FILES LIST_DIRECTORIES true *.h *.cpp)

set(SOURCES ${SOURCE_FILES})

add_library(utils SHARED STATIC ${SOURCE_FILES})