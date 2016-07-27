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
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/private/Acceptor.o \
	${OBJECTDIR}/private/BroadcastManager.o \
	${OBJECTDIR}/private/BroadcastManagerBase.o \
	${OBJECTDIR}/private/BroadcastQueue.o \
	${OBJECTDIR}/private/BroadcastQueueContext.o \
	${OBJECTDIR}/private/BroadcastQueueGroupContext.o \
	${OBJECTDIR}/private/BroadcastStreamer.o \
	${OBJECTDIR}/private/ClientFactory.o \
	${OBJECTDIR}/private/ClientFactory_Linux.o \
	${OBJECTDIR}/private/Common.o \
	${OBJECTDIR}/private/ConnectionContext.o \
	${OBJECTDIR}/private/ConnectionContextPool.o \
	${OBJECTDIR}/private/CumulativeMeasure.o \
	${OBJECTDIR}/private/Demultiplexor.o \
	${OBJECTDIR}/private/DemuxImpl_Linux.o \
	${OBJECTDIR}/private/Dispatcher.o \
	${OBJECTDIR}/private/DistributionMeasure.o \
	${OBJECTDIR}/private/DurationMeasure.o \
	${OBJECTDIR}/private/GarbageCollector.o \
	${OBJECTDIR}/private/IOQueue_Linux.o \
	${OBJECTDIR}/private/KeyedAveragedDistributionMeasure.o \
	${OBJECTDIR}/private/KeyedAveragedMeasure.o \
	${OBJECTDIR}/private/Listener.o \
	${OBJECTDIR}/private/ListenerImpl_Linux.o \
	${OBJECTDIR}/private/ListenerOptions.o \
	${OBJECTDIR}/private/LogicalConnection.o \
	${OBJECTDIR}/private/LogicalConnectionImpl.o \
	${OBJECTDIR}/private/LogicalConnectionPool.o \
	${OBJECTDIR}/private/Measure.o \
	${OBJECTDIR}/private/MeasureArgs.o \
	${OBJECTDIR}/private/MonitorAcceptor.o \
	${OBJECTDIR}/private/MonitorConnection.o \
	${OBJECTDIR}/private/MonitorsBroadcastManager.o \
	${OBJECTDIR}/private/MonitorsMsgFactory.o \
	${OBJECTDIR}/private/PacketInfo.o \
	${OBJECTDIR}/private/PhysicalConnection.o \
	${OBJECTDIR}/private/PhysicalConnectionPool.o \
	${OBJECTDIR}/private/PhysicalConnection_Linux.o \
	${OBJECTDIR}/private/ProtocolManager.o \
	${OBJECTDIR}/private/QueueOptions.o \
	${OBJECTDIR}/private/Routines_Linux.o \
	${OBJECTDIR}/private/Server.o \
	${OBJECTDIR}/private/ServerImpl.o \
	${OBJECTDIR}/private/ServerOptions.o \
	${OBJECTDIR}/private/ServerStats.o \
	${OBJECTDIR}/private/Service.o \
	${OBJECTDIR}/private/StopWatch.o \
	${OBJECTDIR}/private/StopWatchImpl_Linux.o \
	${OBJECTDIR}/private/SystemService.o \
	${OBJECTDIR}/private/SystemServiceImpl_Linux.o \
	${OBJECTDIR}/private/Utilities_Linux.o \
	${OBJECTDIR}/private/XMLPacket.o \
	${OBJECTDIR}/private/XMLProtocol.o \
	${OBJECTDIR}/private/dllmain.o \
	${OBJECTDIR}/private/linuxcs.o \
	${OBJECTDIR}/private/stdafx.o \
	${OBJECTDIR}/private/xmlParser.o


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
LDLIBSOPTIONS=-lpthread -lrt

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ../output/libpushframework.so

../output/libpushframework.so: ${OBJECTFILES}
	${MKDIR} -p ../output
	${LINK.cc} -o ../output/libpushframework.so ${OBJECTFILES} ${LDLIBSOPTIONS} -shared -fPIC

${OBJECTDIR}/private/Acceptor.o: private/Acceptor.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/Acceptor.o private/Acceptor.cpp

${OBJECTDIR}/private/BroadcastManager.o: private/BroadcastManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/BroadcastManager.o private/BroadcastManager.cpp

