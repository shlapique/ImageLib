#ifndef __BMPH__
#define __BMPH__

#include "bmp.h"
#include <string>

using std::string;

class Image
{
    private:
        char *FileBuffer;
        int BufferSize;
    public:
        Image(){} // default const
        Image(string Picture)
        {}
};

#endif
