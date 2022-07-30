default_target: all
.PHONY: all precision benchmarking errors
all: precision benchmarking errors

CXXFLAGS = -std=c++11 -I./src/ -Wall

precision:
	@echo Compiling \"precision\" example program ...
	@g++ examples/precision.cpp ${CXXFLAGS} -o ./precision

benchmarking:
	@echo Compiling \"benchmarking\" example program ...
	@g++ examples/benchmarking.cpp ${CXXFLAGS} -o ./benchmarking

errors:
	@echo Compiling \"errors\" example program ...
	@g++ examples/errors.cpp ${CXXFLAGS} -o ./errors
