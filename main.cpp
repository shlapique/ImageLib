#include <iostream>
#include <string>
#include <unistd.h>
#include <fstream>
#include <ctime>
#include <utility>
#include <string>
#include <vector>
#include <cmath>

#include "bmp.h"

using std::cout;
using std::cin;
using std::endl;
using std::ofstream;
using std::ifstream;
using std::string;
using std::vector;

int rows; 
int cols;

struct Pixel 
{
	unsigned char** r;
	unsigned char** g;
	unsigned char** b;

	char** delta; // later for diff between denc

	int i_max; //coords of max delta
	int j_max;

}pixel;

void PicToText(string FileName)
{
	ofstream write(FileName);
    	if (!write) 
    	{
        	cout << "Failed to write in TEXXT FILE " << FileName << endl;
        	return;
    	}

	/*
    	char** buf = new char* [rows];
    	for (int i = 0; i < rows; i++)
        	buf[i] = new char[cols];	

	vector <char> dic = {'N', '@', '#', 'W', '$', '9', '8', '7',
                '6', '5', '4', '3', '2', '1', '0', '?', '!', 'a', 'b', 'c',
                ';', ':', '+', '=', '-', ',', '.', '_'};
	*/

	vector <vector<char>> buf(rows, vector<char> (cols)); // vector of vectors 
	string dic1 = "N@#W$9876543210?!abc;:+=-,._";
	string dic = "@#OP%+=|i-:. ";

	int metric = 256 / dic.size(); // 
	cout << "metric: " << metric << "\n";	
	for(int i = 0; i < rows; ++i)
	{
		for(int j = 0; j < cols; ++j)
		{
			unsigned char GScale = (pixel.r[i][j] + pixel.g[i][j] + pixel.b[i][j]) / 3;	
			int simbol = GScale / metric;
			buf[i][j] = dic[simbol];
		}
	}

	// print in file
	for(int i = 0; i < rows; ++i)
	{
		for(int j = 0; j < cols; ++j)
		{
			write << buf[i][j];
		}
		write << "\n";
	}	

}

void EdgeSearchNaive()
{
	char min = 0;
	char max = 0xff;

	for(int i = 0; i < rows; ++i)
	{
		for(int j = 0; j < cols; ++j)
		{
			if(pixel.r[i][j] > 0x80)
				pixel.r[i][j] = pixel.g[i][j] = pixel.b[i][j] = max;
			if(pixel.r[i][j] < 0x80) 
				pixel.r[i][j] = pixel.g[i][j] = pixel.b[i][j] = min;
			if(pixel.r[i][j] == 0x80)
				pixel.r[i][j] = pixel.g[i][j] = pixel.b[i][j] = 0x80;
		}

	}
}

void AverageBri(int n, int m, vector <unsigned char> &vec)
{	
    unsigned char max = 0;
    unsigned char min = 0xff;
    //unsigned char aver;
    unsigned char mini_max;
    //long sum = 0;
    
	for(int k = 0; k < rows / n; ++k)
	{
		for(int l = 0; l < cols / m; ++l)
		{ //
			for(int i = k * n + 1; i < (k + 1) * n - 1; ++i)
			{
				for(int j = l * m + 1; j < (l + 1) * m - 1; ++j)
				{
                    if(pixel.r[i][j] > max)
                    {
                        max = pixel.r[i][j];
                    }
                    if(pixel.r[i][j] < min)
                    {
                        min = pixel.r[i][j];
                    }
				    //sum += pixel.r[i][j];
				}
			}
            //aver = sum / n * m;
            mini_max = max - min;
            max = 0;
            min = 0xff;
            vec.push_back(mini_max);
		}	
	}	
}


