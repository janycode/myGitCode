#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define PATH1 "/tmp/test_file1"
#define PATH2 "/tmp/test_file2"
#define PATH3 "/tmp/test_file3"

typedef struct device_file {
	char *file_name;
	int	  fd;
} device_file_t;

static device_file_t g_device_file[] =
{
	{PATH1, 0},
	{PATH2, 0},
	{PATH3, 0},
};

int main (void)
{
	int fd = 0;
	int fd_max = 0;
	fd_set fd_sets;
	fd_set handle_sets;
	int rs = 0;
	int i = 0;
	int index = 0;
	char buff[128] = {0};

	FD_ZERO (&fd_sets);
	for (index = 0; index < sizeof (g_device_file)/sizeof (device_file_t); index++)
	{
		fd = 0;
		fd = open (g_device_file[index].file_name, O_RDWR | O_CREAT, 0664);
		if (fd <= 0)
			perror ("open:");

		g_device_file[index].fd = fd;
		FD_SET (fd, &handle_sets);
		if (fd > fd_max) {
			fd_max = fd;
		}
	}

	while (1) {
		if (i >= 1)
			break;
		fd_sets = handle_sets; 
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
			for (index = 0; index < sizeof (g_device_file)/sizeof (device_file_t); index++) {
				fd = g_device_file[index].fd;
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
	}

	return 0;
}
