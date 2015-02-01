#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 3)
    {
        printf("Usage: whodunnit cluefile \n");
        return 1;
    }
    char *infile = argv[1];
    char *outfile = argv[2];
    // open input file 
    FILE* clue_ptr = fopen(infile, "r");
    if (clue_ptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE* solution_ptr = fopen(outfile, "w");
    if (solution_ptr == NULL)
    {
        fclose(clue_ptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, clue_ptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, clue_ptr);

    // ensure infile is probably a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(solution_ptr);
        fclose(clue_ptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, solution_ptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, solution_ptr);

    // determine padding for scanlines
    int padding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        // iterate over pixels in scanline
        for (int j = 0; j < bi.biWidth; j++)
        {
            // temporary storage
            RGBTRIPLE triple;

            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, clue_ptr);

            // write RGB triple to outfile
            if(triple.rgbtRed == 255 && triple.rgbtGreen == 0 && triple.rgbtBlue == 0)
            {
              triple.rgbtRed = 0;
            }
            if(triple.rgbtRed == 255 && triple.rgbtGreen == 225 && triple.rgbtBlue == 255)
            {
		        triple.rgbtRed = 0;
  		        triple.rgbtBlue = 0;
		        triple.rgbtGreen = 0;
            } 
            fwrite(&triple, sizeof(RGBTRIPLE), 1, solution_ptr);
	    
	    
        }

        // skip padding
        fseek(clue_ptr, padding, SEEK_CUR);

        // then padding
        for (int k = 0; k < padding; k++)
            fputc(0x00, solution_ptr);
    }

    // close infile
    fclose(clue_ptr);

    // close outfile
    fclose(solution_ptr);

    // done-zo
    return 0;
}