void uDotFilter(int n, int m)
{
    long sumR = 0;
    long sumG = 0;
    long sumB = 0;
    unsigned char averR;
    unsigned char averG;
    unsigned char averB;

	for(int k = 0; k < rows / n; ++k)
	{
		for(int l = 0; l < cols / m; ++l)
		{
			for(int i = k * n; i < (k + 1) * n; ++i)
			{
				for(int j = l * m; j < (l + 1) * m; ++j)
				{
				    sumR += pixel.r[i][j];
				    sumG += pixel.g[i][j];
				    sumB += pixel.b[i][j];
				}
			}

            averR = sumR / n * m;
            averG = sumG / n * m;
            averB = sumB / n * m;
			for(int i = k * n; i < (k + 1) * n; ++i)
			{
				for(int j = l * m; j < (l + 1) * m; ++j)
                {
                    pixel.r[i][j] = averR;
                    pixel.g[i][j] = averG;
                    pixel.b[i][j] = averB;
				}
			}
            sumR = 0; 
            sumG = 0;
            sumB = 0;
		}	
	}	
}


void SubmatrixSearch()
{
	int n = 16;
	int m = 16;
	int diff = 30;
    
	cout << "ENTER: n, m, diff \n";
	//cin >> n >> m >> diff;

	if((rows < n) | (cols < m))
	{
		cout << "Change SubmatrixSrearch size ! \n";
		return;
	}

	if((rows * cols) % (n * m) == 0)
	{
		cout << "Vse s kaifom @ \n";
	}
	else
		return; // POKA CHTO !

	vector <Pixel> raznost;
	vector <unsigned char> average; 

    AverageBri(n, m, average);

	char max_delta = 0; //in abs	
	for(int k = 0; k < rows / n; ++k)
	{
		for(int l = 0; l < cols / m; ++l)
		{   
            /*
            if((k - 1 > 0) && (l - 1 > 0) && (k + 1 < rows) && (l + 1 < cols)) // ideal case
            {
                int summa = average[k * n + l - m] + average[k * n + l + m] + average[k * n + l - 1] + average[k * n + l + 1];
                char sosedi = summa / 4;
                //if(average[k * n + l] + std::abs(average[k * n + l] - sosedi) > sosedi)
                if(average[k * n + l] + 10 > sosedi)
                {
                    diff = 50;
                }
                else
                {
                    diff = 10;
                }
            }  
            */
            if(average[k * (cols / m) + l] > 80)
            {
                //cout << "k * cols / m + l = " << k * (cols / m) + l << "\n";
                //cout << "average[k * (cols / m) + l] = " << (int)average[k * (cols / m) + l] << "\n";
			for(int i = k * n + 1; i < (k + 1) * n - 1; ++i)
			{
				for(int j = l * m + 1; j < (l + 1) * m - 1; ++j)
				{
					//pixel.delta[i][j] = std::sqrt(((pixel.r[i][j + 1] - pixel.r[i][j - 1]) / 1 * 2) * ((pixel.r[i][j + 1] - pixel.r[i][j - 1]) / 1 * 2) + ((pixel.r[i + 1][j] - pixel.r[i - 1][j]) / 1 * 2) * ((pixel.r[i + 1][j] - pixel.r[i - 1][j]) / 1 * 2));
					pixel.delta[i][j] = std::abs(((pixel.r[i][j + 1] - pixel.r[i][j - 1]) / 1 * 2));
					if(pixel.delta[i][j] > max_delta + diff)
					{
						raznost.clear();
						max_delta = pixel.delta[i][j];
						raznost.push_back(pixel);
						raznost.back().i_max = i;
						raznost.back().j_max = j;
					}
				}
					
				for(vector <Pixel>::iterator it = raznost.begin(); it != raznost.end(); ++it)
				{
					it -> r[it -> i_max][it -> j_max] = 0;
					it -> g[it -> i_max][it -> j_max] = 0xff;
					it -> b[it -> i_max][it -> j_max] = 0xff;
				}
				max_delta = 0;		
			}

			for(int j = l * m + 1; j < (l + 1) * m - 1; ++j)
			{
				for(int i = k * n + 1; i < (k + 1) * n - 1; ++i)
				{
					pixel.delta[i][j] = std::abs(((pixel.r[i + 1][j] - pixel.r[i - 1][j]) / 1 * 2));
					if(pixel.delta[i][j] > max_delta + diff)
					{
						raznost.clear();
						max_delta = pixel.delta[i][j];
						raznost.push_back(pixel);
						raznost.back().i_max = i;
						raznost.back().j_max = j;
					}	
				}
					
				for(vector <Pixel>::iterator it = raznost.begin(); it != raznost.end(); ++it)
				{
					it -> r[it -> i_max][it -> j_max] = 0;
					it -> g[it -> i_max][it -> j_max] = 0xff;
					it -> b[it -> i_max][it -> j_max] = 0xff;
				}
				max_delta = 0;		
			}
            }

		}	

	}	

}

