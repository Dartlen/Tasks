TASK1_DIR = $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

include Makefile_env.inc
TASK_DIR:=.


BINARY_NAME_SERVER = server
BINARY_NAME_CLIENT = client

all:
	@mkdir -p $(OUT_TARG_DIR)/bin/server
	$(CROSS_LD) server.c $(CROSS_LDFLAG) $(EXTRA_LIBS) -o $(OUT_TARG_DIR)/bin/server/$(BINARY_NAME_SERVER)
	@mkdir -p $(OUT_TARG_DIR)/bin/client
	$(CROSS_LD) client.c $(CROSS_LDFLAG) $(EXTRA_LIBS) -o $(OUT_TARG_DIR)/bin/client/$(BINARY_NAME_CLIENT)
	

	
clean:
	@rm -rf $(TARGET_OBJ_DIR) $(OUT_TARG_DIR)


		
