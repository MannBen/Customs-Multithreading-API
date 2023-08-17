# thread library
lib := libuthread.a
objs := queue.o uthread.o context.o sem.o preempt.o

# general gcc options
CC = gcc
CFLAGS	:= -Wall -Wextra -Werror

# Debug flag
ifneq ($(D),1)
CFLAGS    += -O2
else
CFLAGS    += -g
endif

all: $(lib)

deps := $(patsubst %.o,%.d,$(objs))
-include $(deps)

$(lib): $(objs)
	ar rcs $@ $^

%.o: %.c
	@echo "CC	$@"
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo "clean"
	rm -f $(lib) $(objs) $(deps)