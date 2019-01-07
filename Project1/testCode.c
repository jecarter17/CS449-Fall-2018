#include <stdio.h>
#include <stdlib.h>
#pragma pack(1)

struct BMP {
    char filetype[2]; // must be BM, must check if BM
    unsigned int filesize;
    short reserved1;
    short reserved2;
    unsigned int dataoffset;

};

struct DIB {
    unsigned int fileheader;
    unsigned int headersize;
    int width;
    int height;
    short planes;
    short bitsperpixel; /* we only support the value 24 here */
    unsigned int compression; /* we do not support compression */
    unsigned int bitmapsize;
    int horizontalres;
    int verticalres;
    unsigned int numcolors;
    unsigned int importantcolors;
};

struct PIXEL {
    unsigned char b;
    unsigned char g;
    unsigned char r;

};

void invertImage(char fileName[]) {

    struct BMP bmp;
    struct DIB dib;
    struct PIXEL pixel;

    FILE *fp = fopen(fileName, "rb+");

    //FileType
    fread(bmp.filetype, 1, 2, fp);
    printf("Value is %c\n", bmp.filetype[1]);

    //Check if file format is BM
    if (bmp.filetype[0] != 'B' && bmp.filetype[1] != 'M') {
        printf("File format not supported");
        exit(-1);
    }
    //Size of the file in bytes
    fread(&bmp.filesize, 4, 1, fp);
    printf("Value is %d\n", bmp.filesize);

    //Go to dataoffset
    fseek(fp, 4, SEEK_CUR);

    fread(&bmp.dataoffset, 4, 1, fp);
    printf("Offset is %d\n", bmp.dataoffset);


    fread(&dib.fileheader, 4, 1, fp);
    printf("File header is %d bytes\n", dib.fileheader);

    if (dib.fileheader != 40) {
        printf("File format not supported");
        exit(-1);
    }

    fread(&dib.width, 4, 1, fp);
    printf("Width is %d\n", dib.width);

    fread(&dib.height, 4, 1, fp);
    printf("Height is %d\n", dib.height);

    fread(&dib.planes, 2, 1, fp);
    printf("Color planes is %d\n", dib.planes);

    //Number of bits per pixel
    fread(&dib.bitsperpixel, 2, 1, fp);
    printf("Pixels per bit is %d\n", dib.bitsperpixel);


    fread(&dib.compression, 4, 1, fp);
    printf("Compression scheme used is %d\n", dib.compression);

    fread(&dib.bitmapsize, 4, 1, fp);
    printf("Image size is %d\n", dib.bitmapsize);

    fread(&dib.horizontalres, 4, 1, fp);
    printf("Horizontal resolution is %d\n", dib.horizontalres);

    fread(&dib.verticalres, 4, 1, fp);
    printf("Vertical resolution is %d\n", dib.verticalres);

    fread(&dib.numcolors, 4, 1, fp);
    printf("Number of colors used %d\n", dib.numcolors);

    fread(&dib.importantcolors, 4, 1, fp);
    printf("Important colors used %d\n", dib.importantcolors);

    //fseek(fp,3,SEEK_CUR);

    int x = 0;
    while (x < dib.width) {
        int y = 0;
        while (y < dib.height) {

            fread(&pixel.b, 1, 1, fp);
            unsigned int blue = pixel.b;
            blue = ~blue;
            pixel.b = (char) blue;
            //printf("Pixel 1 is %d\n", pixel.b);

            fread(&pixel.g, 1, 1, fp);
            unsigned int green = pixel.g;
            green = ~green;
            pixel.g = (char) green;
            //printf("Pixel 2 is %d\n", pixel.g);

            fread(&pixel.r, 1, 1, fp);
            unsigned int red = pixel.r;
            red = ~red;
            pixel.r = (char) red;
            //printf("Pixel 3 is %d\n", pixel.r);

            fseek(fp, -3, SEEK_CUR);
            fwrite(&pixel.b, 1, 1, fp);
            fwrite(&pixel.g, 1, 1, fp);
            fwrite(&pixel.r, 1, 1, fp);

            y++;
        }
        //  fseek(fp,1,SEEK_CUR);

        x++;
    }

    fclose(fp);
}

int main(int argc, char *argv[]) {

    printf("Program name %s\n", argv[0]);
   if( strcmp(argv[1],"-invert") == 0) {

    printf("Invert\n");
    printf("File name is %s\n", argv[2]);

    invertImage(argv[2]);
}
    return 0;
}