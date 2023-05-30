#include "ft_ping.h"

void	send_v4()
{
	int 			len;
	struct icmp		*icmp;
	unsigned char	*data;

	icmp = (struct icmp *)send_buff;
	icmp->icmp_code = 0;
	icmp->icmp_type = ICMP_ECHO;
	icmp->icmp_hun.ih_idseq.icd_seq = nsent++;
	icmp->icmp_hun.ih_idseq.icd_id = pid;
	data = send_buff + HEADER_ICMP;
	memset(send_buff + HEADER_ICMP, 0, data_len);
	len = HEADER_ICMP + data_len;
	icmp->icmp_cksum = ;//todo implement checksum ;
	sendto(sockfd, send_buff, len, 0, pr.sasend, pr.salen);
}