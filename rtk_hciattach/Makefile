CFLAGS := -g
#CC := $(CROSS_COMPLE)gcc
all: rtk_hciattach
OBJS := hciattach.o hciattach_rtk.o hciattach_h4.o rtb_fwc.o

rtk_hciattach: $(OBJS)
	$(CC) -o rtk_hciattach $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)  rtk_hciattach

install:
	install -m 755 -d $(DESTDIR)/usr/bin
	install -m 755 rtk_hciattach $(DESTDIR)/usr/bin

tags: FORCE
	ctags -R
	find ./ -name "*.h" -o -name "*.c" -o -name "*.cc" -o -name "*.cpp" > cscope.files
	cscope -bkq -i cscope.files
PHONY += FORCE
FORCE:
