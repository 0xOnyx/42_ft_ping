#include "ft_ping.h"

void	proc_v4(ssize_t n, struct msghdr *msg, struct timeval *tvrecv)
{
	int 			ihl;
	int 			icmplen;
	double			rtt;
	struct ip		*ip;
	struct icmp		*icmp;
	struct timeval	*tvsend;
	unsigned char	*ptr;


	ptr = msg->msg_iov->iov_base;
	ip = (struct ip *)ptr;
	ihl = ip->ip_hl << 2; //to bytes value * 4;
	if (ip->ip_p != IPPROTO_ICMP)
		return ; //not icmp packet
	icmp = (struct icmp *)(ptr + ihl);
	icmplen = (int)n - ihl;
	if (icmplen < 8)
		return ; //mal format packet
	if (icmp->icmp_type == ICMP_ECHOREPLY)
	{
		if (icmp->icmp_hun.ih_idseq.icd_id != pid
			|| icmplen < 16)
			return ;
		tvsend = (struct timeval *)(icmp + HEADER_ICMP);
		tv_sub(tvrecv, rvsend);
		rtt = tvrecv->tv_sec * 1000.0 + tvrecv->tv_usec / 1000.0; //to ms;
		printf("%d bytes from %s (%s): icmp_seq=%d ttl=%d time=%lf ms\n",
			   icmplen, host, )
	}
	else if (verbose)
	{

	}
}