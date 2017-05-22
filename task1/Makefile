TASK1_DIR = $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

include Makefile_env.inc
TASK_DIR:=.
LIB_NAME:=goodbye
LIB_NAME_SECOND:=hello
target_objs:=$(TARGET_OBJ_DIR)/libgoodbye.o $(TARGET_OBJ_DIR)/libhello.o 
BINARY_NAME = hello

all:
	make libs
	@mkdir -p $(OUT_TARG_DIR)/bin
	$(CROSS_LD) hello.c $(CROSS_LDFLAG) $(EXTRA_LIBS) -L$(OUT_TARG_DIR)/lib $(target_objs) -o $(OUT_TARG_DIR)/bin/$(BINARY_NAME)
	
make libs: 

	@mkdir -p $(TARGET_OBJ_DIR)
	
	make $(target_objs)
	
	@mkdir -p $(OUT_TARG_DIR)/lib
	$(CROSS_LD) $(CROSS_LDFLAGS) $(target_objs)-shared -o $(OUT_TARG_DIR)/lib/lib$(LIB_NAME).so
	
	$(CROSS_LD) $(CROSS_LDFLAGS) $(target_objs)-shared -o $(OUT_TARG_DIR)/lib/lib$(LIB_NAME_SECOND).so
	
clean:
	@rm -rf $(TARGET_OBJ_DIR) $(OUT_TARG_DIR)


$(target_objs):$(TARGET_OBJ_DIR)/%.o: %.c
		$(CROSS_CC) $(CROSS_CFLAGS) $(CROSS_INCLUDES) -c -o $@ $<
		
