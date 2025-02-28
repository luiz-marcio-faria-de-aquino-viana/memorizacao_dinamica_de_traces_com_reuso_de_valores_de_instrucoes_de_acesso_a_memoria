#
# SUPERSIM - 25/05/2020
# Luiz Marcio Faria de Aquino Viana, M.Sc.
# COMPILAR E LIGAR USANDO -m32 (RODA APENAS EM 32 BITS)
#

CC = /usr/bin/gcc
CFLAGS = -ggdb -m32 -O0 -Wall 

OBJS = error.o component.o stat.o asm.o queue.o trace.o ftrace.o vtrace.o mem.o memotable.o memotableg.o memotablet.o btb.o predict.o fetchbuf.o immu.o dmmu.o fetch.o iqueue.o idecctl.o idec.o disq.o decode.o reg.o sreg.o ireg.o rsctl.o rs.o maq.o rob.o dtm.o dispatch.o resbuf.o iu.o lsu.o bpu.o complete.o io.o trap.o wb.o cpu.o main.o

all:	$(OBJS)
	    $(CC) $(OBJS) -m32 -lstdc++ -o supersim

bak:
	rm -f *~

clean:
	rm $(OBJS)
	rm -f supersim

log:
	rm *.log

error.o:	error.h error.cpp
		$(CC) $(CFLAGS) -c error.cpp

component.o:	component.h component.cpp
		$(CC) $(CFLAGS) -c component.cpp

stat.o:		defs.h types.h opcode.h error.h stat.h stat.cpp
		$(CC) $(CFLAGS) -c stat.cpp

asm.o:		defs.h types.h opcode.h error.h asm.h asm.cpp
		$(CC) $(CFLAGS) -c asm.cpp

queue.o:	types.h error.h queue.h queue.cpp
		$(CC) $(CFLAGS) -c queue.cpp

trace.o:	types.h error.h queue.h trace.h trace.cpp
		$(CC) $(CFLAGS) -c trace.cpp

ftrace.o:	types.h error.h queue.h ftrace.h ftrace.cpp
		$(CC) $(CFLAGS) -c ftrace.cpp

vtrace.o:	types.h error.h queue.h vtrace.h vtrace.cpp
		$(CC) $(CFLAGS) -c vtrace.cpp

memotable.o:	defs.h types.h error.h memotable.h memotable.cpp
		$(CC) $(CFLAGS) -c memotable.cpp

memotableg.o:	defs.h types.h error.h memotable.h memotableg.h memotableg.cpp
		$(CC) $(CFLAGS) -c memotableg.cpp

memotablet.o:	defs.h types.h error.h  idecctl.h  rsctl.h memotable.h memotablet.h memotablet.cpp
		$(CC) $(CFLAGS) -c memotablet.cpp

mem.o:		defs.h types.h opcode.h error.h asm.h mem.h mem.cpp
		$(CC) $(CFLAGS) -c mem.cpp

btb.o:		defs.h types.h error.h btb.h btb.cpp
		$(CC) $(CFLAGS) -c btb.cpp

predict.o:	defs.h types.h error.h btb.h predict.h predict.cpp
		$(CC) $(CFLAGS) -c predict.cpp

fetchbuf.o:	defs.h types.h opcode.h error.h asm.h queue.h btb.h predict.h fetchbuf.h fetchbuf.cpp
		$(CC) $(CFLAGS) -c fetchbuf.cpp

immu.o:		defs.h types.h opcode.h error.h asm.h queue.h mem.h btb.h predict.h fetchbuf.h immu.h immu.cpp
		$(CC) $(CFLAGS) -c immu.cpp

dmmu.o:		defs.h types.h error.h mem.h dmmu.h dmmu.cpp
		$(CC) $(CFLAGS) -c dmmu.cpp

iqueue.o:	defs.h types.h opcode.h error.h asm.h queue.h btb.h predict.h fetchbuf.h iqueue.h iqueue.cpp
		$(CC) $(CFLAGS) -c iqueue.cpp

fetch.o:	defs.h types.h opcode.h error.h asm.h queue.h mem.h btb.h predict.h fetchbuf.h immu.h iqueue.h fetch.h fetch.cpp
		$(CC) $(CFLAGS) -c fetch.cpp

idecctl.o:	defs.h types.h opcode.h error.h asm.h queue.h btb.h predict.h fetchbuf.h iqueue.h idecctl.h idecctl.cpp
		$(CC) $(CFLAGS) -c idecctl.cpp

idec.o:		defs.h types.h opcode.h error.h asm.h queue.h btb.h predict.h fetchbuf.h iqueue.h idecctl.h idec.h idec.cpp
		$(CC) $(CFLAGS) -c idec.cpp

disq.o:		defs.h types.h opcode.h error.h asm.h queue.h btb.h predict.h fetchbuf.h iqueue.h idec.h disq.h disq.cpp
		$(CC) $(CFLAGS) -c disq.cpp

decode.o:	defs.h types.h opcode.h error.h asm.h queue.h btb.h predict.h fetchbuf.h iqueue.h idecctl.h idec.h disq.h decode.h decode.cpp
		$(CC) $(CFLAGS) -c decode.cpp

reg.o:		defs.h types.h error.h reg.h reg.cpp
		$(CC) $(CFLAGS) -c reg.cpp

sreg.o:		defs.h types.h error.h reg.h sreg.h sreg.cpp
		$(CC) $(CFLAGS) -c sreg.cpp

ireg.o:		defs.h types.h error.h reg.h ireg.h ireg.cpp
		$(CC) $(CFLAGS) -c ireg.cpp