${OBJECTDIR}/private/BroadcastManagerBase.o: private/BroadcastManagerBase.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/BroadcastManagerBase.o private/BroadcastManagerBase.cpp

${OBJECTDIR}/private/BroadcastQueue.o: private/BroadcastQueue.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/BroadcastQueue.o private/BroadcastQueue.cpp

${OBJECTDIR}/private/BroadcastQueueContext.o: private/BroadcastQueueContext.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/BroadcastQueueContext.o private/BroadcastQueueContext.cpp

${OBJECTDIR}/private/BroadcastQueueGroupContext.o: private/BroadcastQueueGroupContext.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/BroadcastQueueGroupContext.o private/BroadcastQueueGroupContext.cpp

${OBJECTDIR}/private/BroadcastStreamer.o: private/BroadcastStreamer.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/BroadcastStreamer.o private/BroadcastStreamer.cpp

${OBJECTDIR}/private/ClientFactory.o: private/ClientFactory.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/ClientFactory.o private/ClientFactory.cpp

${OBJECTDIR}/private/ClientFactory_Linux.o: private/ClientFactory_Linux.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/ClientFactory_Linux.o private/ClientFactory_Linux.cpp

${OBJECTDIR}/private/Common.o: private/Common.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/Common.o private/Common.cpp

${OBJECTDIR}/private/ConnectionContext.o: private/ConnectionContext.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/ConnectionContext.o private/ConnectionContext.cpp

${OBJECTDIR}/private/ConnectionContextPool.o: private/ConnectionContextPool.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/ConnectionContextPool.o private/ConnectionContextPool.cpp

${OBJECTDIR}/private/CumulativeMeasure.o: private/CumulativeMeasure.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/CumulativeMeasure.o private/CumulativeMeasure.cpp

${OBJECTDIR}/private/Demultiplexor.o: private/Demultiplexor.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/Demultiplexor.o private/Demultiplexor.cpp

${OBJECTDIR}/private/DemuxImpl_Linux.o: private/DemuxImpl_Linux.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/DemuxImpl_Linux.o private/DemuxImpl_Linux.cpp

${OBJECTDIR}/private/Dispatcher.o: private/Dispatcher.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/Dispatcher.o private/Dispatcher.cpp

${OBJECTDIR}/private/DistributionMeasure.o: private/DistributionMeasure.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/DistributionMeasure.o private/DistributionMeasure.cpp

${OBJECTDIR}/private/DurationMeasure.o: private/DurationMeasure.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/DurationMeasure.o private/DurationMeasure.cpp

${OBJECTDIR}/private/GarbageCollector.o: private/GarbageCollector.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/GarbageCollector.o private/GarbageCollector.cpp

${OBJECTDIR}/private/IOQueue_Linux.o: private/IOQueue_Linux.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/IOQueue_Linux.o private/IOQueue_Linux.cpp

${OBJECTDIR}/private/KeyedAveragedDistributionMeasure.o: private/KeyedAveragedDistributionMeasure.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/KeyedAveragedDistributionMeasure.o private/KeyedAveragedDistributionMeasure.cpp

${OBJECTDIR}/private/KeyedAveragedMeasure.o: private/KeyedAveragedMeasure.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/KeyedAveragedMeasure.o private/KeyedAveragedMeasure.cpp

${OBJECTDIR}/private/Listener.o: private/Listener.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/Listener.o private/Listener.cpp

${OBJECTDIR}/private/ListenerImpl_Linux.o: private/ListenerImpl_Linux.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/ListenerImpl_Linux.o private/ListenerImpl_Linux.cpp

${OBJECTDIR}/private/ListenerOptions.o: private/ListenerOptions.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/ListenerOptions.o private/ListenerOptions.cpp

${OBJECTDIR}/private/LogicalConnection.o: private/LogicalConnection.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/LogicalConnection.o private/LogicalConnection.cpp

${OBJECTDIR}/private/LogicalConnectionImpl.o: private/LogicalConnectionImpl.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/LogicalConnectionImpl.o private/LogicalConnectionImpl.cpp

${OBJECTDIR}/private/LogicalConnectionPool.o: private/LogicalConnectionPool.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/LogicalConnectionPool.o private/LogicalConnectionPool.cpp

