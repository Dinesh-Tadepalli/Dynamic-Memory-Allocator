###############################################################################
# @file: Makefile
#
# Simple makefile for project maintanence, usage, and testing 
#
# @creator: 7/15/22 Bujji Setty (selagamsetty@wisc.edu)
# @modified: 7/25/22 Bujji Setty (selagamsetty@wisc.edu)
#
###############################################################################

DRIVERDIR = drivers/

all: build

build: inc/header.h src/mem.c drivers/*.c
	@for file in $(shell ls ${DRIVERDIR}) ; do \
	    gcc -g -Iinc/ -o $${file}.out utils/*.c src/*.c ${DRIVERDIR}/$${file} -m64 -Wall; \
     done
    
clean: 
	@rm -f *.out

# Feel free to add any additional commands that may make testing easier. Our 
# grading scripts will be using the version of this file that was provided to 
# you when the project was released. 
#         file = $${file//.c/} ; \

