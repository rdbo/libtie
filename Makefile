SRC_DIR=./tie
SRC_FILES=$(SRC_DIR)/proc/proc.c $(SRC_DIR)/io/io.c
OUT_DIR=./bin
OUTPUT=libtie.so
CFLAGS=-g -std=c89 -ansi -pedantic -Wall -Wextra -I$(SRC_DIR)
LDFLAGS=-Wl,-R,$(OUT_DIR) -Wl,--enable-new-dtags -L$(OUT_DIR) -l:$(OUTPUT)

libtie: clean
	$(CC) -o $(OUT_DIR)/$(OUTPUT) $(CFLAGS) -shared -fPIC $(SRC_FILES)

tests: libtie
	$(CC) -o $(OUT_DIR)/tests $(CFLAGS) $(LDFLAGS) tests/main.c
clean:
	if [ -d $(OUT_DIR) ]; then rm -rf $(OUT_DIR); fi
	mkdir $(OUT_DIR)