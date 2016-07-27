#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/AbstractPool.o \
	${OBJECTDIR}/src/Buffer.o \
	${OBJECTDIR}/src/BufferPool.o \
	${OBJECTDIR}/src/Connection.o \
	${OBJECTDIR}/src/DeserializeData.o \
	${OBJECTDIR}/src/ErrorCodes.o \
	${OBJECTDIR}/src/IncomingPacket.o \
	${OBJECTDIR}/src/MemorySegment.o \
	${OBJECTDIR}/src/MessageFactory.o \
	${OBJECTDIR}/src/OutgoingPacket.o \
	${OBJECTDIR}/src/Protocol.o \
	${OBJECTDIR}/src/ProtocolContext.o \
	${OBJECTDIR}/src/ProtocolFramework.o \
	${OBJECTDIR}/src/ProtocolManager.o \
	${OBJECTDIR}/src/RecyclableBuffer.o \
	${OBJECTDIR}/src/SerializeData.o \
	${OBJECTDIR}/src/linuxcs.o \
	${OBJECTDIR}/src/stdafx.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk /home/ChessComposer/${CND_CONF}/libprotocolframework.so

/home/ChessComposer/${CND_CONF}/libprotocolframework.so: ${OBJECTFILES}
	${MKDIR} -p /home/ChessComposer/${CND_CONF}
	${LINK.cc} -o /home/ChessComposer/${CND_CONF}/libprotocolframework.so ${OBJECTFILES} ${LDLIBSOPTIONS} -shared -fPIC

${OBJECTDIR}/src/AbstractPool.o: src/AbstractPool.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/AbstractPool.o src/AbstractPool.cpp

${OBJECTDIR}/src/Buffer.o: src/Buffer.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Buffer.o src/Buffer.cpp

${OBJECTDIR}/src/BufferPool.o: src/BufferPool.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/BufferPool.o src/BufferPool.cpp

${OBJECTDIR}/src/Connection.o: src/Connection.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Connection.o src/Connection.cpp

${OBJECTDIR}/src/DeserializeData.o: src/DeserializeData.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/DeserializeData.o src/DeserializeData.cpp

${OBJECTDIR}/src/ErrorCodes.o: src/ErrorCodes.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ErrorCodes.o src/ErrorCodes.cpp

${OBJECTDIR}/src/IncomingPacket.o: src/IncomingPacket.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/IncomingPacket.o src/IncomingPacket.cpp

${OBJECTDIR}/src/MemorySegment.o: src/MemorySegment.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MemorySegment.o src/MemorySegment.cpp

${OBJECTDIR}/src/MessageFactory.o: src/MessageFactory.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MessageFactory.o src/MessageFactory.cpp

${OBJECTDIR}/src/OutgoingPacket.o: src/OutgoingPacket.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/OutgoingPacket.o src/OutgoingPacket.cpp

${OBJECTDIR}/src/Protocol.o: src/Protocol.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Protocol.o src/Protocol.cpp

${OBJECTDIR}/src/ProtocolContext.o: src/ProtocolContext.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ProtocolContext.o src/ProtocolContext.cpp

${OBJECTDIR}/src/ProtocolFramework.o: src/ProtocolFramework.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ProtocolFramework.o src/ProtocolFramework.cpp

${OBJECTDIR}/src/ProtocolManager.o: src/ProtocolManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ProtocolManager.o src/ProtocolManager.cpp

${OBJECTDIR}/src/RecyclableBuffer.o: src/RecyclableBuffer.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/RecyclableBuffer.o src/RecyclableBuffer.cpp

${OBJECTDIR}/src/SerializeData.o: src/SerializeData.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/SerializeData.o src/SerializeData.cpp

${OBJECTDIR}/src/linuxcs.o: src/linuxcs.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/linuxcs.o src/linuxcs.cpp

${OBJECTDIR}/src/stdafx.o: src/stdafx.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/stdafx.o src/stdafx.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} /home/ChessComposer/${CND_CONF}/libprotocolframework.so

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
