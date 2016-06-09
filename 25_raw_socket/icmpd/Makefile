include ../../Make.definesfortraceroute

OBJS = icmpd.o readable_listen.o readable_conn.o readable_v4.o readable_v6.o 


PROGS =	icmpd udpcli



icmpd:	${OBJS}
		${CC} ${CFLAGS} -o $@ ${OBJS} ${LIBS}

udpcli:	udpcli.o dg_cli.o
		${CC} ${CFLAGS} -o $@ udpcli01.o dgcli01.o ${LIBS}

clean:
		rm -f ${PROGS} ${CLEANFILES}
