# If you create new headers/cpp files, add them to these list!
set(HEADER_FILES
	SrcMain.h
	ReadFasta.h
	AAStateMachine.h
	NeedlemanWunsch.h
)

set(SOURCE_FILES
	SrcMain.cpp
	ReadFasta.cpp
	AAStateMachine.cpp
	NeedlemanWunsch.cpp
)

# Don't change this
add_library(src ${SOURCE_FILES} ${HEADER_FILES})
