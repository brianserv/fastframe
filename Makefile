include Makefile_public

TARGET	= $(DEBUG_TARGET)

DEBUG_TARGET = $(LIB_DIR)/libfastframe.a

SRC = $(wildcard *.cpp)
OBJS = $(addprefix $(OBJ_DIR)/,$(subst .cpp,.o,$(SRC)))

INC = -I$(FASTFRAME_INCLUDE_DIR) -I$(MYSQL_INCLUDE_DIR)

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(AR) $(ARFLAGS) $@ $? #$(COMMON_LIB)

$(OBJ_DIR)/%.o : %.cpp
	$(CXX) $(CPPFLAGS) -c $< -o $@

clean: 
	rm -f $(OBJS) $(TARGET)
