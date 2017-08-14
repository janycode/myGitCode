#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define PATH "/tmp/test_file"

int main (void)
{
	int fd = 0;
	int fd_max = 0;
	fd_set fd_sets;
	fd_set handle_sets;
	int rs = 0;
	char buff[128] = {0};
	int i = 0;

	fd = open (PATH, O_RDWR | O_CREAT, 0664);
	if (fd <= 0)
		perror ("open:");

	FD_ZERO (&fd_sets);
	FD_SET (fd, &handle_sets);
	if (fd > fd_max) {
		fd_max = fd;
	}
	while (1) {
		if (i >= 1)
			break;
		fd_sets = handle_sets; // 保存handle_sets
		rs = select (fd_max + 1, &fd_sets, NULL, NULL, NULL);
		if(rs < 0)
		{
			if(errno == EINTR)
			{
				continue;
			}
			else
			{
				break;
			}
		}
		else if(rs == 0)
		{
			continue;
		}
		else
		{
			if (FD_ISSET (fd, &fd_sets)) {
				rs = read (fd, buff, sizeof (buff));
				if (rs <= 0) {
					continue;
				}
				printf ("buff = %s\n", buff);
				lseek (fd, 0, SEEK_SET);
				i++;
			}
		}
	}

	return 0;
}
