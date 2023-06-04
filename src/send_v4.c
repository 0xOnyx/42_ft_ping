#include "ft_ping.h"

void	send_v4(void)
{
	int		 		len;
	struct icmp		*icmp;
	unsigned char	*data;

	len = HEADER_ICMP + data_len;
	memset(send_buff, 0, len);
	icmp = (struct icmp *)send_buff;
	icmp->icmp_code = 0;
	icmp->icmp_type = ICMP_ECHO;
	icmp->icmp_hun.ih_idseq.icd_seq = nsent++;
	icmp->icmp_hun.ih_idseq.icd_id = pid;
	data = send_buff + HEADER_ICMP;
	memset(data, 0x0, data_len);
	gettimeofday((struct timeval *)data, 0);
	icmp->icmp_cksum = checksum((void *)send_buff, len);
	sendto(sockfd, send_buff, len, 0, pr.sasend, pr.salen);
	if (flood)
		write(1, ".", 1);
}