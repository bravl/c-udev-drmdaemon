CC = gcc
CC_FLAGS = -w -I/usr/include/libdrm/
LD_FLAGS = -ldrm -ludev -lpthread
PASSWD = "enterpasswd"
EXEC = drmdaemon
IP = "10.200.18.205"
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)
all: $(EXEC) cleanup

$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC) $(LD_FLAGS)

%.o: %.c
	$(CC) -c $(CC_FLAGS) $< -o $@

cleanup:
	rm -f $(OBJECTS)
clean:
	rm -f $(EXEC) $(OBJECTS) TODO
todo:
	grep -ihr --exclude="*.swp" --exclude="Makefile" --exclude="TODO.txt" TODO: | tr -d '/','*' | sed -e 's/^[ \t]*//' > TODO
install:
	sshpass -p '$(PASSWD)' scp drmdaemon root@$(IP):/store/
