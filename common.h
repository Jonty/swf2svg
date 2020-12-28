//----------------------------------------------------------------------------------------
// Copyright: See definitions.h for License
//----------------------------------------------------------------------------------------
#ifndef _COMMON_
#define _COMMON_
//----------------------------------------------------------------------------------------
#include <string>            
#include <sstream>
#include <iostream>
#include <fstream>           
#include <iomanip>           
//----------------------------------------------------------------------------------------
using namespace std;         
//----------------------------------------------------------------------------------------
#define GL_FPPRECISION                  0.01                                       
#define CM_OUTMANIP_ENDL                100                                                
//----------------------------------------------------------------------------------------
string IntToStr(const int &);
string FloatToStr(const float &);
string ByteToHex(const int &);
string ByteToBin(const int &); 
bool CompareFP(const float &, const float &);
//----------------------------------------------------------------------------------------
#endif /*_COMMON_*/
//----------------------------------------------------------------------------------------
