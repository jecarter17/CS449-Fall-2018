#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double pow(double x, double y);

struct Pixel{
	unsigned char b;
	unsigned char g;
	unsigned char r;
};

int main(int argc, char **argv){
	
	//read in command line args
	
	char *operation = argv[1];
	int opCode;
	
	if((strcmp(operation, "-invert") != 0) && (strcmp(operation, "-grayscale") != 0)){
		printf("Invalid operation entered\n");
		exit(0);
	}
	else if(strcmp(operation, "-invert") == 0){
		opCode = 0;
	}else{
		opCode = 1;
	}
	
	char *filename = argv[2];
	
	printf("opCode = %d, filename = %s\n\n", opCode, filename);
	
	//struct for bmp header
	struct bmpHeader{
		char bm[2];		//magic #  (2 bytes)
		int fSize;		//4 bytes
		short resVal;	//2 bytes
		short resVal2;	//2 bytes
		int offset;		//4 bytes
	}bmp, *pbmp;
	
	//struct for DIB header
	struct dibHeader{
		int dibSize;	//4 bytes
		int imWidth;	//4 bytes
		int imHeight;	//4 bytes
		short colPanes;	//2 bytes
		short bpp;		//2 bytes
		int compSch;	//4 bytes
		int imSize;		//4 bytes
		int horRes;		//4 bytes
		int verRes;		//4 bytes
		int numCols;	//4 bytes
		int numImpCols;	//4 bytes
	}dib, *pdib;
	
	//open file
	FILE *inFile = fopen(filename, "r+b");
	
	//read in bmp header	
	fread(bmp.bm, sizeof(char), 2, inFile);	
	fread(&bmp.fSize, sizeof(int), 1, inFile);	
	fread(&bmp.resVal, sizeof(short), 1, inFile);	
	fread(&bmp.resVal2, sizeof(short), 1, inFile);	
	fread(&bmp.offset, sizeof(int), 1, inFile);
	
	//print bmp header contents (for checking)
	printf("Magic #: %c%c\n",bmp.bm[0],bmp.bm[1]);
	printf("File size: %d\n",bmp.fSize);
	printf("ResVal: %d\n",bmp.resVal);
	printf("ResVal2: %d\n",bmp.resVal2);
	printf("Offset: %d\n\n", bmp.offset);
	
	//check that first two bytes are "BM", if not display error message and exit
	if(bmp.bm[0] != 'B' || bmp.bm[1] != 'M'){
		printf("We do not support this file format\n");
		exit(0);
	}
	
	//read in dib header	
	fread(&dib.dibSize, sizeof(int), 1, inFile);	
	fread(&dib.imWidth, sizeof(int), 1, inFile);	
	fread(&dib.imHeight, sizeof(int), 1, inFile);		
	fread(&dib.colPanes, sizeof(short), 1, inFile);	
	fread(&dib.bpp, sizeof(short), 1, inFile);	
	fread(&dib.compSch, sizeof(int), 1, inFile);	
	fread(&dib.imSize, sizeof(int), 1, inFile);	
	fread(&dib.horRes, sizeof(int), 1, inFile);	
	fread(&dib.verRes, sizeof(int), 1, inFile);	
	fread(&dib.numCols, sizeof(int), 1, inFile);	
	fread(&dib.numImpCols, sizeof(int), 1, inFile);

	//print dib header contents (for checking)
	printf("DIB size: %d\n",dib.dibSize);
	printf("Image Width: %d\n",dib.imWidth);
	printf("Image Height: %d\n",dib.imHeight);
	printf("# Color Panes: %d\n", dib.colPanes);
	printf("# bits per pixel: %d\n", dib.bpp);
	printf("Compression scheme: %d\n",dib.compSch);	
	printf("Image size in bytes: %d\n",dib.imSize);
	printf("Horizontal Resolution: %d\n",dib.horRes);
	printf("Vertical Resolution: %d\n",dib.verRes);
	printf("# colors: %d\n",dib.numCols);
	printf("# important colors: %d\n\n",dib.numImpCols);
	
	//check that DIB header size is 40 bytes, if not display error message and exit
	if(dib.dibSize != 40){
		printf("We do not support this file format\n");
		exit(0);
	}
	
	int bytespp = dib.bpp / 8;
	printf("Bytes per pixel = %d\n", bytespp);
	
	int numPads;
	if((dib.imWidth * bytespp) % 4 == 0)
		numPads = 0;
	else
		numPads = 4 - ((dib.imWidth * bytespp) % 4);
	
	printf("Pad bytes per row = %d\n", numPads);
	
	int rowBytes = (dib.imWidth * bytespp) + numPads;
	printf("%d bytes per row\n", rowBytes);
	
	int rowPix = rowBytes / bytespp;
	printf("%d pixels per row\n", rowPix);
	
	int numRows = dib.imSize / rowBytes;
	printf("%d rows of pixels\n", numRows);
	
	struct Pixel p;
	//jump to beginning of last row of image
	int i,j, countPix = 0;
	for(i = 0;i < numRows;i++){
		
		//fseek(inFile, (-1 * i * rowBytes), SEEK_END);
		
		for(j=0;j<rowPix;j++){
			
			fseek(inFile, (bmp.offset + (i * rowBytes) + (j * bytespp)), SEEK_SET);
			
			//read pixel fields
			fread(&p.b, sizeof(char), 1, inFile);	
			fread(&p.g, sizeof(char), 1, inFile);	
			fread(&p.r, sizeof(char), 1, inFile);
			
			//operate on pixel
			if(opCode == 0){
				
				//invert pixel
				p.b = ~(p.b);				
				p.g = ~(p.g);	
				p.r = ~(p.r);
				
			}else{
				
				//convert to grayscale
				double R = ((unsigned int) p.r) / 255.0;
				double G = ((unsigned int) p.g) / 255.0;
				double B = ((unsigned int) p.b) / 255.0;
				
				double y = (0.2126 * R) + (0.7152 * G) + (0.0722 * B);				
				double val;
				
				if(y <= 0.0031308){				
					val = 12.92 * y;
					
				}else{					
					val = (1.055 * pow(y,(1/2.4))) - 0.055;					
				}
								
				p.b = (unsigned char)(255 * val);
				p.g = (unsigned char)(255 * val);
				p.r = (unsigned char)(255 * val);
			}
			
			
			//adjust file pointer to proper location (start of current pixel)
			fseek(inFile, (-1 * bytespp), SEEK_CUR);
						
			//write pixel back to file
			fwrite(&p.b, sizeof(char), 1, inFile);	
			fwrite(&p.g, sizeof(char), 1, inFile);	
			fwrite(&p.r, sizeof(char), 1, inFile);
			
			countPix++;
		}
	}
	
	//close file stream
	fclose(inFile);
	
	return 0;
}