#include<unistd.h>
#include<string.h>
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>

void display_usage(){
	write(1,"Usage:\n",7);
	write(1,"1.note text...\n",15);
	write(1,"2.note -l\n",10);
	write(1,"3.note -c\n",10);
	write(1,"4.note -d n\n",12);
	write(1,"5.note --help\n",14);
	return;
}

int main(int argc, char *argv[]){
	//set up absolute paths
	char path[256];
	char temp_path[256];

	if(getenv("HOME")==NULL){
	write(2,"HOME not set\n",13);
	return 1;
	}

	snprintf(path,sizeof(path),"%s/.notes",getenv("HOME"));
	snprintf(temp_path,sizeof(temp_path),"%s/.temp",getenv("HOME"));

	int fd;
	char buff[256];

	// stdin mode: each line = one note
	if (argc == 1) {
	    int interactive=isatty(0);
	    ssize_t n;
	    fd = open(path, O_WRONLY | O_CREAT | O_APPEND, 0644);
	    if (fd < 0) {
		write(2, "note: ", 6);
		write(2, strerror(errno), strlen(strerror(errno)));
		write(2, "\n", 1);
		return 1;
	    }

	    char linebuf[256];
	    int pos = 0;

	if (interactive) {
        	write(1, "Enter notes (Ctrl-D to finish):\n", 32);
    	}

	    while ((n = read(0, buff, sizeof(buff))) > 0) {
		for (ssize_t i = 0; i < n; i++) {
		    if (buff[i] == '\n') {
		        if (pos > 0) {
		            write(fd, linebuf, pos);
		            write(fd, "\n", 1);
		            pos = 0;
		        }
		    } else {
		        if (pos < (int)sizeof(linebuf) - 1) {
		            linebuf[pos++] = buff[i];
		        }
		    }
		}
	    }

		//flush buffered data on EOF
		if(pos>0){
			write(fd,linebuf,pos);
			write(fd,"\n",1);
		}

	    if (n < 0) {
		write(2, "note: ", 6);
		write(2, strerror(errno), strlen(strerror(errno)));
		write(2, "\n", 1);
	    }

	    close(fd);
	    return 0;
	}


	//list notes
	if(argc==2 && strcmp(argv[1],"-l")==0){
		fd=open(path,O_RDONLY);
		if(fd<0){
			if(errno==ENOENT){
				write(1,"No notes yet\n",13);
				return 0;
			} else {
				write(2,"note: ",6);
				write(2,strerror(errno),strlen(strerror(errno)));
				write(2,"\n",1);
				return 1;
			}
		}
		int at_line_start=1;
		int line=1;
		ssize_t n;
		while((n=read(fd,buff,sizeof(buff)))>0){
			for(ssize_t i=0;i<n;i++){
				if(at_line_start){
					char num[16];
					char temp[16];

					int t=0;
					int len=0;

					int x=line;
					while(x>0){
						temp[t++]='0'+ (x%10);
						x/=10;
					}
					for(int j=t-1;j>=0;j--){
						num[len++]=temp[j];
					}
					num[len++]='.';
					num[len++]=' ';
					//num here is character array of line number, write() only prints characters

					write(1,num,len);
					at_line_start=0;
				}

				write(1,&buff[i],1);
				if(buff[i]=='\n'){
					at_line_start=1;
					line++;
				}
			}
		}
	return 0;
	}

	//clear .notes (-c)
	if(argc==2 && strcmp(argv[1],"-c")==0){
		fd=open(path,O_WRONLY | O_CREAT | O_TRUNC, 0644);
		close(fd);
		return 0;
	}

	//display usage (--help)
	if(argc==2 && strcmp(argv[1],"--help")==0){
		display_usage();
		return 0;
	}

	//delete specific note (-d n)
	if(argc==3 && strcmp(argv[1],"-d")==0){

		fd=open(path,O_RDONLY);
		int fd1=open(temp_path,O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if(fd<0 || fd1<0){
			write(2,"note :",6);
			write(2,strerror(errno),strlen(strerror(errno)));
			write(2,"\n",1);
			return 1;
		}
		int deleted=0;
		//convert argv[2] into integer
		int value=0;
		char *s=argv[2];
		while(*s){
			if(*s<'0' || *s>'9') return -1;
			value=value*10 + (*s-'0');
			s++;
		}

		int line_no=1;
		ssize_t n;
		while((n=read(fd,buff,sizeof(buff)))>0){
			for(int k=0;k<n;k++){
				if(line_no!=value){
					write(fd1,&buff[k],1);
				} else { deleted=1; }
				if(buff[k]=='\n') line_no++;
			}
		}
		if(!deleted){
			write(1,"No such note\n",13);
			remove(temp_path);
			return 0;
		}

		if(rename(temp_path,path)<0){
		write(2,"note: ",6);
		write(2,strerror(errno),strlen(strerror(errno)));
		write(2,"\n",1);
		return 1;
		}

		close(fd);
		close(fd1);
		return 0;
	}


	//add note
	if(argc>1){
	fd=open(path,O_WRONLY | O_CREAT | O_APPEND, 0644);

	if(fd<0){
		write(2,"note: ",6);
		write(2,strerror(errno),strlen(strerror(errno)));
		write(2,"\n",1);
		return 1;
	}

	for(int i=1;i<argc;i++){
		write(fd,argv[i],strlen(argv[i]));
		write(fd," ",1);
	}
	write(fd,"\n",1);

	}
	close(fd);
	return 0;
}
