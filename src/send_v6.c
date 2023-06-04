#include "ft_ping.h"

void	send_v6(void)
{
	int		 			len;
	struct icmp6_hdr	*icmp6;
	unsigned char		*data;

	len = HEADER_ICMP + data_len;
	memset(send_buff, 0, len);
	icmp6 = (struct icmp6_hdr *)send_buff;
	icmp6->icmp6_code = 0;
	icmp6->icmp6_type = ICMP6_ECHO_REQUEST;
	icmp6->icmp6_dataun.icmp6_un_data16[0] = pid;
	icmp6->icmp6_dataun.icmp6_un_data16[1] = nsent++;
	data = send_buff + HEADER_ICMP;
	memset(data, 0x0, data_len);
	gettimeofday((struct timeval *)data, 0);
	sendto(sockfd, send_buff, len, 0, pr.sasend, pr.salen);
	if (flood)
		write(1, ".", 1);
}