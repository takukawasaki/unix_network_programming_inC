#include "sntp.h"



int
main(int argc, char **argv)
{
	int					sockfd;
	char				buf[MAXLINE];
	ssize_t				n;
	socklen_t			salen, len;
	struct ifi_info		*ifi;
	struct sockaddr		*mcastsa, *wild, *from;
	struct timeval		now;

	if (argc != 2)
		err_quit("usage: ssntp <IPaddress>");

	sockfd = Udp_client(argv[1], "ntp", (void **) &mcastsa, &salen);

	wild = Malloc(salen);
	memcpy(wild, mcastsa, salen);	/* copy family and port */
	sock_set_wild(wild, salen);
	Bind(sockfd, wild, salen);	/* bind wildcard */

#ifdef	MCAST
		/* 4obtain interface list and process each one */
	for (ifi = Get_ifi_info(mcastsa->sa_family, 1); ifi != NULL;
		 ifi = ifi->ifi_next) {
		if (ifi->ifi_flags & IFF_MULTICAST) {
			Mcast_join(sockfd, mcastsa, salen, ifi->ifi_name, 0);
			printf("joined %s on %s\n",
				   Sock_ntop(mcastsa, salen), ifi->ifi_name);
		}
	}
#endif

	from = Malloc(salen);
	for ( ; ; ) {
		len = salen;
		n = Recvfrom(sockfd, buf, sizeof(buf), 0, from, &len);
		Gettimeofday(&now, NULL);
		sntp_proc(buf, n, &now);
	}
}




void
sntp_proc(char *buf, ssize_t n, struct timeval *nowptr)
{
	int				version, mode;
	uint32_t		nsec, useci;
	double			usecf;
	struct timeval	diff;
	struct ntpdata	*ntp;

	if (n < (ssize_t)sizeof(struct ntpdata)) {
		printf("\npacket too small: %d bytes\n", n);
		return;
	}

	ntp = (struct ntpdata *) buf;
	version = (ntp->status & VERSION_MASK) >> 3;
	mode = ntp->status & MODE_MASK;
	printf("\nv%d, mode %d, strat %d, ", version, mode, ntp->stratum);
	if (mode == MODE_CLIENT) {
		printf("client\n");
		return;
	}

	nsec = ntohl(ntp->xmt.int_part) - JAN_1970;
	useci = ntohl(ntp->xmt.fraction);	/* 32-bit integer fraction */
	usecf = useci;				/* integer fraction -> double */
	usecf /= 4294967296.0;		/* divide by 2**32 -> [0, 1.0) */
	useci = usecf * 1000000.0;	/* fraction -> parts per million */

	diff.tv_sec = nowptr->tv_sec - nsec;
	if ( (diff.tv_usec = nowptr->tv_usec - useci) < 0) {
		diff.tv_usec += 1000000;
		diff.tv_sec--;
	}
	useci = (diff.tv_sec * 1000000) + diff.tv_usec;	/* diff in microsec */
	printf("clock difference = %d usec\n", useci);
}
