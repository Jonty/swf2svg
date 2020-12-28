//----------------------------------------------------------------------------------------
// Copyright: See definitions.h for License
//----------------------------------------------------------------------------------------
#include "flashformat.h"
//----------------------------------------------------------------------------------------
// Goes to the beginning of a new byte (or if already at the head of an untouched byte doesn't do anything)
//----------------------------------------------------------------------------------------
inline void FlashFormat::FreshByteBitPos(int &bitPos)
{
        if ((bitPos%8)!=0) bitPos = (bitPos/8)*8+8;
}
//----------------------------------------------------------------------------------------
// Reads a number of bits less than a byte long
//----------------------------------------------------------------------------------------
#define readBitsVerbose false
//----------------------------------------------------------------------------------------
int FlashFormat::ReadBits(const char *buff, int &externalBitPos, const int &len)
{
        if (readBitsVerbose) cout << " >> Readbits:";
        if (readBitsVerbose) cout << "         bitPos: " << externalBitPos << " len:" << len << endl;
        if (readBitsVerbose) cout << "         buff[0]: 0x" << ByteToHex(buff[externalBitPos/8]) << endl;
        if (readBitsVerbose) cout << "         buff[1]: 0x" << ByteToHex(buff[(externalBitPos/8)+1]) << endl;
        if (readBitsVerbose) cout << "         buff[2]: 0x" << ByteToHex(buff[(externalBitPos/8)+2]) << endl;
        
        // Initialization
    int res = 0;

    int bytePos = externalBitPos / 8;
        int lastByte = (externalBitPos+len-1)/8;
        int bitPos = (7 - ((externalBitPos+len-1) % 8));

        if (readBitsVerbose) cout << "         bitPos: " << bitPos << endl;
        if (readBitsVerbose) cout << "         bytePos: " << bytePos << endl;
        if (readBitsVerbose) cout << "         lastByte: " << lastByte << endl;

    // First part
    int frame = ((unsigned char) buff[lastByte]) >> bitPos;
    int shiftLen=8-bitPos;
    
        if (readBitsVerbose) cout << "         frame-first: " << frame << endl;
        if (bytePos==lastByte)
        {
                int margin = (externalBitPos%8)+bitPos;
                frame = frame << margin;
                if (readBitsVerbose) cout << "         frame-first0: " << frame << endl;
                frame = frame & 0xff;
                frame = frame >> margin;
                if (readBitsVerbose) cout << "         frame-first0: " << frame << endl;
                shiftLen = len;
        }

        // Middle bytes
        for (int i=lastByte-1;i>bytePos;i--)
        {
        frame += (((unsigned char) buff[i]) << shiftLen);
        shiftLen+=8;
        }
        if (readBitsVerbose) cout << "         frame-mid: " << frame << endl;
        
        if (readBitsVerbose) cout << "         shiftLen: " << shiftLen << endl;

        // Last part
        if (shiftLen<len)
        {
                int margin = 8-(len-shiftLen);
                if (readBitsVerbose) cout << "         margin: " << margin << endl;
                int temp = ((unsigned char) buff[bytePos]) << margin;
                if (readBitsVerbose) cout << "         temp-before0: " << temp << endl;
                temp = temp & 0xFF;
                if (readBitsVerbose) cout << "         temp-before1: " << temp << endl;
                temp = (temp >> margin);
                if (readBitsVerbose) cout << "         temp-after: " << temp << endl;
                frame += ( temp << shiftLen);
        }
        
        if (readBitsVerbose) cout << "         frame: " << frame << endl;

        res = frame;       
//      int oldValue = GetBitsOld(buff,externalBitPos,externalBitPos+len-1);
        
//      if (oldValue!=res) cout << "Fatal Error: Wrong fetching variant bits. Must get: " << oldValue << " But get: " << res << endl;
//      logFile << " < GetBits result: " << res << " buff[0]: 0x" << ByteToHex(buff[bitPos/8])<<" bitPos: "<<bitPos<<" len" << len <<" > " <<endl;
        
        externalBitPos += len;
        
        return res;
}
//----------------------------------------------------------------------------------------
bool FlashFormat::ReadUI8(const char *buff, int &bitPos, int &result)
{
        FreshByteBitPos(bitPos);
    result = ReadBits(buff,bitPos,8);
    
//    cout << "RESULT: " << result << endl;
    
        return true;
}
//----------------------------------------------------------------------------------------
bool FlashFormat::ReadUI16(const char *buff, int &bitPos, int &result)
{
        FreshByteBitPos(bitPos);
//      cout << endl << "ReadUI16::bitPos = " << bitPos << " buff[0]: 0x" << ByteToHex(buff[bitPos/8]);
//      cout << " - 0x"<< ByteToHex(buff[(bitPos/8)+1]) << endl;
    int low = ReadBits(buff,bitPos,8);
//    cout << "low: " << low << endl;
    int high = ReadBits(buff,bitPos,8);
//    cout << "high: " << high << endl;
    
    result = low + (high * 256);
        return true;
}
//----------------------------------------------------------------------------------------
bool FlashFormat::ReadSI16(const char *buff, int &bitPos, int &result)
{
        FreshByteBitPos(bitPos);
    int low = ReadBits(buff,bitPos,8);
    int high = ReadBits(buff,bitPos,8);
    
    result = low + (high * 256);
    result = ConvertIntoSignedBits(result,16);
    
        return true;
}
//----------------------------------------------------------------------------------------
bool FlashFormat::ReadSB(const char *buff, int &bitPos, const int &bitLen, int &result)
{
        bool readSBVerbose = false;
        
        int decimal = ReadBits(buff, bitPos, bitLen);
        if (readSBVerbose) cout << " < SB Decimal: " << decimal << " > ";
        result = ConvertIntoSignedBits(decimal,bitLen);
        
        return true;
}
//----------------------------------------------------------------------------------------
bool FlashFormat::ReadFB(const char *buff, int &bitPos, const int &bitLen, float &result)
{
        bool readFBVerbose = true;

        int decimal = 0;
        if (readFBVerbose) logFile << " < FB buff[0]: 0x" << ByteToHex(buff[bitPos/8]) << " 0x" << ByteToHex(buff[(bitPos/8)+1]) << " > ";
        ReadSB(buff, bitPos, bitLen, decimal);
        if (readFBVerbose) logFile << " < FB Decimal: " << decimal << " > ";
        result = decimal / ((float) 0xFFFF);
        
        return true;
        
}
//----------------------------------------------------------------------------------------
bool FlashFormat::ReadRECT(const char *buff, int &bitPos, RECT &rect)
{
        /*
        cout << " >> ReadRECT:";
        cout << "         bitPos: " << bitPos << endl;
        cout << "         buff[0]: 0x" << ByteToHex(buff[bitPos/8]) << endl;
        cout << "         buff[1]: 0x" << ByteToHex(buff[(bitPos/8)+1]) << endl;
        cout << "         buff[2]: 0x" << ByteToHex(buff[(bitPos/8)+2]) << endl;
        */
        
        int nBits = ReadBits(buff,bitPos,5);
//      cout << "nBits: " << nBits << endl;     

        int twips = 0;
        
        ReadSB(buff,bitPos,nBits,twips);
        rect.from.x = TwipsToFloat(twips);
        
        ReadSB(buff,bitPos,nBits,twips);
        rect.size.x = TwipsToFloat(twips);
        
        ReadSB(buff,bitPos,nBits,twips);
        rect.from.y = TwipsToFloat(twips);
        
        ReadSB(buff,bitPos,nBits,twips);
        rect.size.y = TwipsToFloat(twips);
        
        //FreshByteBitPos(bitPos);
        
        return true;
}
//----------------------------------------------------------------------------------------
bool FlashFormat::ReadRGB(const char *buff, int &bitPos, RGB &rgb, const int &tagType)
{
     int red = GetBits(buff,bitPos,bitPos+7); bitPos+=8;     // Red
     int green = GetBits(buff,bitPos,bitPos+7); bitPos+=8;   // Green
     int blue = GetBits(buff,bitPos,bitPos+7); bitPos+=8;   // Blue
     int alpha = GL_NOALPHA;                               // Default = Alpha Disabled
     
     if (tagType == MFL_TAG_DEFINESHAPE3)
     {
        alpha = GetBits(buff,bitPos,bitPos+7); bitPos+=8;   // Alpha
        rgb.alphaEnabled = true;
     }  

     rgb.SetRGB(red,green,blue,alpha);
     
     return true;
}
//----------------------------------------------------------------------------------------
// FillStyles: FILLSTYLEARRAY                         
//----------------------------------------------------------------------------------------
bool FlashFormat::ReadFillStyleArray(const char *buff, int &bitPos, Shape &shape)
{
     int fillStyleCount = -1;
     
     ReadUI8(buff,bitPos,fillStyleCount);
     
     if (fillStyleCount==0xFF)
        fillStyleCount += ReadBits(buff,bitPos,8) << 8;
     
     shape.fillStyleCount = fillStyleCount;
                              
     if (requestAnalyzer->GetVerboseMode()) cout << "        >> * Fill Styles Count: " << fillStyleCount << endl;
     if (loggingEnabled)
        logFile << "        >> * Fill Styles Count: " << fillStyleCount << endl;

         //shape.ClearFillStyle();
     
     for (int i=0;i<fillStyleCount;i++)
     {
         int fillStyleType = GetBits(buff,bitPos,bitPos+7); bitPos+=8;
         FillStyle tempFillStyle;
         
         tempFillStyle.type = fillStyleType;
         
         if (requestAnalyzer->GetVerboseMode()) cout << "        >>    * Fill Style #" << i+1 << " Type: ";
         if (loggingEnabled)
            logFile << "        >> * Fill Style #" << i+1 << " Type: " << ByteToHex(fillStyleType) << endl;

         switch (fillStyleType)
         {
                case MFL_FILL_SOLID:
                {
                         if (requestAnalyzer->GetVerboseMode()) cout << "<solid>";
                         RGB rgb;
                         ReadRGB(buff,bitPos,rgb,parser_tagType);
                     
                     tempFillStyle.colour = rgb;
                     tempFillStyle.valid = true;
                }
                break;
                case MFL_FILL_LINEAR:
                case MFL_FILL_RADIAL:
                {
                        if (requestAnalyzer->GetVerboseMode()) cout << "<gradient>";
                                // Gradient fills
                        Gradient gradient;
                        Matrix matrix;

                        ReadMatrix(buff,bitPos,matrix);
                        
                        ReadGradient(buff,bitPos,gradient);

                        gradient.matrix = matrix;
                        
                    tempFillStyle.valid = true;
                    tempFillStyle.gradient = gradient;

                }
                break;
                case MFL_FILL_REPEAT_BMP:
                case MFL_FILL_CLIPPED_BMP:
                case MFL_FILL_NS_REPEAT_BMP:
                case MFL_FILL_NS_CLIPPED_BMP:
                {
                         if (requestAnalyzer->GetVerboseMode()) cout << "<bitmap>";
                     int bitmapCharacterID = 0;
                     ReadUI16(buff,bitPos,bitmapCharacterID);
                     logFile << "        >>   Bitmap Character ID: " << bitmapCharacterID << endl;

                     logFile << "        >> * Pos = " << (float) bitPos/8 << " bits" << endl;
                     
                     Matrix matrix;
                     ReadMatrix(buff,bitPos,matrix);
                     
                     tempFillStyle.valid = true;
                     tempFillStyle.bitmapCharacterID = bitmapCharacterID;
                     
                }
                break;
                default:
                        if (requestAnalyzer->GetVerboseMode()) cout << "0x" << ByteToHex(fillStyleType);
                break;
         }
        
         shape.AddFillStyle(tempFillStyle, i);
         
         // Outputting fill style details we just read (if verbose mode is enabled)
         if (DSH_VERBOSE) cout << shape.fillStyle[i];
        
         if (requestAnalyzer->GetVerboseMode()) cout << endl;
     }
     
     return true;
}
//----------------------------------------------------------------------------------------
// LineStyles: LINESTYLEARRAY
//----------------------------------------------------------------------------------------
bool FlashFormat::ReadLineStyleArray(const char *buff, int &bitPos, Shape &shape)
{
         FreshByteBitPos(bitPos);
         int lineStyleCount = GetBits(buff,bitPos,bitPos+7); bitPos+=8;
         
         if (lineStyleCount==0xFF)
            lineStyleCount += ReadBits(buff,bitPos,8) << 8;
         
         shape.lineStyleCount = lineStyleCount;
                                  
         if (requestAnalyzer->GetVerboseMode()) cout << "        >> * Line Styles Count: " << lineStyleCount << endl;
         if (loggingEnabled)
            logFile << "        >> * Line Styles Count: " << lineStyleCount << endl;
        
         //shape.ClearLineStyle();
         
         for (int i=0;i<lineStyleCount;i++)
         {
                 int lowByte = GetBits(buff,bitPos,bitPos+7); bitPos+=8;
                 int highByte = GetBits(buff,bitPos,bitPos+7); bitPos+=8;
                 
             int lineWidth = lowByte+(highByte)*256;
             
             LineStyle tempLineStyle;
             
                 tempLineStyle.lineWidth = lineWidth;
                
                 RGB rgb;
                 ReadRGB(buff,bitPos,rgb,parser_tagType);
             
             tempLineStyle.colour = rgb;
                 tempLineStyle.valid = true;
                 
                 shape.AddLineStyle(tempLineStyle, i);
        }
        
        return true;
}
//----------------------------------------------------------------------------------------
bool FlashFormat::ReadMatrix(const char *buff, int &bitPos, Matrix &matrix)
{
        bool matrixVerbose = false;
        
        if (loggingEnabled)
        {
           logFile << "        >> * Matrix: " << endl;
           logFile << "        >>         BitPos          = " << bitPos << endl;
        }
        
        matrix.hasScale = (GetBits(buff,bitPos,bitPos) == 1); bitPos++;
        
        if (matrix.hasScale)
        {
           if (loggingEnabled)
              logFile << "        >>         HasScale            = " << matrix.hasScale << endl;
           
           int matrixScaleNbits = GetBits(buff,bitPos,bitPos+4); bitPos+=5; // ERROR bitPos+=5?
           if (loggingEnabled)
              logFile << "        >>         ScaleNbits          = " << matrixScaleNbits << endl;
        
           ReadFB(buff,bitPos,matrixScaleNbits,matrix.scaleX);
           ReadFB(buff,bitPos,matrixScaleNbits,matrix.scaleY);
                
           if (loggingEnabled)
           {
               logFile << "        >>         ScaleX          = " << matrix.scaleX << endl;
               logFile << "        >>         ScaleY          = " << matrix.scaleY << endl;
           }
        } else
        {
                matrix.scaleX = 1;
                matrix.scaleY = 1;
        }
        
        matrix.hasRotate = (GetBits(buff,bitPos,bitPos) == 1); bitPos++;
        if (matrix.hasRotate)
        {
           if (loggingEnabled)
              logFile << "        >> *         matrixHasRotate     = " << matrix.hasRotate << endl;
        
           int rotateNbits = GetBits(buff,bitPos,bitPos+4); bitPos+=5; // ERROR bitPos+=5?
        
           ReadFB(buff,bitPos,rotateNbits,matrix.rotateSkew0);
           ReadFB(buff,bitPos,rotateNbits,matrix.rotateSkew1);
           if (loggingEnabled)
           {
               logFile << "        >>         RotateSkew0          = " << matrix.rotateSkew0 << endl;
               logFile << "        >>         RotateSkew1          = " << matrix.rotateSkew1 << endl;
           }
        }
        
        int nTranslateBits = GetBits(buff,bitPos,bitPos+4); bitPos+=5;
        
        if (matrixVerbose) cout << "nTranslateBits: " << nTranslateBits << endl;
        
        if (nTranslateBits>0)
        {
                int crudeTranslateX = GetBits(buff,bitPos,bitPos+nTranslateBits-1);
                crudeTranslateX = ConvertIntoSignedBits(crudeTranslateX,nTranslateBits);
                matrix.translateX = TwipsToFloat(crudeTranslateX);bitPos+=nTranslateBits;
                
                int crudeTranslateY = GetBits(buff,bitPos,bitPos+nTranslateBits-1);
                crudeTranslateY = ConvertIntoSignedBits(crudeTranslateY,nTranslateBits);
                matrix.translateY = TwipsToFloat(crudeTranslateY);bitPos+=nTranslateBits;
                if (loggingEnabled)
                {
                   logFile << "        >>         translateX  = " << matrix.translateX << endl;
                   logFile << "        >>         translateY  = " << matrix.translateY << endl;
                }
        } else
        if (nTranslateBits<0)
        {
                cout << "Fatal error: nTranslateBits" << endl;
                return false;
        }
        
        return true;
}       
//----------------------------------------------------------------------------------------
bool FlashFormat::ReadGradient(const char *buff, int &bitPos, Gradient &gradient)
{
FreshByteBitPos(bitPos);

// Reading the number of GradientRecords : UI8  
int count = GetBits(buff,bitPos,bitPos+7); bitPos+=8;
gradient.count = count;
if (DSH_VERBOSE) cout << endl << endl << "\t\t\t\t >> Gradient count: " << count << endl;

//cout << "<defs>" << endl;
//cout << "<radialGradient id=\"fill00\" gradientUnits=\"objectBoundingBox\">" << endl;

for (int i=0;i<gradient.count;i++)
{
        GradientRecord gradientRecord;
        ReadGradientRecord(buff,bitPos,gradientRecord);
        gradient.gradientRecordVector.push_back(gradientRecord);
        //cout << "GradientRecord #" << i << ": Colour=" << gradientRecord.colour <<" Ratio: "<<gradientRecord.ratio<<endl;
//    cout << "\t<stop offset=\"" << ((gradientRecord.ratio*100)/255) <<"%\" stop-color=\""<<gradientRecord.colour<<"\"/>" <<endl;
}
//cout << "</radialGradient>" << endl;
//cout << "</defs>" << endl;

//cout << "<rect x=\"40\" y=\"40\" width=\"130\" height=\"130\" fill=\"url(#fill00)\"/>" << endl;

return true;
}
//----------------------------------------------------------------------------------------
bool FlashFormat::ReadGradientRecord(const char *buff, int &bitPos, GradientRecord &gradientRecord)
{
//logFile << "GradientRecord@ Bitpos: " << bitPos << endl;
gradientRecord.ratio = GetBits(buff,bitPos,bitPos+7); bitPos+=8;
ReadRGB(buff,bitPos,gradientRecord.colour,parser_tagType);
}
//----------------------------------------------------------------------------------------
bool FlashFormat::ReadTagDefineFont(const char *buff, int &bitPos, const int &tagType, Font &font)
{
        ReadUI16(buff,bitPos,font.characterID);
        if (requestAnalyzer->GetVerboseMode()) cout << "              * Font ID: " << font.characterID << endl;
        
        // Saving the bitPos for the beggining of the "OffsetTable"
        int referenceBitPos = bitPos;
        
        int offset = -1;
        ReadUI16(buff,bitPos,offset);
        
        font.offsetTable.push_back(offset);
        
        // Calculating the number of glyphs by diving the first offset by 2
        font.nGlyphs = offset >> 1;
        
        if (DSH_VERBOSE) cout << "              * nGlyphs: " << font.nGlyphs << endl;
        
        // Remaining offsets (one already read)
        int remainingOffsets = font.nGlyphs - 1;
        
        // Reading OffsetTable
        for (int i=0;i<remainingOffsets;i++)
        {
                ReadUI16(buff,bitPos,offset);
                if (DSH_VERBOSE) cout << "              * offset["<<(i+1)<<"] = " << offset << endl;
                font.offsetTable.push_back(offset);
        }

        // Reading Glyphs
        for (int i=0;i<font.nGlyphs;i++)
        {
                bitPos = referenceBitPos + font.offsetTable[i]*8;
                if (DSH_VERBOSE) cout << "Glyph[" << i << "]:" << endl;
                logFile << "::>> Glyph[" << i << "]:" << endl;
                
                FillStyle fillStyle;
                
                fillStyle.SetRGB(RGB(0,0,0));
                fillStyle.type = MFL_FILL_SOLID;
                
                Shape shape;
                shape.fillStyleCount = 1;
                
                shape.fillStyle[0] = fillStyle;

                ReadShape(buff,bitPos,MFL_TAG_DEFINESHAPE,shape);
                
                font.glyph.push_back(shape);
                
                if (DSH_VERBOSE) cout << " path count=" << shape.pathCount << endl;
                if (shape.pathCount>0)
                        if (DSH_VERBOSE) cout << "      " << shape.path[0] << endl;
        }
        
        return true;
}
//----------------------------------------------------------------------------------------
bool FlashFormat::ReadTagDefineText(const char *buff, int &bitPos, const int &tagType, Text &text)
{
        ReadUI16(buff,bitPos,text.characterID);
        if (DSH_VERBOSE) cout << "              * CharacterID: " <<text.characterID<< endl;

        ReadRECT(buff, bitPos, text.bounds);
        if (DSH_VERBOSE) cout << "              * RECT: (" <<text.bounds.from<< ") size (" << text.bounds.size << ") "<<endl;

        FreshByteBitPos(bitPos);
        ReadMatrix(buff, bitPos, text.matrix);
        if (DSH_VERBOSE) cout << "              * Matrix: " <<text.matrix.ToString()<<endl;

        
        ReadUI8(buff,bitPos,text.glyphBits);
        if (DSH_VERBOSE) cout << "              * glyphBits: " <<text.glyphBits<<endl;
        ReadUI8(buff,bitPos,text.advanceBits);
        if (DSH_VERBOSE) cout << "              * advanceBits: " <<text.advanceBits<<endl;
        
        bool validTextRecord = true;
        
        while (validTextRecord)
        {
                TextRecord textRecord;
                validTextRecord = ReadTextRecord(buff,bitPos,text,textRecord);

                if (validTextRecord)
                {
                        text.textRecordList.push_back(textRecord);
                }
                
        }
        
        if (!validTextRecord)
        {
                bitPos--;

                int endOfRecordsFlag = -1;
                ReadUI8(buff,bitPos,endOfRecordsFlag);
                
                if (endOfRecordsFlag==0)
                {
                        if (requestAnalyzer->GetVerboseMode()) cout << "        >> (Healthy endOfRecordsFlag)" << endl;
                        TriggerOutputEvent(EV_DEFINETEXT,"",NULL,NULL,&text);
                }
                else
                        cout << "        >> Error: Unhealthy endOfRecordsFlag! (0x" << ByteToHex(endOfRecordsFlag) << ")"<< endl;
        }
        
        return true;
}
//----------------------------------------------------------------------------------------
bool FlashFormat::ReadTextRecord(const char *buff, int &bitPos, const Text &text, TextRecord &textRecord)
{
        if (DSH_VERBOSE) cout << "<textrecord>" << endl;
        
        textRecord.type = ReadBits(buff,bitPos,1);
        if (DSH_VERBOSE) cout << " type: "<< textRecord.type << endl;
        
        if (textRecord.type==0) return false;
        
        textRecord.reservedFlags = ReadBits(buff,bitPos,3);
        
        textRecord.hasFont = (ReadBits(buff,bitPos,1)==1);
        textRecord.hasColor = (ReadBits(buff,bitPos,1)==1);
        textRecord.hasYOffset = (ReadBits(buff,bitPos,1)==1);
        textRecord.hasXOffset = (ReadBits(buff,bitPos,1)==1);
        
        textRecord.fontID = -1;
        if (textRecord.hasFont)
        {
                ReadUI16(buff,bitPos,textRecord.fontID);
                if (DSH_VERBOSE) cout << " fontID: " << textRecord.fontID << endl;
        }

        if (textRecord.hasColor)
        {
                ReadRGB(buff,bitPos,textRecord.colour);
                if (DSH_VERBOSE) cout << " colour: " << textRecord.colour << endl;
        }
        
        if (textRecord.hasXOffset)
        {
                int xOffsetTwips;
                ReadSI16(buff,bitPos,xOffsetTwips);
                textRecord.xOffset = TwipsToFloat(xOffsetTwips);
                if (DSH_VERBOSE) cout << " xOffset: " << textRecord.xOffset << endl;
        }
        
        if (textRecord.hasYOffset)
        {
                int yOffsetTwips;
                ReadSI16(buff,bitPos,yOffsetTwips);
                textRecord.yOffset = TwipsToFloat(yOffsetTwips);
                if (DSH_VERBOSE) cout << " yOffset: " << textRecord.yOffset<< endl;
        }

        if (textRecord.hasFont)
        {
                int textHeightTwips=-1;
                ReadUI16(buff,bitPos,textHeightTwips);
                textRecord.textHeight = TwipsToFloat(textHeightTwips);
                if (DSH_VERBOSE) cout << " textHeight: " << textRecord.textHeight<< endl;
        }
        
        ReadUI8(buff,bitPos,textRecord.glyphCount);
        if (DSH_VERBOSE) cout << " glyphCount: " << textRecord.glyphCount<< endl;
        
        for (int i=0;i<textRecord.glyphCount;i++)
        {
                GlyphEntry glyphEntry;

                glyphEntry.glyphIndex = ReadBits(buff,bitPos,text.glyphBits);

                int glyphAdvanceTwips;
                ReadSB(buff,bitPos,text.advanceBits, glyphAdvanceTwips);
                glyphEntry.glyphAdvance = TwipsToFloat(glyphAdvanceTwips);

                if (DSH_VERBOSE) cout << "      glyph["<<i<<"]: "<< endl;
                if (DSH_VERBOSE) cout << "              glyphIndex: " << glyphEntry.glyphIndex<< endl;
                if (DSH_VERBOSE) cout << "              glyphAdvance: " << glyphEntry.glyphAdvance<< endl;
                
                textRecord.glyphEntryList.push_back(glyphEntry);
        }
        
        if (DSH_VERBOSE) cout << "</textrecord>" << endl;
        
        return true;
}
//----------------------------------------------------------------------------------------
/*              
                cout << endl << "Glyph::bitPos = " << bitPos << "(" << (bitPos % 8) << ")";
                cout << " buff: 0x" << ByteToHex(buff[bitPos/8]);
                cout << " - 0x"<< ByteToHex(buff[(bitPos/8)+1]);
                cout << " - 0x"<< ByteToHex(buff[(bitPos/8)+2]);
                cout << endl;
*/
/*

                cout << endl << "ReadTagDefineText::bitPos = " << bitPos << "(" << (bitPos % 8) << ")";
                cout << " buff: 0x" << ByteToHex(buff[bitPos/8]);
                cout << " - 0x"<< ByteToHex(buff[(bitPos/8)+1]);
                cout << " - 0x"<< ByteToHex(buff[(bitPos/8)+2]);
                cout << " - 0x"<< ByteToHex(buff[(bitPos/8)+3]);
                cout << " - 0x"<< ByteToHex(buff[(bitPos/8)+4]);
                cout << endl << endl;

*/