${OBJECTDIR}/private/Measure.o: private/Measure.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/Measure.o private/Measure.cpp

${OBJECTDIR}/private/MeasureArgs.o: private/MeasureArgs.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/MeasureArgs.o private/MeasureArgs.cpp

${OBJECTDIR}/private/MonitorAcceptor.o: private/MonitorAcceptor.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/MonitorAcceptor.o private/MonitorAcceptor.cpp

${OBJECTDIR}/private/MonitorConnection.o: private/MonitorConnection.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/MonitorConnection.o private/MonitorConnection.cpp

${OBJECTDIR}/private/MonitorsBroadcastManager.o: private/MonitorsBroadcastManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/MonitorsBroadcastManager.o private/MonitorsBroadcastManager.cpp

${OBJECTDIR}/private/MonitorsMsgFactory.o: private/MonitorsMsgFactory.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/MonitorsMsgFactory.o private/MonitorsMsgFactory.cpp

${OBJECTDIR}/private/PacketInfo.o: private/PacketInfo.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/PacketInfo.o private/PacketInfo.cpp

${OBJECTDIR}/private/PhysicalConnection.o: private/PhysicalConnection.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/PhysicalConnection.o private/PhysicalConnection.cpp

${OBJECTDIR}/private/PhysicalConnectionPool.o: private/PhysicalConnectionPool.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/PhysicalConnectionPool.o private/PhysicalConnectionPool.cpp

${OBJECTDIR}/private/PhysicalConnection_Linux.o: private/PhysicalConnection_Linux.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/PhysicalConnection_Linux.o private/PhysicalConnection_Linux.cpp

${OBJECTDIR}/private/ProtocolManager.o: private/ProtocolManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/ProtocolManager.o private/ProtocolManager.cpp

${OBJECTDIR}/private/QueueOptions.o: private/QueueOptions.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/QueueOptions.o private/QueueOptions.cpp

${OBJECTDIR}/private/Routines_Linux.o: private/Routines_Linux.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/Routines_Linux.o private/Routines_Linux.cpp

${OBJECTDIR}/private/Server.o: private/Server.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/Server.o private/Server.cpp

${OBJECTDIR}/private/ServerImpl.o: private/ServerImpl.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/ServerImpl.o private/ServerImpl.cpp

${OBJECTDIR}/private/ServerOptions.o: private/ServerOptions.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/ServerOptions.o private/ServerOptions.cpp

${OBJECTDIR}/private/ServerStats.o: private/ServerStats.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/ServerStats.o private/ServerStats.cpp

${OBJECTDIR}/private/Service.o: private/Service.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/Service.o private/Service.cpp

${OBJECTDIR}/private/StopWatch.o: private/StopWatch.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/StopWatch.o private/StopWatch.cpp

${OBJECTDIR}/private/StopWatchImpl_Linux.o: private/StopWatchImpl_Linux.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/StopWatchImpl_Linux.o private/StopWatchImpl_Linux.cpp

${OBJECTDIR}/private/SystemService.o: private/SystemService.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/SystemService.o private/SystemService.cpp

${OBJECTDIR}/private/SystemServiceImpl_Linux.o: private/SystemServiceImpl_Linux.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/SystemServiceImpl_Linux.o private/SystemServiceImpl_Linux.cpp

${OBJECTDIR}/private/Utilities_Linux.o: private/Utilities_Linux.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/Utilities_Linux.o private/Utilities_Linux.cpp

${OBJECTDIR}/private/XMLPacket.o: private/XMLPacket.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/XMLPacket.o private/XMLPacket.cpp

${OBJECTDIR}/private/XMLProtocol.o: private/XMLProtocol.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/XMLProtocol.o private/XMLProtocol.cpp

${OBJECTDIR}/private/dllmain.o: private/dllmain.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/dllmain.o private/dllmain.cpp

${OBJECTDIR}/private/linuxcs.o: private/linuxcs.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/linuxcs.o private/linuxcs.cpp

${OBJECTDIR}/private/stdafx.o: private/stdafx.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/stdafx.o private/stdafx.cpp

${OBJECTDIR}/private/xmlParser.o: private/xmlParser.cpp 
	${MKDIR} -p ${OBJECTDIR}/private
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/private/xmlParser.o private/xmlParser.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ../output/libpushframework.so

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
