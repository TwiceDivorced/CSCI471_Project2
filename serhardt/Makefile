#
# Makefile
# Computer Networking Programing Assignments
#
#  Created by Phillip Romig on 4/3/12.
#  Copyright 2012 Colorado School of Mines. All rights reserved.
#
USERNAME = serhardt

CXX = g++
LD = g++
CXXFLAGS = -std=c++17 -g
LDFLAGS = 

#
# You should be able to add object files here without changing anything else
#
TARGET = rft-client
OBJ_FILES =  datagram.o rft-client.o timerC.o unreliableTransport.o
INC_FILES = datagram.h logging.h timerC.h unreliableTransport.h

#
# Any libraries we might need.
#
LIBRARYS = 

${TARGET}: ${OBJ_FILES}
	${LD} ${LDFLAGS} ${OBJ_FILES} -o $@ ${LIBRARYS}

%.o : %.cpp ${INC_FILES}
	${CXX} -c ${CXXFLAGS} -o $@ $<

#
# Please remember not to submit objects or binarys.
#
clean:
	rm -f core ${TARGET} ${OBJ_FILES}

#
# This might work to create the submission tarball in the formal I asked for.
#
submit:
	rm -f core project1 ${OBJ_FILES}
	mkdir ${USERNAME} 
	mkdir ${USERNAME}/data
	cp Makefile README.txt *.h *.cpp ${USERNAME} 
	cp data/* ${USERNAME}/data
	tar zcf ${USERNAME}.tgz ${USERNAME} 
