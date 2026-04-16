set(BANKING_SOURCES
	Account.cpp
	Transaction.cpp
)

add_library(banking STATIC ${BANKING_SOURCES})

target_include_directories(banking PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})