#****************************************************************************
#
# Makefile for TinyXml test.
# Lee Thomason
# www.grinninglizard.com
#
# This is a GNU make (gmake) makefile
#****************************************************************************

# DEBUG can be set to YES to include debugging info, or NO otherwise
#DEBUG          := YES

# PROFILE can be set to YES to include profiling info, or NO otherwise
#PROFILE        := NO

# TINYXML_USE_STL can be used to turn on STL support. NO, then STL
# will not be used. YES will include the STL files.
s := YES 

#****************************************************************************

CC     := gcc
CXX    := g++
LD     := g++
AR     := ar rc
RANLIB := ranlib

DEBUG_CFLAGS     := -Wall -Wno-format -g -DDEBUG -fPIC
RELEASE_CFLAGS   := -Wall -Wno-unknown-pragmas -Wno-format -O3 -fPIC

LIBS		 :=

DEBUG_CXXFLAGS   := ${DEBUG_CFLAGS} 
RELEASE_CXXFLAGS := ${RELEASE_CFLAGS}

DEBUG_LDFLAGS    := -g
RELEASE_LDFLAGS  :=

ifeq (d, ${mode})
   CFLAGS       := ${DEBUG_CFLAGS}
   CXXFLAGS     := ${DEBUG_CXXFLAGS}
   LDFLAGS      := ${DEBUG_LDFLAGS}
   LIB_DIR      := ../../lib/Debug/
else
   CFLAGS       := ${RELEASE_CFLAGS}
   CXXFLAGS     := ${RELEASE_CXXFLAGS}
   LDFLAGS      := ${RELEASE_LDFLAGS}
   LIB_DIR      := ../../lib/Release/
endif

ifeq (YES, ${PROFILE})
   CFLAGS   := ${CFLAGS} -pg -O3
   CXXFLAGS := ${CXXFLAGS} -pg -O3
   LDFLAGS  := ${LDFLAGS} -pg
endif

#****************************************************************************
# Preprocessor directives
#****************************************************************************

ifeq (YES, ${TINYXML_USE_STL})
  DEFS := -DTIXML_USE_STL
else
  DEFS :=
endif

#****************************************************************************
# Include paths
#****************************************************************************

#INCS := -I/usr/include/g++-2 -I/usr/local/include
INCS := -I../../include/tinyxml


#****************************************************************************
# Makefile code common to all platforms
#****************************************************************************

CFLAGS   := ${CFLAGS}   ${DEFS}
CXXFLAGS := ${CXXFLAGS}  ${DEFS}

#****************************************************************************
# Targets of the build
#****************************************************************************
SONAME  =   libtinyxml.a libtinyxml.so
#SONAME  =   libtinyxml.a
OUTPUT := xmltest

all: ${OUTPUT} ${SONAME}


#****************************************************************************
# Source files
#****************************************************************************

SRCS := tinyxml.cpp tinyxmlparser.cpp xmltest.cpp tinyxmlerror.cpp tinystr.cpp xmlconf.cpp

# Add on the sources for libraries
SRCS := ${SRCS}

OBJS := $(addsuffix .o,$(basename ${SRCS}))

#****************************************************************************
# Output
#****************************************************************************

${OUTPUT}: ${OBJS}
	${LD} -o $@ ${LDFLAGS} ${OBJS} ${LIBS} ${EXTRA_LIBS}

#****************************************************************************
# common rules
#****************************************************************************

# Rules for compiling source files to object files
%.o : %.cpp
	${CXX} -c ${CXXFLAGS} ${INCS} $< -o $@

%.o : %.c
	${CC} -c ${CFLAGS} ${INCS} $< -o $@

dist:
	bash makedistlinux

clean:
	-rm -f core ${OBJS} ${OUTPUT} ${SONAME}

depend:
	#makedepend ${INCS} ${SRCS}

install:
	cp libtinyxml.* ${LIB_DIR}	
	     
libtinyxml.a:tinyxml.o tinyxmlparser.o tinyxmlerror.o tinystr.o xmlconf.o
	rm -f libtinyxml.a
	${AR} libtinyxml.a tinyxml.o tinyxmlparser.o tinyxmlerror.o tinystr.o xmlconf.o
	-ranlib libtinyxml.a

libtinyxml.so:tinyxml.o tinyxmlparser.o tinyxmlerror.o tinystr.o xmlconf.o
	rm -f libtinyxml.so
	rm -f libtinyxml.so.0.1
	$(CXX) $(CFLAGS) -shared -Wl,-soname,libtinyxml.so.0.1 -o libtinyxml.so.0.1 tinyxml.o tinyxmlparser.o tinyxmlerror.o tinystr.o xmlconf.o
	ln -s libtinyxml.so.0.1 libtinyxml.so

test:libtinyxml.a conftest.o
	${CXX} -o test $(CFLAGS) conftest.o libtinyxml.a 


