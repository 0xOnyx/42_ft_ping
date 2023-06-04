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

		tvsend = (struct timeval *)(ptr + ihl + HEADER_ICMP);

		struct timeval a; //shit 8 byte align address
		memcpy(&a, tvsend, sizeof(struct timeval));
		tvsend = &a;

		tv_sub(tvrecv, tvsend);
		rtt = (double)tvrecv->tv_sec * 1000.0 + (double)tvrecv->tv_usec / 1000.0; //to ms;
		if (!flood)
			printf("%d bytes from %s (%s): icmp_seq=%d ttl=%d time=%.2lf ms\n",
			   icmplen, pr.canonname ? pr.canonname : host, Sock_ntop_host(pr.sarecv),
			   icmp->icmp_hun.ih_idseq.icd_seq, ip->ip_ttl, rtt);
		else
			write(1, "\b", 1);
		set_rtt(rtt);
		nrecv++;
	}
	else if (icmp->icmp_type == ICMP_TIMXCEED)
	{
		char *lookup;

		lookup = (char *)reverse_dns_lookup(pr.sarecv, pr.salen);
		if (!lookup)
			lookup = host;
		if (!flood)
			printf("From %s (%s) icmp_seq=%d Time to live exceeded\n",
			   lookup, Sock_ntop_host(pr.sarecv),
			   icmp->icmp_hun.ih_idseq.icd_seq);
		else
			write(1, "E", 1);
	}
	else if (verbose)
	{
		if (!flood)
			printf("%d bytes from %s: type = %d, code = %d\n",
			   icmplen, Sock_ntop_host(pr.sarecv), icmp->icmp_type, icmp->icmp_code);
		else
			write(1, "E", 1);
	}
}