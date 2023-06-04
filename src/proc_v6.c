#include "ft_ping.h"

void 	proc_v6(ssize_t n, struct msghdr *msg, struct timeval *tvrecv)
{
	int 				hlim;
	double				rtt;
	struct icmp6_hdr	*icmp6;
	struct timeval		*rvsend;
	struct cmsghdr		*cmsg;
	unsigned char		*ptr;

	ptr = msg->msg_iov->iov_base;
	icmp6 = (struct icmp6_hdr *)ptr;
	if (n < 8)
		return ; //not icmp packet
	if (icmp6->icmp6_type == ICMP6_ECHO_REPLY)
	{
		if (icmp6->icmp6_dataun.icmp6_un_data16[0] != pid
			|| n < 16)
			return ; //no data struct timeval

		rvsend = (struct timeval *)(ptr + HEADER_ICMP);

		struct timeval a; //shit 8 byte align address
		memcpy(&a, rvsend, sizeof(struct timeval));
		rvsend = &a;

		tv_sub(tvrecv, rvsend);
		rtt = (double)tvrecv->tv_sec * 1000.0 + (double)tvrecv->tv_usec / 1000.0; //to ms;

		hlim = -1;
		for (cmsg = CMSG_FIRSTHDR(msg); cmsg != NULL; cmsg = CMSG_NXTHDR(msg, cmsg))
		{
			if (cmsg && cmsg->cmsg_level == IPPROTO_IPV6 && cmsg->cmsg_type == IPV6_HOPLIMIT)
			{
				hlim = *(u_int16_t *) CMSG_DATA(cmsg);
				break ;
			}
		}

		if (!flood)
			printf("%zd bytes from %s (%s): icmp6_seq=%d hlim=%d time=%.2lf ms\n",
				   n, pr.canonname ? pr.canonname : host, Sock_ntop_host(pr.sarecv),
				   icmp6->icmp6_dataun.icmp6_un_data16[1],
				   hlim, rtt);
		else
			write(1, "\b", 1);
		set_rtt(rtt);
		nrecv++;
	}
	else if (icmp6->icmp6_type == ICMP6_TIME_EXCEEDED)
	{
		char *lookup;

		lookup = (char *)reverse_dns_lookup(pr.sarecv, pr.salen);
		if (!lookup)
			lookup = host;
		if (!flood)
			printf("From %s (%s) icmp_seq=%d Time to live exceeded\n",
				   lookup, Sock_ntop_host(pr.sarecv),
				   icmp6->icmp6_dataun.icmp6_un_data16[1]);
		else
			write(1, "E", 1);
	}
	else if (verbose)
	{
		if (!flood)
			printf("%zd bytes from %s: type = %d, code = %d\n",
				   n, Sock_ntop_host(pr.sarecv), icmp6->icmp6_type, icmp6->icmp6_code);
		else
			write(1, "E", 1);
	}
}