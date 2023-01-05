# If you create new headers/cpp files, add them to these list!
set(HEADER_FILES
   SrcMain.h
   sha1.h
   HashTable.hpp
DictionaryAttack.h
BruteForce.h
)

set(SOURCE_FILES
   SrcMain.cpp
   sha1.cpp
DictionaryAttack.cpp
BruteForce.cpp
)

# Don't change this
add_library(src ${SOURCE_FILES} ${HEADER_FILES})
