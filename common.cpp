//----------------------------------------------------------------------------------------
// Copyright: See definitions.h for License
//----------------------------------------------------------------------------------------
#include "common.h"
//----------------------------------------------------------------------------------------
string IntToStr(const int & param)
{
  ostringstream myStream; 
  myStream << param << flush;
  
  return(myStream.str());
}
//----------------------------------------------------------------------------------------
string FloatToStr(const float & param)
{
  ostringstream myStream; 
  myStream << fixed << setprecision(3);
  myStream << fixed << param << flush;
  
//  cout << "<[ float: " << param << " , string: " << myStream.str() << " ]>";
  
  return(myStream.str());
}
//----------------------------------------------------------------------------------------
string ByteToHex(const int & param)
{
  char table[]="0123456789abcdef";
  int high = (param & 0xF0) >> 4;
  int low = (param & 0x0F);
  
  string ret = "";
  ret += table[high];
  ret += table[low];
  
  return ret;
}
//----------------------------------------------------------------------------------------
string ByteToBin(const int & param)
{
  char table[]="01";
  int high = (param & 0xF0) >> 4;
  int low = (param & 0x0F);
  
  string ret = "";
  ret += table[(param & 0x80) >> 7];
  ret += table[(param & 0x40) >> 6];
  ret += table[(param & 0x20) >> 5];
  ret += table[(param & 0x10) >> 4];
  ret += " ";
  ret += table[(param & 0x08) >> 3];
  ret += table[(param & 0x04) >> 2];
  ret += table[(param & 0x02) >> 1];
  ret += table[(param & 0x01)];
  
  return ret;
}
//----------------------------------------------------------------------------------------
bool CompareFP(const float &x, const float &y)
{
        return (x>y-GL_FPPRECISION) && (x<y+GL_FPPRECISION);
}
//----------------------------------------------------------------------------------------
