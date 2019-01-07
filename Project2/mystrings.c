#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
	
	char *filename = argv[1];
	
	FILE *inFile = fopen(filename, "rb");
	
	char currChar;
	int i;
	
	char buffer[3];
	int buffSize = sizeof(buffer)/ sizeof(char);
	int strLength = 0;
	int first = 1;
	
	while(!feof(inFile)){
		
		fread(&currChar, sizeof(char), 1, inFile);
		int charInt = (int) currChar;
		
		if((charInt >= 32 && charInt <= 126) || charInt == 9){
			strLength++;
			if(strLength < 4){
				buffer[strLength - 1] = currChar;
			}
			else if(strLength == 4){
				
				if(first == 1){
					first = 0;
				}
				else{
					printf("\n");
				}		
				
				for(i = 0;i<buffSize;i++){
					printf("%c", buffer[i]);
				}
				printf("%c", currChar);
			}
			else{
				printf("%c", currChar);
			}
		}
		else{
			strLength = 0;
			
			//clear buffer
			for(i = 0;i<buffSize;i++){
				buffer[i] = '\0';
			}			
		}		
	}
	
	fclose(inFile);
	return 0;
}