//----------------------------------------------------------------------------------------
// Copyright: See definitions.h for License
//----------------------------------------------------------------------------------------
#include "matrix.h"
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
Matrix::Matrix()
{
hasScale = false;
hasRotate= false;
   
translateX = 0;
translateY = 0;
scaleX = 1;
scaleY = 1;
rotateSkew0 = 0;
rotateSkew1 = 0;
}
//----------------------------------------------------------------------------------------
string Matrix::ToString()
{
        ostringstream myStream; 
        myStream << fixed << setprecision(3);
        myStream << scaleX << " " << rotateSkew0;
        myStream << " " << rotateSkew1 << " " << scaleY;
        myStream << " " << translateX << " " << translateY;
        myStream << flush;
        
        return (myStream.str());         
}
//----------------------------------------------------------------------------------------
