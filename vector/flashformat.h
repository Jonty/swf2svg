//----------------------------------------------------------------------------------------
// Copyright: See definitions.h for License
//----------------------------------------------------------------------------------------
#ifndef __FlashFORMAT__
#define __FlashFORMAT__
//----------------------------------------------------------------------------------------
#include "../definitions.h"
#include "../zlib/zutil.h"
#include "vectorformat.h"
#include "shapespace.h"
#include <exception>
#include "font.h"
#include "text.h"
#include <map>
//----------------------------------------------------------------------------------------
#define DSH_VERBOSE false
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
class FlashFormat: public VectorFormat
{
      public:
             FlashFormat();                           // Constructor
             ~FlashFormat();                          // Destructor

             // Handles messages received by this later
             virtual bool IncommingEventHandler(const int &, const string &, int *, float *, void *);

             float TwipsToFloat(const int &);
             
      protected:

             int flashVersion;
             bool isCompressed;
             int uncompressedBuffPos, uncompressedBuffLength;
             char *uncompressedBuff; 
             char *buff;
             
             bool endTagReached;
             
             int pos;                           
             ifstream inFile;           
                 ofstream logFile;              
             int maxBuffLen;
             int parser_shapeID;    
             int parser_tagType;    
             int parser_fillIndexBits;
             int parser_lineIndexBits;

                         typedef map<int, Shape, less<int> > Dictionary;
             Dictionary dict;
             int shapeCount;
             
             FontMap fontMap;
             
             virtual void Parse();
             virtual void FlushOutput();
             
                         bool ReadUI8(const char *, int &, int &);
                         bool ReadUI16(const char *, int &, int &);
                         bool ReadSI16(const char *, int &, int &);
                         bool ReadSB(const char *, int &, const int &, int &);
                         bool ReadFB(const char *, int &, const int &, float &);
                         bool ReadRECT(const char *buff, int &, RECT &);
             bool ReadRGB(const char *, int &, RGB &, const int &tagType=MFL_TAG_DEFINESHAPE);
             bool ReadFillStyleArray(const char *, int &, Shape &);
             bool ReadLineStyleArray(const char *, int &, Shape &);
                         bool ReadMatrix(const char *, int &, Matrix &);
                         bool ReadGradient(const char *, int &, Gradient &);
                         bool ReadGradientRecord(const char *, int &, GradientRecord &);
                         bool ReadShape(const char *, int &, const int &, Shape &);
                         bool ReadTextRecord(const char *, int &, const Text &, TextRecord &);

                         bool ReadTagDefineShape(const char *, int &, const int &, Shape &);
                         bool ReadTagDefineFont(const char *, int &, const int &, Font &);
                         bool ReadTagDefineText(const char *, int &, const int &, Text &);
//                       bool ReadTagPlaceObject(const char *, int &, const int &, Text &);

                         int ConvertIntoSignedBits(const int &, const int &);
             int ReadRECTbuff(const char *, float *, int &, int=0);
             void ReadRECTfile(float *, int &);
                         int ReadBits(const char *, int &, const int &);
             int GetBitsOld(const char *, int, int);
             int GetBits(const char *, int, int);
                         int GetBitsNew(const char *, int &, const int &);
             bool ReadBytes(const int, const bool = true);
             
                         inline void FreshByteBitPos(int &);     // Goes to the beginning of a new byte (or if already at the head of an untouched byte doesn't do anything)
};
//----------------------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------------------