rsctl.o:	defs.h types.h opcode.h error.h asm.h queue.h btb.h predict.h fetchbuf.h iqueue.h idecctl.h idec.h disq.h decode.h reg.h sreg.h ireg.h rsctl.h rsctl.cpp
		$(CC) $(CFLAGS) -c rsctl.cpp

rs.o:		defs.h types.h opcode.h error.h asm.h queue.h btb.h predict.h fetchbuf.h iqueue.h idecctl.h idec.h disq.h decode.h reg.h sreg.h ireg.h rsctl.h rs.h rs.cpp
		$(CC) $(CFLAGS) -c rs.cpp

maq.o:		defs.h types.h opcode.h error.h asm.h queue.h btb.h predict.h fetchbuf.h iqueue.h idecctl.h idec.h disq.h decode.h reg.h sreg.h ireg.h rsctl.h rs.h maq.h maq.cpp
		$(CC) $(CFLAGS) -c maq.cpp

rob.o:		defs.h types.h opcode.h error.h asm.h queue.h btb.h predict.h fetchbuf.h iqueue.h idecctl.h idec.h disq.h decode.h reg.h sreg.h ireg.h rsctl.h rs.h maq.h rob.h rob.cpp
		$(CC) $(CFLAGS) -c rob.cpp

dtm.o:		defs.h types.h opcode.h error.h asm.h queue.h memotable.h memotableg.h btb.h predict.h fetchbuf.h iqueue.h idecctl.h idec.h disq.h decode.h reg.h sreg.h ireg.h rsctl.h rs.h maq.h rob.h dtm.h dtm.cpp
		$(CC) $(CFLAGS) -c dtm.cpp

dispatch.o:	defs.h types.h opcode.h error.h asm.h queue.h btb.h predict.h fetchbuf.h iqueue.h idecctl.h idec.h disq.h decode.h reg.h sreg.h ireg.h rsctl.h rs.h maq.h rob.h dtm.h dispatch.h dispatch.cpp
		$(CC) $(CFLAGS) -c dispatch.cpp

resbuf.o:	defs.h types.h opcode.h error.h asm.h queue.h btb.h predict.h fetchbuf.h iqueue.h idecctl.h idec.h disq.h decode.h reg.h sreg.h ireg.h rsctl.h rs.h resbuf.h resbuf.cpp
		$(CC) $(CFLAGS) -c resbuf.cpp

iu.o:		defs.h types.h opcode.h error.h asm.h queue.h btb.h predict.h fetchbuf.h iqueue.h idecctl.h idec.h disq.h decode.h reg.h sreg.h ireg.h rsctl.h rs.h maq.h rob.h dispatch.h iu.h iu.cpp
		$(CC) $(CFLAGS) -c iu.cpp

lsu.o:		defs.h types.h opcode.h error.h asm.h queue.h dmmu.h btb.h predict.h fetchbuf.h iqueue.h idecctl.h idec.h disq.h decode.h reg.h sreg.h ireg.h rsctl.h rs.h maq.h rob.h dispatch.h iu.h lsu.h lsu.cpp
		$(CC) $(CFLAGS) -c lsu.cpp

bpu.o:		defs.h types.h opcode.h error.h asm.h queue.h dmmu.h btb.h predict.h fetchbuf.h iqueue.h idecctl.h idec.h disq.h decode.h reg.h sreg.h ireg.h rsctl.h rs.h maq.h rob.h dispatch.h iu.h lsu.h bpu.h bpu.cpp
		$(CC) $(CFLAGS) -c bpu.cpp

complete.o:	defs.h types.h opcode.h error.h asm.h queue.h btb.h predict.h fetchbuf.h iqueue.h idecctl.h idec.h disq.h decode.h reg.h sreg.h ireg.h rsctl.h rs.h maq.h rob.h dispatch.h iu.h lsu.h bpu.h complete.h complete.cpp
		$(CC) $(CFLAGS) -c complete.cpp

io.o:		io.h io.c
		$(CC) $(CFLAGS) -c io.c

trap.o:		defs.h types.h opcode.h error.h syscall.h mem.h reg.h sreg.h ireg.h trap.h trap.cpp
		$(CC) $(CFLAGS) -c trap.cpp

wb.o:		defs.h types.h opcode.h error.h stat.h syscall.h asm.h queue.h btb.h predict.h fetchbuf.h iqueue.h idecctl.h idec.h disq.h decode.h reg.h sreg.h ireg.h rsctl.h rs.h maq.h rob.h dispatch.h iu.h lsu.h bpu.h complete.h trap.h wb.h wb.cpp
		$(CC) $(CFLAGS) -c wb.cpp

cpu.o:		defs.h types.h opcode.h error.h stat.h syscall.h asm.h queue.h btb.h predict.h fetchbuf.h iqueue.h idecctl.h idec.h disq.h decode.h reg.h sreg.h ireg.h rsctl.h rs.h maq.h rob.h dispatch.h iu.h lsu.h bpu.h complete.h trap.h wb.h cpu.h cpu.cpp
		$(CC) $(CFLAGS) -c cpu.cpp

main.o:		defs.h types.h opcode.h error.h stat.h syscall.h asm.h queue.h btb.h predict.h fetchbuf.h iqueue.h idecctl.h idec.h disq.h decode.h reg.h sreg.h ireg.h rsctl.h rs.h maq.h rob.h dispatch.h iu.h lsu.h bpu.h complete.h trap.h wb.h main.h main.cpp
		$(CC) $(CFLAGS) -c main.cpp
