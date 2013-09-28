#
# Generic makefile
#

SHELL = sh
UNAME := $(shell uname)

# define the C compiler to use
CC = g++

# define any compile-time flags
CFLAGS = -Wall -g

# define any directories containing header files other than /usr/include
#
INCLUDES = 

# define library paths in addition to /usr/lib
LFLAGS =

# define any libraries to link into executable:
LIBS =

# Other flags
OTHER = 

# define the C source files

# Client
CLIENT_SRCSRAW = $(foreach d,.,$(wildcard $(addprefix $(d)/httpclient/*,.cpp)))
# File that contains main
CLIENT_MAIN = ./httpclient/main.cpp
CLIENT_SRCS = $(filter-out $(CLIENT_MAIN),$(CLIENT_SRCSRAW))

# Server                                                                      
SERVER_SRCSRAW = $(foreach d,.,$(wildcard $(addprefix $(d)/httpserver/*,.cpp)))
# File that contains main                                                      
SERVER_MAIN = ./httpserver/main.cpp
SERVER_SRCS = $(filter-out $(SERVER_MAIN),$(SERVER_SRCSRAW))

# Shared
SHARED_SRCSRAW =  $(foreach d,.,$(wildcard $(addprefix $(d)/shared/*,.cpp)))
SHARED_FILTER =
SHARED_SRCS =  $(filter-out $(SHARED_FILTER),$(SHARED_SRCSRAW))

# define the C object files 
CLIENT_OBJS = $(CLIENT_SRCS:.cpp=.o)
SERVER_OBJS = $(SERVER_SRCS:.cpp=.o)
SHARED_OBJS = $(SHARED_SRCS:.cpp=.o)

# define the executable file
CLIENT_BIN = http_client
SERVER_BIN = http_server

#
# The following part of the makefile is generic; it can be used to 
# build any executable just by changing the definitions above and by
# deleting dependencies appended to the file from 'make depend'
#

.PHONY: depend clean

all:    $(CLIENT_BIN) $(SERVER_BIN)
	@echo  Build Successful

# Client
$(CLIENT_BIN): $(CLIENT_OBJS) $(SHARED_OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(CLIENT_BIN) $(CLIENT_MAIN) $(CLIENT_OBJS) $(SHARED_OBJS) $(LFLAGS) $(LIBS) $(OTHER)

# Server
$(SERVER_BIN): $(SERVER_OBJS) $(SHARED_OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(SERVER_BIN) $(SERVER_MAIN) $(SERVER_OBJS) $(SHARED_OBJS) $(LFLAGS) $(LIBS) $(OTHER)

# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)
.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) httpclient/*.o httpserver/*.o shared/*.o *~ $(CLIENT_BIN) $(SERVER_BIN)

depend: $(SRCS) $(S_SRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it
