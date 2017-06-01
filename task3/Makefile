TASK1_DIR = $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

include Makefile_env.inc
TASK_DIR:=.


BINARY_NAME = main

all:
	@mkdir -p $(OUT_TARG_DIR)/bin
	$(CROSS_LD) main.c $(CROSS_LDFLAG) $(EXTRA_LIBS) -o $(OUT_TARG_DIR)/bin/$(BINARY_NAME)
	

	
clean:
	@rm -rf $(TARGET_OBJ_DIR) $(OUT_TARG_DIR)


		
