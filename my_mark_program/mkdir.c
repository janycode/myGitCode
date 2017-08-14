#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

int CreateDir(const char *sPathName) 
{ 
	char DirName[256]; 
	strcpy(DirName, sPathName); 
	int i,len = strlen(DirName); 
	if(DirName[len-1]!='/') 
		strcat(DirName, "/"); 

	len = strlen(DirName); 

	for(i=1; i<len; i++) 
	{ 
		if(DirName[i]=='/') 
		{ 
			DirName[i] = 0; 
			if( access(DirName, R_OK | W_OK)!=0 ) 
			{ 
				if(mkdir(DirName, 0755)==-1) 
				{ 
					perror("mkdir error"); 
					return -1; 
				} 
			} 
			DirName[i] = '/'; 
		} 
	} 

	return 0; 
}

int main (void)
{
	CreateDir ("hello/world/and/hello/kitty");
	return 0;
}
