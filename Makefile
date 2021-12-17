C_FLAGS	:=	-c
O_FLAGS	:=	-o

CC		?=	gcc
RM		?=	rm -f
RM_DIR	?=	rm -rf

OBJS	:=	main.o httpd.o socket.o

#Verbose level
V=@

all:	httpd


httpd:	$(OBJS)
	$(V) $(CC) $(OBJS) $(O_FLAGS) $@
	$(V) echo " [MK]	httpd"

clean:
	$(V) $(RM) $(OBJS)
	$(V) echo " [RM]	$(OBJS)"

