#include "ft_ping.h"

void	readloop(void)
{
	int 			size;
	struct msghdr	msg;
	struct iovec	iov;
	struct timeval	tval;
	ssize_t			n;
	char 			recv_buff[BUFF_SIZE];
	char 			control_buff[BUFF_SIZE];

	size = 60 * 1024;
	if ((sockfd = socket(pr.sasend->sa_family, SOCK_RAW, pr.icmp_proto)) == -1
	|| setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size))) // set the socket receive buffer size to 61,440 bytes (60 x 1024) for  IPv4 broadcast address or a multicast address)
	{
		dprintf(2, "error with the socket: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	setuid(getuid()); //delete all permission and go to real user ID of calling process
	sig_alarm(SIGALRM);
	iov.iov_base = recv_buff;
	iov.iov_len = sizeof(recv_buff);

	msg.msg_control = control_buff;
	msg.msg_flags = 0;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_name = pr.sarecv;
	while(42)
	{
		msg.msg_namelen = pr.salen;
		msg.msg_controllen = sizeof(control_buff);
		n = recvmsg(sockfd, &msg, 0);
		if (n < 0)
		{
			if (errno == EINTR) //for prevent alarm signal
				continue ;
			else
				dprintf(2, "recvmsg error\n");
		}
		gettimeofday(&tval, NULL);
		(*pr.fproc)(n, &msg, &tval);
		nrecv++;
	}
}