OUTPUT = build
CFLAGS = -O0 -g3
LFLAGS = -lftdi1

all: test

%.o: %.c out
	$(CC) $(CFLAGS) -c -o $(OUTPUT)/$@ $<

test: main.o
	$(CC) $(CFLAGS) -o $(OUTPUT)/test $(OUTPUT)/main.o $(LFLAGS) 

.PHONY: clean out

clean:
	rm -rf $(OUTPUT)

out:
	mkdir -p $(OUTPUT)