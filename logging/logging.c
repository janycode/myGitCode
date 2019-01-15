#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define LOG_FILE_MAX_SIZE (5240880)    // 5M
#define LOG_FILE "./test.log"

unsigned long log_writing(char *path, char *content, FILE *p)
{
	unsigned long writen_size = -1;
	//int fd = 0;
	FILE *fp = NULL;

	//fd = open (path, O_WRONLY|O_CREAT|O_TRUNC, 0775);
	if (access (path, 0) != 0) {
		fp = fopen(path, "w+"); 
	} else {
		fp = fopen(path, "a+"); 
	}
	//if (fd < 0) {
	if (fp == NULL) {
		printf ("open fail\n");
		return writen_size;
	}
	p = fp;
	//writen_size = write(fd, content, strlen(content));
	writen_size = fwrite(content, sizeof(char), strlen(content), fp);
	if (writen_size <= 0) {
		printf ("writen null or error\n");
		return -1;
	}
	//close(fd);
	fclose(fp);

	return writen_size;
}

unsigned long get_file_size(const char *path)
{
	unsigned long filesize = -1;
	FILE *fp;
	fp = fopen(path, "r");
	if(fp == NULL)  {
		printf ("open :%s FAIL\n", path);
		return filesize;
	}
	fseek(fp, 0L, SEEK_END);
	filesize = ftell(fp);
	fclose(fp);
	return filesize;
}

int move_file_point(FILE *fp, unsigned long file_size, unsigned long writ_size)
{
	//FILE *fp;
	//fp = fopen(path, "r");
	if (fp = NULL) {
		printf ("fp is null\n");
		return -1;
	}
	fseek(fp, file_size + writ_size - LOG_FILE_MAX_SIZE, SEEK_CUR);

	return 0;
}

int main (void) {
	int i = 0;
	int file_size = 0;
	int writen_size = 0;
	char buff[32] = {0};
	char *p_buf = "This is a test content\n";
	FILE *fp = NULL;

	while (1) {
		i++;
		sprintf (buff, "%d ", i);
		strncat (buff, p_buf, strlen(p_buf));
		writen_size += log_writing (LOG_FILE, buff, fp);
		printf ("WRITEN: %d lines\n", writen_size);
		if (writen_size >= LOG_FILE_MAX_SIZE) {
			file_size = get_file_size(LOG_FILE);
			//move_file_point(fp, file_size, writen_size);
			printf ("file size = %d\n", file_size);
			break;
		}
	}

	return 0;
}
