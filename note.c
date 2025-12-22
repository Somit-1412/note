#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

int main(int argc, char *argv[]){
	char path[200];
	snprintf(path,sizeof(path),"%s/.notes",getenv("HOME"));
	int counter=1;
	FILE *f;
	char line[200];
	int i;
	
	if (argc < 2) {
    		printf("Usage:\n");
		printf("	note text...\n");
		printf("	note -l:	List saved notes\n");
		printf("	note -c:	Clear notes\n");
		printf("	note -d n:	Delete nth note\n");
		return 0;
	}


	//commands
	//if user types: note --help
	if(argc == 2 && strcmp(argv[1],"--help") == 0){
		printf("Usage:\n");
		printf("	note text...\n");
		printf("	note -l:	List saved notes\n");
		printf("	note -c:	Clear notes\n");
		printf("	note -d n:	Delete nth note\n");
		return 0;
	}

	//if user types: note -l
	else if(argc == 2 && strcmp(argv[1],"-l") == 0){
		f = fopen(path,"r");
		if(f == NULL){
			printf("No notes yet\n");
			return 0;
		}

		while(fgets(line,200,f) != NULL){
			printf("%d. %s",counter,line);
			counter++;
		}

		fclose(f);
		return 0;
	}

	//if user types: note -c
	else if(argc == 2 && strcmp(argv[1],"-c") == 0){
		char choice[10];
		while(1){
			printf("Clear notes?[y/n]: ");
			fflush(stdout);

			if(fgets(choice,sizeof(choice),stdin) == NULL){
				return 0;
				}

			//remove newline
			choice[strcspn(choice,"\n")] = '\0';

			//convert to lower case
			for(int i=0;choice[i];i++){
				choice[i] = tolower((unsigned char)choice[i]);
				}

			if(strcmp(choice,"y") == 0 || strcmp(choice,"yes") == 0){
				f = fopen(path,"w");
				fclose(f);
				return 0;
				}

			if(strcmp(choice,"n") == 0 || strcmp(choice,"no") == 0){
				return 0;
				}
			printf("Enter [y/n]: ");
			fflush(stdout);
		}
	}

	//if user types: note -d n
	else if(argc == 3 && strcmp(argv[1],"-d") == 0){
		int target = atoi(argv[2]);
		if(target <= 0){
			printf("Invalid.\n");
			return 1;
		}

		FILE *input = fopen(path,"r");
		if(input == NULL){
			printf("Notes empty.\n");
			return 1;
		}

		char temp_path[220];
		snprintf(temp_path,sizeof(temp_path),"%s.tmp",path);

		FILE *output = fopen(temp_path,"w");
		if(output == NULL){
			printf("Error.");
			fclose(input);
			return 1;
		}

		char buffer[200];
		int line_number = 1;
		int deleted = 0;

		while(fgets(buffer,sizeof(buffer),input)){
			if(line_number != target){
				fputs(buffer,output);
			} else {
				deleted = 1;
			}
			line_number++;
		}
		fclose(input);
		fclose(output);

		if(!deleted){
			remove(temp_path);
			printf("No such note.\n");
			return 1;
		}

		rename(temp_path,path);
		return 0;

	}

	//handle unknown flags
	else if(argv[1][0] == '-'){
		printf("Unknown option: %s\n",argv[1]);
		return 1;
	}

	//if user types: note some text
	else {
		f = fopen(path,"a");
		if(f == NULL){
			printf("Error opening file\n");
			return 1;
		}

		for(i=1;i<argc;i++){
			fprintf(f,"%s ",argv[i]);
		}
		fprintf(f,"\n");
		fclose(f);
		return 0;
	}

	return 0;
}
