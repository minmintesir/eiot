
SHELL=/bin/bash

CROSSCOMPILE=/opt/gcc-linaro/arm-linux-gnueabihf/4.9-2015.05-x86_64/bin/arm-linux-gnueabihf-

AR=$(CROSSCOMPILE)ar
CC=$(CROSSCOMPILE)gcc
STRIP=$(CROSSCOMPILE)strip

CFLAGS+=-Wall -O2

CFLAGS+=-D_GNU_SOURCE

LIB_OBJS=tq2440.o
LIB_OBJS2=rc522.o

STATIC_LIB=libtq2440.a
STATIC_LIB2=librc522.a

.PHONY:tags clean

CCNAME=$(shell basename $(CC))
ifneq ("$V", "1")
  Q=@
endif

#CFLAGS+=-ggdb3 -O0
all: $(STATIC_LIB) $(STATIC_LIB2) 
	cp $(STATIC_LIB) $(STATIC_LIB2) ../lib
$(STATIC_LIB):$(LIB_OBJS) 
	$(AR) cru -o $@ $^

$(STATIC_LIB2):$(LIB_OBJS2) 
	$(AR) cru $@ $^

tags:
	ctags *.[ch]

%.o : %.c
	@echo -e "\033[1;35m $(CCNAME) $@\033[0m"
	$(Q)$(CC) -pipe $(CFLAGS) -c -o $@ $<

clean:
	rm *.a $(LIB_OBJS) $(LIB_OBJS2) -f;

distclean: clean
	rm tags -f;