// Fisher - Yates shuffle	
void FisherShuffle()
{
	std::srand(time(NULL));
	for(int i = rows - 1; i >= 1; --i)
	{
		for(int j = cols; j >= 1; --j)
		{
			int n = rand() % (i + 1);
			int m = rand() % (j + 1);
			std::swap(pixel.r[i][j], pixel.r[n][m]);	
			std::swap(pixel.g[i][j], pixel.g[n][m]);			
			std::swap(pixel.b[i][j], pixel.b[n][m]);			
		}
	}
}

void PixelDownshift()
{
	for(int i = 0; i < rows; ++i)
	{
		for(int j = 0; j < cols; ++j)
		{
			unsigned char tmpPixel = pixel.r[i][j];
			pixel.r[i][j] = pixel.g[i][j];
			pixel.b[i][j] = tmpPixel;
		}
	}
}

void AlienFilter() 
{
	for(int i = 0; i < rows; ++i)
	{
		for(int j = 0; j < cols; ++j)
		{
			if(pixel.r[i][j] <= 0x3c && pixel.g[i][j] <= 0x3c && pixel.b[i][j] <= 0x3c)
			{
				pixel.r[i][j] = 0;
				pixel.g[i][j] = 0;
				pixel.b[i][j] = 0xff;
			}
			if(pixel.r[i][j] <= 0x64 && pixel.g[i][j] <= 0x64 && pixel.b[i][j] <= 0x64)
			{
				pixel.r[i][j] = 0x84;
				pixel.g[i][j] = 0xde;
				pixel.b[i][j] = 2;
			}
			
			if(pixel.r[i][j] <= 0xdc && pixel.g[i][j] <= 0xdc && pixel.b[i][j] <= 0xdc)
			{
				pixel.r[i][j] = 0x99;
				pixel.g[i][j] = 0x66;
				pixel.b[i][j] = 0xcc;
			}
		}
	}	
}

void PaintTest()
{
    	for (int i = rows / 10; i < 3 * rows / 10; i++)
	{
        	for (int j = cols / 10; j < 7 * cols / 10; j++)
		{
            		pixel.r[i][j] = 0xff;
	    		pixel.g[i][j] = 0;
	    		pixel.b[i][j] = 0;
		}
	}

    	for (int i = 8 * rows / 10; i < rows; i++)
        	for (int j = 8 * cols / 10; j < cols; j++)
            		pixel.g[i][j] = 0xff;

    	for (int i = rows * 4 / 10; i < rows * 6 / 10; i++)
	{
        	for (int j = cols * 4 / 10; j < cols * 6 / 10; j++)
		{
            		pixel.g[i][j] = 0xff;
            		pixel.r[i][j] = 0xff;
            		pixel.b[i][j] = 0xff;
        	}
	}

    	for (int i = rows * 6 / 10; i < rows; i++)
        	for (int j = cols * 0; j < cols * 1 / 10; j++)
            		pixel.b[i][j] = 0xff;

	for(int i = 0; i < rows / 2; ++i)
	{
		for(int j = 0; j < cols / 10; ++j)
		{
			pixel.r[i][j] = 0x0;
			pixel.b[i][j] = 0x0;
		}

	}
}

void GScale()
{
	for(int i = 0; i < rows; ++i)
	{
		for(int j = 0; j < cols; ++j)
		{
			unsigned char GScale = (pixel.r[i][j] + pixel.g[i][j] + pixel.b[i][j]) / 3;	
			pixel.r[i][j] = pixel.g[i][j] = pixel.b[i][j] = GScale;
		}
	}
}

// allocate memory for array of pixels
void RGB_Allocate(unsigned char** &dude) 
{
    dude = new unsigned char* [rows];
    for (int i = 0; i < rows; i++)
        dude[i] = new unsigned char[cols];
}

void Delta_Allocate(char** &dude)
{
    dude = new char* [rows];
    for (int i = 0; i < rows; i++)
        dude[i] = new char[cols];
}

