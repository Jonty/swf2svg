//----------------------------------------------------------------------------------------
// Copyright: See definitions.h for License
//----------------------------------------------------------------------------------------
#include "flashformat.h"
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
FlashFormat::FlashFormat()
{
//cout << "Hi I'm Flash" << endl;
pos = 0;
maxBuffLen = 0;
shapeCount = 0;
uncompressedBuffPos = 0;
uncompressedBuffLength = 0;
isCompressed = false;
endTagReached = false;
}
//----------------------------------------------------------------------------------------
FlashFormat::~FlashFormat()                          // Destructor
{
        // Freeing the space occupied the buffer "xbuff"
        delete [] buff;
}
//----------------------------------------------------------------------------------------
bool FlashFormat::IncommingEventHandler(const int & msg, const string & strParam, int * intParam, float * floatParam, void * untypedPtr)
{
     
}
//----------------------------------------------------------------------------------------
void FlashFormat::Parse()
{
     if (requestAnalyzer->GetVerboseMode()) cout << endl << "Flash Parser initialized ..." << endl << "Input file: '" << GetIOFileName() << "' ..." << endl;

     string fileName = GetIOFileName();
     
     inFile.open(fileName.c_str(), ios::in | ios::binary);

     if (! inFile)
     {
         cout << "Couldn't open the input file" << endl;
         return;
     }

     if (loggingEnabled)
     {
        string logFileName = ("log.txt");
        logFile.open(logFileName.c_str(), ios::out);
        cout << "Logging at " << logFileName << endl;
     }
     
//     char buff[GL_MAXMFLTAGSIZE]; // input buffer
     
     pos=0;

     if (! ReadBytes(8)) return;
     bool compressedFlashDetected = false;
     if (buff[0]=='C')     
     {
         cout << "Compressed flash file detected" << endl;
         compressedFlashDetected = true;
     }

     if ((buff[0]!='F' && buff[0]!='C') ||  buff[1]!='W' ||  buff[2]!='S')     
     {
         cout << "Invalid flash file" << endl;
         return;
     }

     flashVersion = (unsigned char) buff[3];

     if (requestAnalyzer->GetVerboseMode() || true) 
     cout << "Flash signature verified for SWF version: " << flashVersion  << ".0" << endl << endl;
     if (loggingEnabled)
        logFile << "Flash signature verified for SWF version: " << flashVersion << ".0" << endl;
     
     int intParam[10];
     float floatParam[10];
     
     TriggerOutputEvent(EV_VALID,"",intParam,NULL,NULL);
     
     int headerFileLength = 0;

     headerFileLength  = ((unsigned char) buff[4]);
     headerFileLength += ((unsigned char) buff[5])*256;
     headerFileLength += ((unsigned char) buff[6])*256*256;
     headerFileLength += ((unsigned char) buff[7])*256*256*256;

     if (requestAnalyzer->GetVerboseMode()) cout << "Stream length supposed to be: " << headerFileLength << " bytes" <<endl;
     if (loggingEnabled)
        logFile << "Stream length supposed to be: " << headerFileLength << " bytes" <<endl;

         // In case flash file is not compressed but user has tried the "decompress only" option        
     if ((!compressedFlashDetected) && requestAnalyzer->GetRequestedAction()==RQ_MODE_DECOMPRESS)
     {
        cout << "Input file is not a compressed flash file" << endl;
        
        return;
     }
     
     if (compressedFlashDetected)
     {
         int i = 0;
         Byte *compr, *uncompr;
         uLong uncompressedLength = headerFileLength-pos;
         compr = new Byte[uncompressedLength];
         uncompr = new Byte[uncompressedLength];
         int posBeforeDecompression = pos;
         
         char headerBuff[10];
         int headerPos;

         for (headerPos=0;headerPos<7;headerPos++)
                 headerBuff[headerPos] = buff[headerPos];

         cout << "Decompressing flash file ..." << endl << endl;
                  
         if (! ReadBytes(2)) return;

//         headerBuff[headerPos] = buff[0]; headerPos++;
//         headerBuff[headerPos] = buff[1]; headerPos++;

         compr[i++] = buff[0];
         compr[i++] = buff[1];

         int CMF = (unsigned char) buff[0];
         int FLAGS = (unsigned char) buff[1];

                 int tempBitPos = 0;
         int CINFO = ReadBits(buff,tempBitPos,4);
         int CM = ReadBits(buff,tempBitPos,4);

         if (loggingEnabled && GL_DEBUG_MODE)
         {
             logFile << endl << "         * CM = " << CM << " " << endl;
             logFile << endl << "         * CINFO = " << CINFO << " " << endl;
         }
         
         if (CM!=0x08)
         {
            cout << "         >> Error: Compression algorithm not supported (Mode: 0x";
            cout << ByteToHex(CM) << ")" << endl;
            return;
         }
         
         cout << "         >> Compression algorithm: DEFLATE (rfc1950, rfc1951)" << endl;
         cout << "         >> Using library code wirtten by Jean-loup and Gailly Mark Adler" << endl;
         cout << "            to decompress ";
         cout << "(zlib ver: " << zlibVersion() << ")" << endl;
         
         int checkSum = (CMF * 256 + FLAGS) % 31;
/*         
         cout << endl << "         * Compressed header checksum: ";
         if (checkSum == 0) cout << "OK"; else
            cout << "Corrupt!";
         cout << endl;
*/
         while ((! inFile.eof()) && ReadBytes(1, false))
         {
               compr[i++] = buff[0];
         }
         
         uLong compressedLength = i;
                 
                 /*
                 cout << endl;
                 for (int jj=0;jj<compressedLength;jj++)
               cout << ByteToHex(compr[jj]) << " ";
         cout << endl;
         //*/     
         
         cout << "         * Uncompressed Length:    " << uncompressedLength << " bytes" << endl;
         cout << "         * Compressed Length:      " << compressedLength << " bytes" << endl;
         
         
         int err = uncompress(uncompr, &uncompressedLength, compr, compressedLength);

         if (err != Z_OK)
         {
            cout << "          >> Decompression error: #" << err << endl;
            return;
         }
 
         cout << endl << "         >> Decompressed successfully" << endl;
         
         uncompressedBuff = new char[uncompressedLength]; 
         uncompressedBuffPos = 0;
         uncompressedBuffLength = uncompressedLength;
         
         for (i=0;i<uncompressedLength;i++)
             uncompressedBuff[i] = uncompr[i];

         isCompressed = true;
         pos = posBeforeDecompression;

         if (requestAnalyzer->GetVerboseMode()) cout << endl;
         
         // Now writing the decompressed flash into a file
         if (requestAnalyzer->GetRequestedAction() == RQ_MODE_DECOMPRESS)
         {
                cout << "Only decompress mode: Decompressing flash file into \"decompressed.swf\"..." << endl;
                ofstream decompFlash;
                decompFlash.open("decompressed.swf", ios::out | ios::binary);
                headerBuff[0] = 'F';

                        cout << "Header: " << endl;
                        
                 for (i=0;i<8;i++)
                 {
                     decompFlash << headerBuff[i];
//                   cout << ByteToHex(headerBuff[i]) << " ";
                 }

                        if (requestAnalyzer->GetVerboseMode()) cout << endl << "Body: " << endl;
                for (i=0;i<uncompressedLength;i++)
                {
                    decompFlash << (char) uncompressedBuff[i];
//              if (i<30) cout << ByteToHex(uncompressedBuff[i]) << " ";
                }
                    
                        // Exits after decompression
                return;
         }
     }

     if (loggingEnabled)
        logFile << "Reading frame's RECT ..." << endl;

     // RECT
     
     int nBits=0;
     int twipsFrame[4];
     float pixelsFrame[4];

     
     ReadRECTfile(pixelsFrame,nBits);

//     for (int i=0;i<4;i++)
//         pixelsFrame[i]=twipsFrame[i]/20;
//     cout << "buff[0] = " << ByteToHex(buff[0]) << " " <<endl;
//     cout << "buff[1] = " << ByteToHex(buff[1]) << " " <<endl;
//     cout << "buff[2] = " << ByteToHex(buff[2]) << " " <<endl;
//     cout << "buff[3] = " << ByteToHex(buff[3]) << " " <<endl;
     
     if (requestAnalyzer->GetVerboseMode()) cout << "nBit = " << nBits << " bits" << endl;
     if (requestAnalyzer->GetVerboseMode()) cout << "frameX = " << pixelsFrame[1] << " pixels  (MinX = " << pixelsFrame[0] << ")" <<endl;
     if (requestAnalyzer->GetVerboseMode()) cout << "frameY = " << pixelsFrame[3] << " pixels  (MinY = " << pixelsFrame[2] << ")" <<endl;

     if (loggingEnabled)
     {
         logFile << "nBit = " << nBits << " bits" << endl;
         logFile << "frameX = " << pixelsFrame[1] << " pixels  (MinX = " << pixelsFrame[0] << ")" <<endl;
         logFile << "frameY = " << pixelsFrame[3] << " pixels  (MinY = " << pixelsFrame[2] << ")" <<endl;
     }

     TriggerOutputEvent(EV_FRAME,"",NULL,pixelsFrame,NULL);

     // Frame Rate + Frame Count
     if (! ReadBytes(4)) return;
     
     int frameRate = (unsigned char) buff[1];
     
     if (requestAnalyzer->GetVerboseMode()) cout << "Frame Rate = " << frameRate<< " fps" << endl;
     
     int frameCount = ((unsigned char) buff[2]) + ((unsigned char) buff[3])*256;
     
     if (requestAnalyzer->GetVerboseMode()) cout << "Frame Count = " << frameCount << " frames" << endl;
     
     if (requestAnalyzer->GetVerboseMode()) cout << "Parsing the header finished" << endl;
     
     int tagNo = 0;
     int debugCount = 0;

     // Current shape on the display list's character ID
         int placedCharacterID = -1;
          
     while (!endTagReached && (!GL_DEBUG_MODE || debugCount++ < 700000))
     {
           if (! ReadBytes(2)) return;
           tagNo++;
           if (requestAnalyzer->GetVerboseMode()) cout << endl << "Tag #" << tagNo << ": " << endl;
           if (loggingEnabled)
           {
              logFile << endl << "Tag #" << tagNo << ": " << endl;
           }
           
           if ((!requestAnalyzer->GetVerboseMode())&&(requestAnalyzer->GetIndicatorEnabled()))
                        cout << ".";

           char tempChar = buff[0];
           buff[0] = buff[1];
           buff[1] = tempChar;
           
           parser_tagType = GetBits(buff,0,9);
           int tagLen  = GetBits(buff,10,15);
           
           if (requestAnalyzer->GetVerboseMode()) cout << "        Tag Type = [" << parser_tagType << "] " << endl;
           if (loggingEnabled)
              logFile << "        Tag Type = [" << parser_tagType << "] " << endl;
           
           // In the case of "long tag"
           if (tagLen==63)
           {
                 if (requestAnalyzer->GetVerboseMode()) cout << "        [Long tag]" << endl;
                 
                 // Extra byte for length
                 if (! ReadBytes(4)) return;
                 tagLen = (unsigned char) buff[0]+((unsigned char) buff[1])*256;
                 tagLen += ((unsigned char) buff[2])*256*256+((unsigned char) buff[3])*256*256*256;
           }
           if (requestAnalyzer->GetVerboseMode()) cout << "        Tag Length = " << tagLen << " bytes " << endl;
           if (loggingEnabled)
              logFile << "        Tag Length = " << tagLen << " bytes " << endl;

           // Reading tag body based on its size
           if (! ReadBytes(tagLen)) return;

           // Quit after some specific tags (Only for debug)
//           if (GL_DEBUG_MODE && tagNo>7) return;
           
           // Parsing only some specific tags (Only for debug)
//           if (GL_DEBUG_MODE && tagNo<40000000)
           switch (parser_tagType)
           {
                  case MFL_TAG_SETBKG:
                  {
                         if (requestAnalyzer->GetVerboseMode()) cout << "        >> SetBackgroundColor" << endl;
                         if (loggingEnabled)
                            logFile << "        >> SetBackgroundColor" << endl;
                  
                         intParam[0] = (unsigned char) buff[0];   // Red
                         intParam[1] = (unsigned char) buff[1];   // Green
                         intParam[2] = (unsigned char) buff[2];   // Blue

                         if (requestAnalyzer->GetVerboseMode()) cout << "        >> * RGB: " << intParam[0] << "," << 
                                        intParam[1] << "," << intParam[2] << endl;
                         if (loggingEnabled)
                            logFile << "        >> * RGB: " << intParam[0] << "," << 
                                        intParam[1] << "," << intParam[2] << endl;

                         TriggerOutputEvent(EV_SETBKG,"",intParam,NULL,NULL);
                  }
                  break;
                  case MFL_TAG_PLACEOBJECT:
                  {
                         if (requestAnalyzer->GetVerboseMode()) cout << "        >> PlaceObject *** Not implemented yet ***" << endl;
                         if (loggingEnabled)
                            logFile << "        >> PlaceObject *** Not implemented yet ***" << endl;
                  }
                  break;
                  case MFL_TAG_PLACEOBJECT2:
                  {
                         if (requestAnalyzer->GetVerboseMode()) cout << "        >> PlaceObject2" << endl;
                         if (loggingEnabled)
                            logFile << "        >> PlaceObject2" << endl;
                         
                         Matrix matrix;
                         int bitPos = 0;

                         bool placeFlagHasClipActions    = (GetBits(buff,bitPos,bitPos) == 1); bitPos++;
                         bool placeFlagHasClipDepth      = (GetBits(buff,bitPos,bitPos) == 1); bitPos++;
                         bool placeFlagHasHasName        = (GetBits(buff,bitPos,bitPos) == 1); bitPos++;
                         bool placeFlagHasRatio          = (GetBits(buff,bitPos,bitPos) == 1); bitPos++;
                         bool placeFlagHasColorTransform = (GetBits(buff,bitPos,bitPos) == 1); bitPos++;
                         bool placeFlagHasMatrix         = (GetBits(buff,bitPos,bitPos) == 1); bitPos++;
                         bool placeFlagHasCharacter      = (GetBits(buff,bitPos,bitPos) == 1); bitPos++;
                         bool placeFlagHasMove           = (GetBits(buff,bitPos,bitPos) == 1); bitPos++;

                         // Usual case is 0x06 therefore if not report it in logfile
                         if (loggingEnabled)
                         {
                             if (((unsigned char) buff[0])!=6)
                             {
                                 logFile << endl;             
                                 logFile << "        >> * placeFlagHasClipActions    = " << placeFlagHasClipActions << endl;
                                 logFile << "        >> * placeFlagHasClipDepth      = " << placeFlagHasClipDepth << endl;
                                 logFile << "        >> * placeFlagHasName           = " << placeFlagHasHasName << endl;
                                 logFile << "        >> * placeFlagHasRatio          = " << placeFlagHasRatio << endl;
                                 logFile << "        >> * placeFlagHasColorTransform = " << placeFlagHasColorTransform << endl;
                                 logFile << "        >> * placeFlagHasMatrix         = " << placeFlagHasMatrix << endl;
                                 logFile << "        >> * placeFlagHasCharacter      = " << placeFlagHasCharacter << endl;
                                 logFile << "        >> * placeFlagHasMove           = " << placeFlagHasMove << endl;
                             } else
                                  logFile << "        >>         (Empty Matrix)" << endl;
                         }
                         
                         // Depth - UI16
                         int depth = ((unsigned char) buff[1]) +  ((unsigned char) buff[2])*256;
                         bitPos += 16;
                         if (requestAnalyzer->GetVerboseMode()) cout << endl;             
                         if (requestAnalyzer->GetVerboseMode()) cout << "        >> * Depth                      = " << placeFlagHasMove << endl;
                         
                         // CharacterID - UI16
                         if (placeFlagHasCharacter) 
                         {
                            placedCharacterID = ((unsigned char) buff[3]) +  ((unsigned char) buff[4])*256;
                            bitPos += 16;
                            if (requestAnalyzer->GetVerboseMode()) cout << "        >> * CharacterID                = " << placedCharacterID << endl;
                         }

                         // Transform Matrix - MATRIX
                         if (placeFlagHasMatrix)
                         {
                                                        ReadMatrix(buff, bitPos, matrix);
                         }
                         
                         floatParam[0] = dict[placedCharacterID].x;
                         floatParam[1] = dict[placedCharacterID].y;
                         floatParam[2] = dict[placedCharacterID].width;
                         floatParam[3] = dict[placedCharacterID].height;
                         
                         intParam[0] = placedCharacterID;
                         intParam[1] = (placeFlagHasMatrix ? MFL_PLACETAGHASMATRIX : 0);
                         
                         TriggerOutputEvent(EV_PLACEOBJECT2,"",intParam,floatParam,&matrix);
                         
                         // Place the only paths shape if we require them to be generated as well
/*                                               if (requestAnalyzer->GetRequestedAction()==RQ_MODE_GENERATEPATHS)
                                                 {
                                                        intParam[0] = placedCharacterID+1;
                                TriggerOutputEvent(EV_PLACEOBJECT2,"",intParam,floatParam,&matrix);
                                                 }*/
                  }
                  break;
                  case MFL_TAG_END:
                  {
                         if (requestAnalyzer->GetVerboseMode()) cout << "        >> End" <<endl;
                         if (loggingEnabled)
                            logFile << "        >> End" <<endl;
                            
                         TriggerOutputEvent(EV_END,"",NULL,NULL,NULL);
                         
                         endTagReached = true;
                  }
                  break;
                  case MFL_TAG_SHOWFRAME:
                  {
                         if (requestAnalyzer->GetVerboseMode()) cout << "        >> ShowFrame " << endl;
                         if (loggingEnabled)
                            logFile << "        >> ShowFrame" <<endl;
                  }
                  break;
                  case MFL_TAG_DEFINEFONTINFO:
                  {
                         if (requestAnalyzer->GetVerboseMode()) cout << "        >> DefineFontInfo " << endl;
                         
                         // Nothing else needed to be done since SVG does not need this information
                  }
                  break;
                  case MFL_TAG_DEFINEFONT:
                  {
                         if (requestAnalyzer->GetVerboseMode()) cout << "        >> DefineFont " << endl;

                                                 Font font;
                                                
                         int bitPos = 0;
                         ReadTagDefineFont(buff,bitPos,parser_tagType,font);
                                                
                                                 fontMap.insert(FontMap::value_type(font.characterID,font));
                                                 
                                                 TriggerOutputEvent(EV_DEFINEFONT,"",NULL,NULL,&font);
                  }
                  break;
                  case MFL_TAG_DEFINETEXT:
                  {
                         if (requestAnalyzer->GetVerboseMode()) cout << "        >> DefineText " << endl;

                                                 Text text;
                                                
                         int bitPos = 0;
                         ReadTagDefineText(buff,bitPos,parser_tagType,text);
                                                
                                                 //fontMap.insert(FontMap::value_type(font.characterID,font));                         
                  }
                  break;
                  case MFL_TAG_DEFINESHAPE:
                  case MFL_TAG_DEFINESHAPE2:
                  case MFL_TAG_DEFINESHAPE3:
                  {
                                                Shape shape;
                                                
                                
                                int bitPos = 0;
                                ReadTagDefineShape(buff,bitPos,parser_tagType, shape);

                                                 int requestMode = requestAnalyzer->GetRequestedAction();
                                                 
                                                 // Sending the shape in the dictionary to output's format object to deal with it
                                                 if (requestMode==RQ_MODE_CONVERT)
                                                 {
                                                TriggerOutputEvent(EV_DEFINESHAPE,IntToStr(parser_shapeID),intParam,floatParam,&dict[parser_shapeID]);
                                                 }
                                        
                                                 if (requestMode==RQ_MODE_GENERATEPATHS)
                                                 {
                                                        dict[parser_shapeID].isOnlyPathsDemonstration = true;
                                                        TriggerOutputEvent(EV_DEFINESHAPE,"",NULL,NULL,&dict[parser_shapeID]);
                                                 }
                  }
                  break;
                  case MFL_TAG_PROTECT:
                  {
                         cout << "        >> Protect" << endl;
                         cout << endl;
                         cout << "    !!! Warning: This file is marked \"protected\" by its author." << endl;
                         cout << "                 The resulted SVG should not be opened in any authoring"  << endl;
                         cout << "                 tool and should not be imported or used in any product"  << endl;
                         cout << "                 or else it will breach copyright laws."  << endl;
                         cout << endl;
                         
                         if (loggingEnabled)
                            logFile << "        >> Protect" <<endl;
                  }
                  break;
                  case MFL_TAG_DEFINEBITS:
                  {
                         if (requestAnalyzer->GetVerboseMode()) cout << "        >> DefineBits" << endl;
                         if (loggingEnabled)
                            logFile << "        >> DefineBits" <<endl;
                  }
                  break;
                  case MFL_TAG_DEFINEBITS2:
                  {
                         if (requestAnalyzer->GetVerboseMode()) cout << "        >> DefineBits2" << endl;
                         if (loggingEnabled)
                            logFile << "        >> DefineBits2" <<endl;
                  }
                  break;
                  case MFL_TAG_DEFINEBITS3:
                  {
                         if (requestAnalyzer->GetVerboseMode()) cout << "        >> DefineBits3" << endl;
                         if (loggingEnabled)
                            logFile << "        >> DefineBits3" <<endl;
                  }
                  break;
                  case MFL_TAG_JPEGTABLES:
                  {
                         if (requestAnalyzer->GetVerboseMode()) cout << "        >> JPEGTables " << endl;
                         cout << endl << "   !!! Warning: integrated raster images are not supported" << endl;
                         if (loggingEnabled)
                            logFile << "        >> JPEGTables" <<endl;
                  }
                  break;
                  case MFL_TAG_DFBITSLOSSLESS:
                  {
                         if (requestAnalyzer->GetVerboseMode()) cout << "        >> DefineBitsLossless " << endl;
                         cout << endl << "   !!! Warning: integrated raster images are not supported" << endl;
                         if (loggingEnabled)
                            logFile << "        >> DefineBitsLossless" <<endl;
                  }
                  break;
                  case MFL_TAG_DFBITSLOSSLESS2:
                  {
                         if (requestAnalyzer->GetVerboseMode()) cout << "        >> DefineBitsLossless2 " << endl;
                         cout << endl << "   !!! Warning: integrated raster images are not supported" << endl;
                         if (loggingEnabled)
                            logFile << "        >> DefineBitsLossless2" <<endl;
                  }
                  break;
                  default:
                         cout << endl << "   !!! Warning: conversion for tag type: " << parser_tagType << " is not supported" << endl;
                  break;
                  
           }
           
//           unsigned char cc = buff[0];
//           cout << "#" << len<<" Char: #" << (int) cc << "     '" << cc << "'" << endl;
     }

   if ((!requestAnalyzer->GetVerboseMode())&&(requestAnalyzer->GetIndicatorEnabled()))
                cout << endl;
                
     string additionalMessage = "(Conforms with the header)";
     if (pos!=headerFileLength) 
        additionalMessage = "(Error: File length doesn't conform with the header)";
     
     cout << endl << "Totally read: " << pos << " bytes " << additionalMessage << endl;
     //cout << "maxBuffLen: " << maxBuffLen<< " bytes " << endl;
     cout << "Parsing completed! " << endl;

}
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
int FlashFormat::ReadRECTbuff(const char *buff, float * frame, int &nBits, int startBit)
{
// nBits = (((unsigned char) buff[0]) & 248) >> 3; // First 5 bits
nBits = GetBits(buff,0+startBit,4+startBit);

int pos = 5+startBit;
//cout << " ********* Nbits = " << (nBits) <<  endl;

for (int i=0;i<4;i++)
{
//   int twips = GetBits(buff,pos,pos+nBits-1); 
        int twips = 0;
        ReadSB(buff,pos,nBits,twips);

        //cout << endl << "twips["<<i<<"]: "<<twips<<endl;
   //int lastBit = (twips >> (nBits-1));

//cout << " ********* LAST BIT = " << (lastBit) << "  Twips: " << twips << endl;

   frame[i] = TwipsToFloat(twips);
//   pos += nBits;
//   logFile << "frame["<<i<<"] = " << twips << " twips" <<endl;
}

int bytesRead = (nBits*4+5);
if (bytesRead % 8 == 0) bytesRead /= 8; else bytesRead = (bytesRead / 8) + 1;

return bytesRead;

}
//----------------------------------------------------------------------------------------
void FlashFormat::ReadRECTfile(float * frame, int &nBits)
{
// nBits = (((unsigned char) buff[0]) & 248) >> 3; // First 5 bits
char tempBuff[GL_MAXMFLTAGSIZE];

if (! ReadBytes(1)) return;

nBits = GetBits(buff,0,4);
int newBytes = (nBits*4-3);
if (newBytes % 8 == 0) newBytes /= 8; else newBytes = (newBytes / 8) + 1;

//cout << "newBytes = " << newBytes << " bytes" <<endl;

if (! ReadBytes(newBytes)) return;

//cout << "buff[0] = " << ByteToHex(buff[0]) << " " <<endl;

for (int i=1;i<newBytes+1;i++)
{
    tempBuff[i] = buff[i-1];
//    cout << "buff["<<i<<"] = " << ByteToHex(buff[i]) << " " <<endl;
}
   

int pos = 5;

for (int i=0;i<4;i++)
{
   int twips = GetBits(tempBuff,pos,pos+nBits-1); 
   frame[i] = TwipsToFloat(twips);
   pos += nBits;
//   cout << "frame["<<i<<"] = " << frame[i] << " twips" <<endl;
}
}
//----------------------------------------------------------------------------------------
// Reads a number of bits positioned from start to end (inclusive) among a string of chars
int FlashFormat::GetBitsOld(const char *buff, int start, int end)
{
    if (end<start)
    {
        cout << "Fatal Error: Unexpected call of function GetBits start: " << start << " end: " << end << endl;
        return 0;
    }
    int bstart = start / 8;
    int s      = 7 - (start % 8);
    int bend   = end   / 8;
    int e      = 7 - (end   % 8);
    
// cout << "e = " << e << " ??" <<endl;
// cout << "s = " << s << " ??" <<endl;
// cout << "bs = " << bstart << " ??" <<endl;
// cout << "be = " << bend << " ??" <<endl;

    int res = 0;
    
    if (bstart==bend)
    {
       int tmpl = 0;
       int pos = 1 << e;
       for (int i=e;i<=s;i++)
       {
           tmpl = tmpl | pos;
           pos = pos << 1;
//           cout << "t = " << tmpl << "   pos = " <<pos<<endl;
       }
       
       res = (tmpl & ((unsigned char) buff[bstart])) >> e;
       
       return res;
    } else
    if (bstart+1==bend)
    {
         int res1 = GetBits(buff,start,((bstart+1)*8)-1);
         int res2 = GetBits(buff,bend*8,end);
//         cout << "res1 = " << res1 << " from:" << start << " to:" << ((bstart+1)*8)-1 << endl;
//         cout << "res2 = " << res2 << " from:" << bend*8 << " to:" << end << endl;
         
         res = (res1 << (end + 1 - bend*8)) | res2;
         
         return res;
    } else
    if (bstart+2==bend)
    {
         int res1 = GetBits(buff,start,((bstart+1)*8)-1);
         int res2 = GetBits(buff,bend*8,end);
         int res3 = (unsigned char) buff[bstart+1];
//         cout << "res1 = " << res1 << " from:" << start << " to:" << ((bstart+1)*8)-1 << endl;
//         cout << "res2 = " << res2 << " from:" << bend*8 << " to:" << end << endl;
//         cout << "res3 = " << res3 << " from:" << bend*8 << " to:" << end << endl;
         
         res = (res3 << (end + 1 - bend*8)) | res2;
         res = (res1 << (end + 9 - bend*8)) | res;
         
         return res;
    } else
    {
        cout << "Fatal Error: Unexpected length of bits" << end-start+1 << endl;
        return 0;
    }
}
//----------------------------------------------------------------------------------------
// Reads a number of bits positioned from start to end (inclusive) among a string of chars
int FlashFormat::GetBits(const char *buff, int start, int end)
{
        return ReadBits(buff,start,end-start+1);
}
//----------------------------------------------------------------------------------------
// Reads a number of bits positioned from start to end (inclusive) among a string of chars
int FlashFormat::GetBitsNew(const char *buff, int &bitPos, const int &len)
{
        int result = 0;
        if (len>0) 
        {
                result = GetBits(buff,bitPos,bitPos+len-1);
                bitPos += len;
        }
        return result;
}
//----------------------------------------------------------------------------------------
bool FlashFormat::ReadBytes(const int count, const bool errorCheck)
{
        delete [] buff;
        try {
                buff = new char[count+1];
        }
        catch (...)
        {
                cout << "Fatal Error: Not enough memory for aquiring a SWF tag of size: "<<count<<" bytes"<<endl;
        }
     if (isCompressed)
     {
         if (uncompressedBuffPos+count>uncompressedBuffLength)
         {
//            cout << "Unexpected end of file [Quitting...] Pos: " << pos << " Bytes yet to be read: " << count << endl;
            cout << "Reach end of the uncompressed buffer  Pos: " << uncompressedBuffPos << " Length: " << uncompressedBuffLength << endl;
            return false;
         }
//        cout << endl << "Buffering from the uncompressed stream  count:"<< count << endl;
        for (int i=0;i<count;i++)
        {
            buff[i] =  uncompressedBuff[uncompressedBuffPos++];
//            cout << ByteToHex (buff[i])<< " ";
        }
//        cout << endl;
     }
     else
     {
         if (count>maxBuffLen) maxBuffLen=count; // Identify the biggest tag size (?)
         
         try
         {
                 if (! inFile.read(buff,count))
                 {
                    if (errorCheck) cout << "Unexpected end of file Pos: " << pos << " Bytes yet to be read: " << count << endl;
                    return false;
                 }
         }
         catch (...)
         {
            cout << "Unexpected exception while reading the file Pos: " << pos << " Bytes yet to be read: " << count << endl;
            return false;
         }
     }

     pos += count;
     return true;
}
//----------------------------------------------------------------------------------------
int FlashFormat::ConvertIntoSignedBits(const int &n, const int &nBits)
{
        int nstar = 0;
        
        int lastBit = (n >> (nBits-1));

//      logFile << endl <<  "    >> lastBit : " << lastBit << endl;

        // Number is negative then we have to de-two's compliment it
        if (lastBit==1)
        {
                bool complement = false;

//              logFile << endl <<  "    >> decoding : " << endl;
                
                for (int i=1;i<nBits+1;i++)
                {
                        int currentBit = (n >> (i-1)) & 1;

//                      logFile << i << ": " << currentBit;

                        if (complement) currentBit = ( currentBit == 0 ? 1 : 0 );
                        nstar = nstar | (currentBit << (i-1));
                        
//                      logFile << "  ,  " << currentBit << "   C: " << complement << endl;
                        
                        if (!complement && currentBit==1) complement = true;
                }
                
                nstar = -nstar;
        }
        else
                nstar = n;
        
        return nstar;
}
//----------------------------------------------------------------------------------------
float FlashFormat::TwipsToFloat(const int &twips)
{
        float scaled = GL_FLASHTWIPSPERPX;
        return ((float)twips)/scaled;
}
//----------------------------------------------------------------------------------------
// Needs to be implemented if conversion TO flash is required
//----------------------------------------------------------------------------------------
void FlashFormat::FlushOutput()
{
 cout << "Flash Flusher!" << endl;
}
//----------------------------------------------------------------------------------------
// dumps a bit of binary flash
//                                                       cout << "Bytepos: " << bytePos << endl;
                                                         
//                                                       for (int gg=0;gg<16;gg++)
//                                                              cout << ByteToHex((unsigned char) buff[bytePos+gg]) << " ";
                                                                
//                                                       cout << endl;
/*
                                        int bytePos = bitPos/8;
                                                         logFile << "Bytepos: " << bytePos << endl;
                                                         logFile << "Bitpos: " << bitPos << endl;
                                                         
                                                         for (int gg=-3;gg<16;gg++)
                                                                logFile << ByteToHex((unsigned char) buff[bytePos+gg]) << " ";
                                                                
                                                         logFile << endl;
*/
/*
                                        int bytePos = bitPos/8;
                                                         cout << "Bytepos: " << bytePos << endl;
                                                         cout << "Bitpos: " << bitPos << endl;
                                                         
                                                         for (int gg=-3;gg<16;gg++)
                                                                cout << ByteToHex((unsigned char) buff[bytePos+gg]) << " ";
                                                                
                                                         cout << endl;

*/
