//----------------------------------------------------------------------------------------
// Copyright: See definitions.h for License
//----------------------------------------------------------------------------------------
#include "flashformat.h"
//----------------------------------------------------------------------------------------
bool FlashFormat::ReadTagDefineShape(const char *buff, int &bitPos, const int &tagType, Shape & shape)
{
     int intParam[10];
     float floatParam[10];
     float pixelsFrame[4];

         ReadUI16(buff, bitPos, parser_shapeID);

         if (requestAnalyzer->GetVerboseMode()) cout << "        >> * Shape ID: " << parser_shapeID << endl;
     if (tagType ==  MFL_TAG_DEFINESHAPE)  
     {
        if (requestAnalyzer->GetVerboseMode()) cout << "        >> DefineShape" << endl;
        shape.version = 1;
     }
     if (tagType ==  MFL_TAG_DEFINESHAPE2) 
     {
        if (requestAnalyzer->GetVerboseMode()) cout << "        >> DefineShape2" << endl;
        shape.version = 2;
     }
     if (tagType ==  MFL_TAG_DEFINESHAPE3) 
     {
        if (requestAnalyzer->GetVerboseMode()) cout << "        >> DefineShape3" << endl;
        shape.version = 3;
     }

     shape.characterID = parser_shapeID;
     
     // ShapeBound: RECT
/*
     int nBits;
     int bytePos = 2;
     bytePos = ReadRECTbuff(buff,pixelsFrame,nBits,bytePos*8);
     
     shape.x = pixelsFrame[0];
     shape.y = pixelsFrame[2];
     shape.width = pixelsFrame[1]-pixelsFrame[0];
     shape.height = pixelsFrame[3]-pixelsFrame[2];
*/

         RECT rect;
         ReadRECT(buff,bitPos,rect);
         
         shape.x = rect.from.x;
         shape.y = rect.from.y;
         shape.width = rect.size.x;
         shape.height = rect.size.y;
         
     if (DSH_VERBOSE)
     {
         cout << "        >> * Shape RECT: ";
         cout << "x: " << shape.x << ", ";
         cout << "y: " << shape.y << ", ";
         cout << "w: " << shape.width << ", ";
         cout << "h: " << shape.height << endl;
     }

     //bitPos += bytePos*8;

         // FillStyles: FILLSTYLEARRAY                         
         if (!ReadFillStyleArray(buff,bitPos, shape)) return false;
         
//       for (int i=0;i<shape.fillStyleCount;i++)
//              cout << "Fill["<<i<<"]: "<<shape.fillStyle[i]<<endl;

     // LineStyles: LINESTYLEARRAY
     if (!ReadLineStyleArray(buff,bitPos, shape)) return false;
     
//     bytePos = bitPos / 8;

         ReadShape(buff, bitPos, tagType, shape);

         dict[parser_shapeID] = shape;
     shapeCount++;
         
         return true;
}
//----------------------------------------------------------------------------------------
bool FlashFormat::ReadShape(const char *buff, int &bitPos, const int &tagType, Shape &shape)
{
 parser_fillIndexBits = ReadBits(buff,bitPos,4);
 parser_lineIndexBits = ReadBits(buff,bitPos,4);

 if (DSH_VERBOSE) cout << endl << "    >> Fill Index Bits:" << parser_fillIndexBits;
 if (DSH_VERBOSE) cout << endl << "    >> Line Index Bits:" << parser_lineIndexBits << endl;
 
 // Shape Records
 bool endShapeRecord = false;
 int recordCount = 0;
 static int edgeCount = 0;
 string pathData = "";

 int fillStyle0 = 0, fillStyle1 = 0, lineStyle = 0;
 float currentX, currentY;
 ShapeSpace *shapeSpace = new ShapeSpace();

                          
 while (!endShapeRecord)
 {
     recordCount++;
     int typeFlag = GetBits(buff,bitPos,bitPos); bitPos++;

     logFile << endl << "    >> Shape Record #" << recordCount << " : ";
     logFile << endl << "    >> Type Flag : " << typeFlag << endl;


    // Non-edge records
        if (typeFlag==0)
        {
   
                bool stateNewStyle = false;
                            
                // Will need to always read only effective in DefineShape2 DefineShape3
        stateNewStyle = (ReadBits(buff,bitPos,1)==1);
        
        if (tagType != MFL_TAG_DEFINESHAPE2 && tagType != MFL_TAG_DEFINESHAPE3)
                stateNewStyle = false;
                
        bool stateLineStyle  = (GetBits(buff,bitPos,bitPos)==1); bitPos++;
        bool stateFillStyle1 = (GetBits(buff,bitPos,bitPos)==1); bitPos++;
        bool stateFillStyle0 = (GetBits(buff,bitPos,bitPos)==1); bitPos++;
        bool stateMoveTo     = (GetBits(buff,bitPos,bitPos)==1); bitPos++;

        logFile << "    >> stateNewStyle  : " << stateNewStyle << endl;
        logFile << "    >> stateLineStyle : " << stateLineStyle << endl;
        logFile << "    >> stateFillStyle1: " << stateFillStyle1 << endl;
        logFile << "    >> stateFillStyle0: " << stateFillStyle0 << endl;
        logFile << "    >> stateMoveTo    : " << stateMoveTo << endl;
        
        int deltaX = 0, deltaY = 0;
        if (stateFillStyle0 || stateFillStyle1 || stateLineStyle || stateMoveTo || stateNewStyle)
        {
           if (pathData!="")
           {
                        logFile << ">> fillStyle0: " << fillStyle0 << endl;
                        logFile << ">> fillStyle1: " << fillStyle1 << endl;
                        logFile << ">> LineStyle: " << lineStyle << endl;
                        if (DSH_VERBOSE) cout << " **" << endl;
                        pathData = "";
           }
        }
        
        if (stateMoveTo)
        {
           int moveBits = GetBits(buff,bitPos,bitPos+4); bitPos+=5;
           deltaX = GetBits(buff,bitPos,bitPos+moveBits-1); bitPos+=moveBits;
           deltaY = GetBits(buff,bitPos,bitPos+moveBits-1); bitPos+=moveBits;

           deltaX = ConvertIntoSignedBits(deltaX, moveBits);
           deltaY = ConvertIntoSignedBits(deltaY, moveBits);
           
           float fDeltaX = TwipsToFloat(deltaX);
           float fDeltaY = TwipsToFloat(deltaY);

           logFile << "    >> moveBits  : " << moveBits << endl;
           logFile << "    >> deltaX : " << deltaX << " px: " << fDeltaX << endl;
           logFile << "    >> deltaY : " << deltaY << " px: " << fDeltaY <<endl;
           
           currentX = fDeltaX;
           currentY = fDeltaY;
           
           pathData += "M " + FloatToStr(fDeltaX) + " " + FloatToStr(fDeltaY);
           if (DSH_VERBOSE) cout << "M " + FloatToStr(fDeltaX) + " " + FloatToStr(fDeltaY);
           
        }

        if (stateFillStyle0)
        {
           fillStyle0 = GetBitsNew(buff,bitPos,parser_fillIndexBits);
           logFile << "    >> fillStyle0 : " << fillStyle0 <<endl;
           if (DSH_VERBOSE) cout << " <Fill0:" << fillStyle0 << ">";
        }

        if (stateFillStyle1)
        {
           fillStyle1 = GetBitsNew(buff,bitPos,parser_fillIndexBits);
           logFile << "    >> fillStyle1 : " << fillStyle1 <<endl;
           if (DSH_VERBOSE) cout << " <Fill1:" << fillStyle1 << ">";
        }

        if (stateLineStyle)
        {
           lineStyle = GetBitsNew(buff,bitPos,parser_lineIndexBits);
           logFile << "    >> lineStyle : " << lineStyle <<endl;
           if (DSH_VERBOSE) cout << " <LineStyle:" << lineStyle << ">";
        }
        
        
        if (stateNewStyle)
        {
                 if (DSH_VERBOSE) cout << "<stateNewStyle>" << endl;
                 
                 ReadFillStyleArray(buff,bitPos, shape);
                 ReadLineStyleArray(buff,bitPos, shape);
             
             parser_fillIndexBits = GetBits(buff,bitPos,bitPos+3); bitPos+=4;
             parser_lineIndexBits = GetBits(buff,bitPos,bitPos+3); bitPos+=4;

                 if (DSH_VERBOSE) cout << endl << "new parser_fillIndexBits: " << parser_fillIndexBits << endl;
                 if (DSH_VERBOSE) cout << endl << "new parser_lineIndexBits: " << parser_lineIndexBits << endl;

                 if (DSH_VERBOSE) cout << "</stateNewStyle>" << endl;
                 //*/
                 //return;
        }       
        
 
        // End shape record
        if (!stateFillStyle0 && !stateFillStyle1 && !stateMoveTo && !stateLineStyle && !stateNewStyle)
        {
            if (pathData!="")
            {
//                                              shape.AddPath(pathData, fillStyle1, lineStyle);
            }
                endShapeRecord = true;
                if (DSH_VERBOSE) cout << endl << endl;
                
        }
     }
     else
     // Edge records
     {
         edgeCount++;
         bool straightFlag = (GetBits(buff,bitPos,bitPos)==1); bitPos++;
         logFile << "    >> straightFlag : " << straightFlag << endl;
         
                 // Preparing fill style and line style of the line to be read
                 Line line;
                 
         FillStyle lFillStyle0, lFillStyle1;
         lFillStyle0.valid = false;
         lFillStyle1.valid = false;
        
         if (fillStyle1!=0)
                        lFillStyle1 = shape.fillStyle[fillStyle1-1];
         if (fillStyle0!=0)
                        lFillStyle0 = shape.fillStyle[fillStyle0-1];
                
         LineStyle lLineStyle;

         lLineStyle.valid = false;
         if (lineStyle!=0)
                         lLineStyle = shape.lineStyle[lineStyle-1];     

         // Straight Edge
         if (straightFlag)
         {
                        // Flash Spec: numBits = Number of bits per value (2 less than the actual number)
            int numBits = GetBits(buff,bitPos,bitPos+3)+2; bitPos+=4;
            logFile << "    >> numBits : " << numBits << endl;
          
            bool generalLine = (GetBits(buff,bitPos,bitPos)==1); bitPos++;
            logFile << "    >> generalLine : " << generalLine << endl;

                int deltaX = 0, deltaY = 0;
                float fDeltaX = 0, fDeltaY = 0;
            
            if (generalLine)
            {
                                deltaX = GetBits(buff,bitPos,bitPos+numBits-1); bitPos+=numBits;
                                deltaY = GetBits(buff,bitPos,bitPos+numBits-1); bitPos+=numBits;
                                
                deltaX = ConvertIntoSignedBits(deltaX, numBits);
                deltaY = ConvertIntoSignedBits(deltaY, numBits);
                        
                        fDeltaX = TwipsToFloat(deltaX);
                        fDeltaY = TwipsToFloat(deltaY);
                        
                        if (pathData=="")
                        {
                                pathData = "M "+FloatToStr(currentX)+" "+FloatToStr(currentY);
                                if (DSH_VERBOSE) cout << "M "+FloatToStr(currentX)+" "+FloatToStr(currentY);
                        }

                        pathData += " l " + FloatToStr(fDeltaX) + " " + FloatToStr(fDeltaY);
                        if (DSH_VERBOSE) cout << " l " + FloatToStr(fDeltaX) + " " + FloatToStr(fDeltaY);

                logFile << "    >> general line : " << fDeltaX << " " << fDeltaY << endl;
            }
            else
            {
                bool verticalLine = (GetBits(buff,bitPos,bitPos)==1); bitPos++;
                logFile << "    >> verticalLine : " << verticalLine << endl;

                                if (verticalLine)
                                {
                                        deltaY = GetBits(buff,bitPos,bitPos+numBits-1);
                                        bitPos+=numBits;
                                        
                        deltaY = ConvertIntoSignedBits(deltaY, numBits);
                                
                                fDeltaY = TwipsToFloat(deltaY);
                                
                                if (pathData=="")
                                {
                                        pathData = "M "+FloatToStr(currentX)+" "+FloatToStr(currentY);
                                        if (DSH_VERBOSE) cout << "M "+FloatToStr(currentX)+" "+FloatToStr(currentY);
                                }

                                pathData += " v " + FloatToStr(fDeltaY);
                                if (DSH_VERBOSE) cout << " v " + FloatToStr(fDeltaY);

                        logFile << "    >> vertical : " << deltaY << "  py: " << fDeltaY << endl;
                                }
                                if (!verticalLine)
                                {
                                        deltaX = GetBits(buff,bitPos,bitPos+numBits-1);
                                        bitPos+=numBits;
                                
                        deltaX = ConvertIntoSignedBits(deltaX, numBits);
                                
                                fDeltaX = TwipsToFloat(deltaX);
                                
                                if (pathData=="")
                                {
                                        pathData = "M "+FloatToStr(currentX)+" "+FloatToStr(currentY);
                                        if (DSH_VERBOSE) cout << "M "+FloatToStr(currentX)+" "+FloatToStr(currentY);
                                }
                                
                                pathData += " h " + FloatToStr(fDeltaX);
                                if (DSH_VERBOSE) cout << " h " + FloatToStr(fDeltaX);
                                
                        logFile << "    >> horizontal : " << deltaX << "  px: " << fDeltaX << endl;
                                }
           } // end if general/vertical line
           
           float lastX = currentX;
           float lastY = currentY;
           
           currentX += fDeltaX;
           currentY += fDeltaY;
           
           line = Line(lastX,lastY,currentX,currentY);
         }
         // Curved Edge
         else
         {
            int numBits = GetBits(buff,bitPos,bitPos+3)+2; bitPos+=4;
            logFile << "    >> numBits : " << numBits << endl;

                int controlDeltaX = 0, controlDeltaY = 0;
                int anchorDeltaX = 0,  anchorDeltaY = 0;

            controlDeltaX = GetBits(buff,bitPos,bitPos+numBits-1); bitPos+=numBits;
            controlDeltaY = GetBits(buff,bitPos,bitPos+numBits-1); bitPos+=numBits;
            anchorDeltaX  = GetBits(buff,bitPos,bitPos+numBits-1); bitPos+=numBits;
            anchorDeltaY  = GetBits(buff,bitPos,bitPos+numBits-1); bitPos+=numBits;
            
            controlDeltaX = ConvertIntoSignedBits(controlDeltaX, numBits);
            controlDeltaY = ConvertIntoSignedBits(controlDeltaY, numBits);
            anchorDeltaX  = ConvertIntoSignedBits(anchorDeltaX, numBits)+controlDeltaX;
            anchorDeltaY  = ConvertIntoSignedBits(anchorDeltaY, numBits)+controlDeltaY;

            logFile << "    >> controlDeltaX : " << controlDeltaX << "  px: " << TwipsToFloat(controlDeltaX) << endl;
            logFile << "    >> controlDeltaY : " << controlDeltaY << "  py: " << TwipsToFloat(controlDeltaY) << endl;
            logFile << "    >> anchorDeltaX : " << anchorDeltaX << "  px: " << TwipsToFloat(anchorDeltaX) << endl;
            logFile << "    >> anchorDeltaY : " << anchorDeltaY << "  py: " << TwipsToFloat(anchorDeltaY) << endl;

                float fControlDeltaX = TwipsToFloat(controlDeltaX)+currentX;
                float fControlDeltaY = TwipsToFloat(controlDeltaY)+currentY;
                float fAnchorDeltaX  = TwipsToFloat(anchorDeltaX);
                float fAnchorDeltaY  = TwipsToFloat(anchorDeltaY);
                
            float lastX = currentX;
            float lastY = currentY;
           
            currentX += fAnchorDeltaX;
            currentY += fAnchorDeltaY;

            pathData += " curved ";

                if (DSH_VERBOSE) cout << " Q " << fControlDeltaX<< " " << fControlDeltaY << " % " << currentX << " " << currentY;

            line = Line(lastX,lastY,currentX,currentY,true);
                        
                        line.control = Point(fControlDeltaX,fControlDeltaY);
            
         } // end if Curved Edge
          
          line.lineStyle = lLineStyle;
          line.fillStyle0.valid = false;
          
          if (DSH_VERBOSE) cout << " << LineStyle: " << lLineStyle << " >> ";
          
         int requestMode = requestAnalyzer->GetRequestedAction();

         if (requestMode==RQ_MODE_CONVERT)
         {
                  if ((lFillStyle1.valid) || (!lFillStyle0.valid))
                  {
              if (SHS_VERBOSE) if (DSH_VERBOSE) cout << endl << endl << "<< Adding as FillStyle1: " << lFillStyle1 << " (fillStyle0:" << line.fillStyle0 << ")" << endl;
                  shapeSpace->AddLine(line,lFillStyle1);
                  }

          if (lFillStyle0.valid)
          {
                                line.fillStyle0 = lFillStyle1;

                                FillStyle newFillStyle0;
                                newFillStyle0 = lFillStyle0;
                                
                                // Reversing the lines direction
                                Point temp = line.from;
                                line.from = line.to;
                                line.to = temp;

                        if (SHS_VERBOSE) if (DSH_VERBOSE) cout << endl << "<< Adding as FillStyle0: " << newFillStyle0 << " (tricky with fillStyle1:" << line.fillStyle0 << ")" << endl;
                        shapeSpace->AddLine(line,newFillStyle0);
          }
         }
         
         // If only paths are to be generated (for demonstration)
         if (requestMode==RQ_MODE_GENERATEPATHS)
         {
                Path path;
                LineList lineList;
                
                lineList.push_back(line);
                
                path.pathData = ProducePathDataBasic(lineList);
                path.lineStyle = lLineStyle;
                
                path.fillStyle = lFillStyle1;
                
                //cout << "path colour" << path.fillStyle.colour << endl;
                
                // Each shape has another shape related to it for showing only paths
                shape.AddPath(path);

                if (lFillStyle0.valid)
                {
                        path.fillStyle.colour = lFillStyle0.colour;
                        shape.AddPath(path);
                }
                
                if (DSH_VERBOSE) cout << endl << "[GEN_PATHS]: Line added to shape #" << parser_shapeID << " pathCount:" << shape.pathCount << " LineStyle:" << path.lineStyle << endl;
         }

     } // End edge and non-edge shape records
 } // End while for shape records
     if (DSH_VERBOSE) cout << endl;
     if (DSH_VERBOSE) cout << endl;
         
         // Definition of the shape has finished
         
         int requestMode = requestAnalyzer->GetRequestedAction();

         if (requestMode==RQ_MODE_CONVERT)
         {
                 PathVector pathVector;
                 
         shapeSpace->FlushPaths(pathVector);
         
         // Adding all the paths generated by shapeSpace to the relevant shape in the "dictionary"
         for (PathVector::iterator i=pathVector.begin();i!=pathVector.end();i++)
         {
                                shape.AddPath(*i);
         }
         }
         
     if (DSH_VERBOSE) cout << endl;
     
         if (requestAnalyzer->GetVerboseMode()) cout << "        >> Number of edges so far: " << edgeCount << endl;

     return true;
     
}
//----------------------------------------------------------------------------------------