//Returns 1 if executed sucessfully, 0 if not sucessfull
bool FillAndAllocate(char* &buffer, string Picture, int& rows, int& cols, int& BufferSize) 
{
	ifstream file(Picture);

    	if (file) 
	{
		//get length of the file
        	file.seekg(0, std::ios::end);
        	std::streampos length = file.tellg();
        	file.seekg(0, std::ios::beg);
		//
		buffer = new char[length];
        	file.read(&buffer[0], length);

        	PBITMAPFILEHEADER file_header;
        	PBITMAPINFOHEADER info_header;

        	file_header = (PBITMAPFILEHEADER)(&buffer[0]);
        	info_header = (PBITMAPINFOHEADER)(&buffer[0] + sizeof(BITMAPFILEHEADER));

		// counting the number of rows and columns
        	rows = info_header-> biHeight;
        	cols = info_header-> biWidth;

        	BufferSize = file_header-> bfSize;
        	return 1;
    	}
    	else 
	{
       		cout << "File" << Picture << " don't Exist!" << endl;
        	return 0;
    	}
}

//Returns 1 if executed sucessfully, 0 if not sucessfull
void GetPixelsFromBMP24(
		struct Pixel,
		int end, 
		int rows, 
		int cols, 
		char* FileReadBuffer) 
{ // end is BufferSize (total size of file)
    	int count = 1;
	int extra = cols % 4; // The nubmer of bytes in a row (cols) will be a multiple of 4.
    	for (int i = 0; i < rows; i++)
	{
		count += extra;
    		for (int j = cols - 1; j >= 0; j--)
        		for (int k = 0; k < 3; k++) 
			{
                		switch (k) 
				{
                			case 0:
                    				pixel.r[i][j] = FileReadBuffer[end - count++];
                    				break;
                			case 1:
                    				pixel.g[i][j] = FileReadBuffer[end - count++];
                    				break;
                			case 2:
                    				pixel.b[i][j] = FileReadBuffer[end - count++];
                    				break;
                		}
            		}
	}
}

void WriteOutBmp24(
		char* FileBuffer, 
		string NameOfFileToCreate, 
		int BufferSize) 
{
    ofstream write(NameOfFileToCreate);
    if (!write) 
    {
        cout << "Failed to write " << NameOfFileToCreate << endl;
        return;
    }
    int count = 1;
    int extra = cols % 4; // The nubmer of bytes in a row (cols) will be a multiple of 4.
    for (int i = 0; i < rows; i++)
    {
	    count += extra;
	    for (int j = cols - 1; j >= 0; j--)
		    for (int k = 0; k < 3; k++)
		    {
			    switch (k) 
			    {
				    case 0: //red
					    FileBuffer[BufferSize - count] = pixel.r[i][j];
					    break;
				    case 1: //green
					    FileBuffer[BufferSize - count] = pixel.g[i][j];
					    break;
				    case 2: //blue
					    FileBuffer[BufferSize - count] = pixel.b[i][j];
					    break;
			    }
			    count++;
		    }
    }
    write.write(FileBuffer, BufferSize);
}


int main(int args, char** cat) 
{
	char* FileBuffer; int BufferSize;
	/////////
	char* FileBuf;
	#define TextFileName "output.txt"
	/////////
	#define Picture "Shapes.bmp"
	#define WriteOutFile "out.bmp"

	if (!FillAndAllocate(FileBuffer, Picture, rows, cols, BufferSize))
	{
		cout << "File read error" << endl; 
		return 0;
	}

	cout << "Rows: " << rows << " Cols: " << cols << endl;

	RGB_Allocate(pixel.r);
	RGB_Allocate(pixel.g);
	RGB_Allocate(pixel.b);

	/*------*/
	Delta_Allocate(pixel.delta);
	/*------*/	
	GetPixelsFromBMP24(pixel, BufferSize, rows, cols, FileBuffer);

	//PicToText(TextFileName);
	//
	//GScale();	
	//SubmatrixSearch();	
	//	
	//AlienFilter();
    uDotFilter(2, 2);

	WriteOutBmp24(FileBuffer,  WriteOutFile,BufferSize);
	
    return 1;
}
