###############################################
#
# Makefile
#
###############################################
_PWD				:= $(shell pwd)
OUT_NAME			= GetText2WavFile
TARGET				= ${HOME}
export COMPLIER     := g++

HEADERS				= ${shell find ./ -name "*.h" -o -name "*.h"}
LDIR				:= -L${HOME}/lib
SRC_BASE			:= ./
LFLAGS 				:= 
						
SYSTEM_CFLAGS		:= -g -rdynamic -Wno-write-strings -std=gnu++11 -DUNIX #-Wall -Werror

#OBJECT 
OBJECT_PREFIX   	= ${HOME}/.objs
OBJECT_PATH     	= ${OBJECT_PREFIX}/${OUT_NAME}
EXEC            	= ${OBJECT_PATH}/${OUT_NAME}
CPP_SRCS 			= $(wildcard ${SRC_BASE}/*.cpp)
CPP_OBJS 			= $(addprefix ${OBJECT_PATH}/,$(notdir $(CPP_SRCS:.cpp=.o)))
OBJS 				= ${CPP_OBJS}

#OPTION
CXXFLAGS  			= ${SYSTEM_CFLAGS} ${G_PROJ_DEFINE} ${G_SYSTEM_DEFINE}

#DEPENENCY
all : INFO Makefile.deps DIR clean ${EXEC} install

INFO :
	@echo ======== FILE : ${_PWD}/cmn_pro.mk ========
	@echo ==== Process Information ====
	@echo OUT_NAME : ${OUT_NAME}
	@echo SRCS : ${CPP_SRCS}
	@echo OBJS : ${OBJS}
	@echo CXXFLAGS : ${CXXFLAGS}
	@echo OBJECT_PREFIX : ${OBJECT_PREFIX}
	@echo LDIR : ${LDIR}

Makefile.deps : ${FILES} ${HEADERS}

DIR:
	@echo ==== Make Directory ====
	if [ ! -f ${OBJECT_PATH} ]; then mkdir -p ${OBJECT_PATH}; fi
	if [ ! -f ${TARGET} ]; then mkdir -p ${TARGET}; fi

$(EXEC) : $(OBJS)
	@echo ==== Link ====
	${COMPLIER} -o ${EXEC} ${OBJS} ${CXXFLAGS} ${LDIR} ${LFLAGS}
	
$(OBJECT_PATH)/%.o: ${SRC_BASE}/%.cpp
	${COMPLIER} -c -o $@ $< -fpic ${CXXFLAGS} ${INCDIR}
	
clean:
	@echo ==== Clear ====
	\rm -rf ${OBJS};
	\rm -rf ${EXEC};
	@echo ==== Compile ====
	
install:
	@echo ==== Install ====
	chmod 755 ${EXEC}; cp ${EXEC} ${TARGET};
	