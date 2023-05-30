#include "ft_ping.h"

void	send_v4(void)
{
	int		 		len;
	struct icmp		*icmp;
	unsigned char	*data;

	icmp = (struct icmp *)send_buff;
	icmp->icmp_code = 0;
	icmp->icmp_type = ICMP_ECHO;
	icmp->icmp_hun.ih_idseq.icd_seq = nsent++;
	icmp->icmp_hun.ih_idseq.icd_id = pid;
	data = send_buff + HEADER_ICMP;
	memset(data, 0, data_len);
	gettimeofday((struct timeval *)data, 0);
	len = HEADER_ICMP + data_len;
	icmp->icmp_cksum = checksum((void *)icmp, (int)len);
	sendto(sockfd, send_buff, len, 0, pr.sasend, pr.salen);
}