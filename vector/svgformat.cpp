//----------------------------------------------------------------------------------------
// Copyright: See definitions.h for License
//----------------------------------------------------------------------------------------
#include "svgformat.h"
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
SVGFormat::SVGFormat()
{
//cout << "Hi I'm SVG" << endl;
headerCount = 0;
footerCount = 0;
bodyCount = 0;
fl_Validated = false;
fl_FrameDefined = false;
fl_Ended = false;
fl_BackgroundSet = false;
placedShapeCount = 0;
definedShapeCount = 0;
backgroundStr = "";
}
//----------------------------------------------------------------------------------------
SVGFormat::~SVGFormat()                          // Destructor
{
}
//----------------------------------------------------------------------------------------
bool SVGFormat::IncommingEventHandler(const int & msg, const string & strParam, int * intParam, float * floatParam, void * untypedPtr)
{
     switch (msg)
     {
            case EV_VALID:
            {
                    fl_Validated = true;
            }
            break;     
            case EV_FRAME:
            {
                 // Transforming flash's twips standard into pixels, deviding it by 20
                 fl_FrameDefined = true;
                 width = floatParam[1];
                 height = floatParam[3];
            }
            break;     
            case EV_END:
            {
                 fl_Ended = true;
            }
            break;     
            case EV_SETBKG:
            {
                 fl_BackgroundSet = true;
                 backgroundStr = "#"+ByteToHex(intParam[0])+ByteToHex(intParam[1])+ByteToHex(intParam[2]);
            }
            break;
            case EV_PLACEOBJECT:
            case EV_PLACEOBJECT2:
            {
                        PlaceTagInfo placeInfo;
                        
                    placeInfo.characterID = intParam[0];
                    placeInfo.hasMatrix = (intParam[1]==MFL_PLACETAGHASMATRIX);
                    
                    if (placeInfo.hasMatrix)
                    {
                                    Matrix *matrix = new Matrix();
                                    matrix = (Matrix *) untypedPtr;
                                    
                                    placeInfo.matrix = *matrix;
                                    
//                                  cout << ">>  matrix.translateX: " << matrix->translateX << endl;
//                                  cout << ">>  matrix.translateY: " << matrix->translateY << endl;
                    }

                                        placedShape.push_back(placeInfo);

                    placedShapeCount++;
            }
            break;
            case EV_DEFINESHAPE:
            {
                Shape *shapeParam = new Shape();
                shapeParam = (Shape *) untypedPtr;

                definedShape.push_back(*shapeParam);
/*                cout << " NEW SHAPE !!!!!! " << endl;
                cout << " shape ID: " << shapeParam->characterID << endl;
                cout << " pathCount: " << shapeParam->pathCount << endl;*/

                definedShapeCount++;
                 /*
                    shape[shapeCount].x = floatParam[0];
                    shape[shapeCount].y = floatParam[1];
                    shape[shapeCount].width = floatParam[2];
                    shape[shapeCount].height = floatParam[3];
                    shape[shapeCount].characterID = intParam[0];

                    shapeCount++;
                 */
            }
            break;
            case EV_DEFINEFONT:
            {
                Font *fontParam = new Font();
                fontParam = (Font *) untypedPtr;

                definedFont.push_back(*fontParam);
            }
            break;
            case EV_DEFINETEXT:
            {
                Text *textParam = new Text();
                textParam = (Text *) untypedPtr;
                
                definedText.push_back(*textParam);
            }
            break;
            
     }
     
}
//----------------------------------------------------------------------------------------
void SVGFormat::Parse()
{
 cout << "SVG Parser!" << endl;
}
//----------------------------------------------------------------------------------------
void SVGFormat::FlushOutput()
{
if (requestAnalyzer->GetVerboseMode()) cout << endl << "SVG Writer: Outputing '" << GetIOFileName() << "' ..." << endl << endl;
if (! (fl_Validated && fl_FrameDefined && fl_Ended) )
{
      cout << "Error: " << endl;
      if (! fl_Validated) cout << "Invalid Flash file or compression unsupported" << endl;
      if (! fl_FrameDefined) cout << "Frame is not defined" << endl;
      if (! fl_Ended) cout << "No end tag detected" << endl;
//      return;
}

ofstream outFile(ioFileName.c_str(), ios::out);

float amplifier = 1;

// Setting the precision of the output stream
outFile << fixed << setprecision(3);

outFile << "<?xml version=\"1.0\" standalone=\"no\"?>" << endl;
outFile << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\"" << endl;
outFile << "                 \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">" << endl;

outFile << "" << endl;
outFile << "<svg" << 
           " width=\"" << width*amplifier << "\" height=\"" << height*amplifier << "\"" << 
           " version=\"1.1\"";

outFile << endl << "     xmlns=\"http://www.w3.org/2000/svg\"" << endl;
outFile         << "     xmlns:xlink=\"http://www.w3.org/1999/xlink\">" << endl;
if (SVG_VERBOSE) cout << "Defined shapes count: " << definedShapeCount << endl;
if (SVG_VERBOSE) cout << "Placed shapes count: " << placedShapeCount << endl;

outFile << "" << endl;
outFile << tab << "<defs>" << endl;


bool backgroundRectPresent = false;
if ((fl_BackgroundSet)&&(backgroundStr!="#ffffff"))
{
        outFile << endl;
        outFile << tab << tab;
        outFile << "<g";
    outFile << " id=\"BackgroundFill\"";
    outFile << ">" << endl;

    outFile << tab << tab << tab;
    outFile << "<rect x=\"0\" y=\"0\"";
    outFile << " width=\"" << width <<"\"";
    outFile << " height=\"" << height <<"\"";
    outFile << " fill=\"" << backgroundStr <<"\"";
    outFile << "/>" << endl;

    outFile << tab << tab << "</g>" << endl << endl;
    
    backgroundRectPresent = true;
}

// Produce font glyph tags
WriteFontTags(outFile);

// Produce text tags
WriteTextTags(outFile);

//cout << "definedShapeCount: " << definedShapeCount << endl;
for (int i=0;i<definedShapeCount;i++)
{
//      cout << "shape["<<i<<"]: "<< endl;
    Shape shape = definedShape[i];
    outFile << "" << endl;
    outFile << tab << tab;
//               "<rect x=\"" << shape.x*amplifier << "\" y=\"" << shape.y*amplifier << "\"" <<
//               " width=\"" << shape.width*amplifier << "\" height=\"" << shape.height*amplifier << "\"" << 

        outFile << "<g";
    outFile << " id=\"shape" << shape.characterID << "\"";
    outFile << ">" << endl;

        if (SVG_VERBOSE) cout << "shape[" << i << "].pathCount = "<< shape.pathCount << endl;
    for (int j=0; j<shape.pathCount;j++)
    {
        if (SVG_VERBOSE) cout << "path: " << shape.path[j] << endl;
        
        string gradientPatternReference = "pattern"+IntToStr(j);
        string gradientFillReference = "gradient"+IntToStr(j);
        string gradientTag="<<<errorTag>>>";
        
                FillStyle fillStyle = shape.path[j].fillStyle;

        if (fillStyle.type==MFL_FILL_LINEAR) gradientTag="linearGradient";
        if (fillStyle.type==MFL_FILL_RADIAL) gradientTag="radialGradient";
        
        if ((fillStyle.type==MFL_FILL_RADIAL)||(fillStyle.type==MFL_FILL_LINEAR))
        {
                Matrix matrix = fillStyle.gradient.matrix;
                        outFile << endl << tab << tab << tab; 
                        outFile << "<defs>";
                        outFile << endl << tab << tab << tab << tab;
                        outFile << "<pattern id=\""<<gradientPatternReference<<"\"";
                        outFile << " patternContentUnits=\"userSpaceOnUse\"";
                        outFile << " patternUnits=\"userSpaceOnUse\"";
                        outFile << ">";
                        outFile << endl << tab << tab << tab << tab << tab;

                        outFile << "<"+gradientTag;
                        outFile << " id=\""<< gradientFillReference << "\"";
                        outFile << ">";
                        GradientRecordVector gradientRecordVector = fillStyle.gradient.gradientRecordVector;
                        
                        // For painting the remaining space of the pattern with the last stopped gradient
                        RGB lastGradientColour;
                        for (GradientRecordVector::iterator i=gradientRecordVector.begin();i!=gradientRecordVector.end();i++)
                        {
                                GradientRecord gradientRecord = *i;
                                outFile << endl << tab << tab << tab << tab << tab << tab;
                            outFile << "\t<stop offset=\"" << ((gradientRecord.ratio*100)/255) << "%\"";
                            outFile << " stop-color=\"#"<<gradientRecord.colour.ToString()<<"\"";
                            if (gradientRecord.colour.alphaEnabled)
                                outFile << " stop-opacity=\""<<gradientRecord.colour.Opacity()<<"\"";
                            outFile << "/>";
                            
                            lastGradientColour = gradientRecord.colour;
                        }
                        outFile << endl << tab << tab << tab << tab << tab;
                        outFile << "</"+gradientTag+">";

                        outFile << endl << tab << tab << tab << tab << tab;
                        outFile << "<rect";
                        outFile << " x=\"" << shape.x << "\" y=\"" << shape.y << "\"";
                        outFile << " width=\"" << shape.width << "\" height=\"" << shape.height << "\"";
                    outFile << " fill=\"#"<<lastGradientColour.ToString()<<"\"";
                    if (lastGradientColour.alphaEnabled)
                        outFile << " fill-opacity=\""<<lastGradientColour.Opacity()<<"\"";
                        outFile << "/>";
                        
                        outFile << endl << tab << tab << tab << tab << tab;
                        outFile << "<g";
/*                      outFile << " transform=\"matrix("<<matrix.scaleX<<" "<<matrix.rotateSkew0;
                        outFile << " "<<matrix.rotateSkew1<<" "<< matrix.scaleY;
                        outFile << " "<< matrix.translateX <<" "<< matrix.translateY <<")\""; */
                        outFile << " transform=\"matrix("<<matrix.ToString()<<")\"";
                        outFile << ">";
                        
                        outFile << endl << tab << tab << tab << tab << tab << tab;
                        outFile << "<rect x=\"-819.2\" y=\"-819.2\" width=\"1638.4\" height=\"1638.4\"";
                        outFile << " fill=\"url(#"<<gradientFillReference<<")\"/>";

                        outFile << endl << tab << tab << tab << tab << tab;
                        outFile << "</g>";

                        outFile << endl << tab << tab << tab << tab;
                        outFile << "</pattern>";
                        
                        outFile << endl << tab << tab << tab; 
                        outFile << "</defs>" << endl;
        }
        
        string markerName = "marker"+IntToStr(shape.characterID)+"-"+IntToStr(j);

                // In case we are only generating paths define the arrow "markers"
                if ((requestAnalyzer->GetRequestedAction()==RQ_MODE_GENERATEPATHS)&&(shape.isOnlyPathsDemonstration))
                {
                        WriteMarkerTag(outFile,markerName,shape.path[j].fillStyle.colour);
                }

                outFile << tab << tab << tab << "<path";
                outFile << " d=\"" << shape.path[j].pathData << "\"";
        
                outFile << endl << GL_PATHDATATAB;

                // RGBs have alpha if the tag had been DefineShape3
/*          if (shape.version==MFL_TAG_DEFINEBITS3)
            {
               float opacity = shape.path[j].fillStyle.colour.Alpha() / 255.0;
               outFile << setprecision(2) << " opacity=\"" << opacity << "\" ";
            }
*/      
                // In case we are only generating paths
            if ((requestAnalyzer->GetRequestedAction()==RQ_MODE_GENERATEPATHS)&&(shape.isOnlyPathsDemonstration))
            {
                // In case the path doesn't have any stroke so we add a dashed line to demonstrate it
/*              if ((!shape.path[j].lineStyle.valid))
                {
                                LineStyle  blankLineStyle;
                                blankLineStyle.valid = true;
                                blankLineStyle.lineWidth = 5;
                                blankLineStyle.colour = RGB(0,0,0);
        
                        shape.path[j].lineStyle = blankLineStyle;
                        
                }*/
                        
                        // If no line style, make the line stroke dashed
                        if (!shape.path[j].lineStyle.valid)
                        {
                        outFile << " stroke-dasharray=\"2,3\"";
                        shape.path[j].lineStyle.lineWidth -=20;
                        }

                        // Increase lineStyle for better demonstration
                shape.path[j].lineStyle.lineWidth+=3*20;
                shape.path[j].lineStyle.colour = shape.path[j].fillStyle.colour;
                shape.path[j].lineStyle.valid = true;

                outFile << " marker-end=\"url(#"<<markerName<<")\"";

                        outFile << endl << tab << tab << tab << tab << tab;
                
                    outFile << " fill-opacity=\"0.0\"";
            }
            
            if (shape.path[j].lineStyle.valid)
            {
               float strokeWidth = ((float) shape.path[j].lineStyle.lineWidth) / GL_FLASHTWIPSPERPX; 
               
               outFile << " stroke-width=\"" << (strokeWidth)*amplifier << "\"";

               LineStyle lineStyle = shape.path[j].lineStyle;
               outFile << " stroke=\"" << lineStyle.colour << "\"";
            }

                if (requestAnalyzer->GetRequestedAction()==RQ_MODE_CONVERT)
                {
                    if (fillStyle.valid)
                    {
                        if (fillStyle.type==MFL_FILL_SOLID)
                        {
                                    outFile << " fill=\"#" << fillStyle.colour.ToString() << "\"";
                        } else
                        if ((fillStyle.type==MFL_FILL_RADIAL)||(fillStyle.type==MFL_FILL_LINEAR))
                        {
                                    outFile << " fill=\"url(#" << gradientPatternReference << ")\"";
                        }
                        
                        if (fillStyle.colour.alphaEnabled)
                        {
                               //float opacity = ((float)fillStyle.colour.Alpha()) / 255.0f;
                               outFile << " fill-opacity=\"" << fillStyle.colour.Opacity() << "\" ";
                               //cout << endl <<  "fill-opacity=: " << fillStyle.colour.Alpha() << endl;
                        }
        
                    } else
                    {
                            outFile << " fill-opacity=\"0.0\"";
                    }
        }

                outFile << "/>" << endl;
    }
    
        outFile << tab << tab << "</g>" << endl;
}
outFile << "" << endl;
outFile << tab << "</defs>" << endl;

if (backgroundRectPresent)
    outFile <<endl<<tab<<"<use xlink:href=\"#backgroundFill\"/>" << endl;

for (PlaceTagInfoList::iterator i = placedShape.begin();i!=placedShape.end();i++)
{
    PlaceTagInfo placeInfo = *i;

        if ((requestAnalyzer->GetRequestedAction()!=RQ_MODE_GENERATEPATHS)||(definedShape[placeInfo.characterID-1].isOnlyPathsDemonstration))
        {
            outFile << "" << endl;
        
                string currentTab=tab;
                string scaleTab=currentTab;
                
            if (placeInfo.hasMatrix)
            {
                outFile << tab << "<g transform=\"matrix(" << placeInfo.matrix.ToString() <<")\">" << endl;
            }
        
            outFile << tab << tab << "<use xlink:href=\"#shape" << placeInfo.characterID << "\"/>" << endl;
        
            if (placeInfo.hasMatrix)
            {
                outFile << tab << "</g>" << endl;
            }
        }
}

outFile << "" << endl;
outFile << "</svg>" << endl;


cout << "SVG successfully written!" << endl;

}
//----------------------------------------------------------------------------------------
bool SVGFormat::WriteFontTags(ofstream &outFile)
{
        FontList::iterator i = definedFont.begin();
        for(;i!=definedFont.end();i++)
        {
                Font font = *i;
                
                string fontName = "font"+IntToStr(font.characterID);
//              cout <<"<"<< fontName <<">"<<endl;
                
                int glyphIndex=0;
        
                ShapeVector::iterator j = font.glyph.begin();
                for (;j!=font.glyph.end();j++)
                {
                        Shape shape= *j;

                        string glyphName = "glyph"+IntToStr(glyphIndex);
                        
//                      cout <<"\t<"<< glyphName <<">"<<endl;
                        outFile << endl << tab << tab << tab;
                        outFile << "<path";
                        if (shape.pathCount>0)
                                outFile << " d=\"" << shape.path[0].pathData << "\"";
                        if (shape.pathCount>1)
                                cout << "FATAL ERROR: " << fontName << "-" << glyphName << " has more than a single path." << endl;
                        outFile << " id=\""<<fontName<<"-"<<glyphName<<"\"";
                        outFile << "/>";
                        outFile << endl << tab << tab << tab;
//                      cout <<"\t</"<< glyphName <<">"<<endl;

                        glyphIndex++;
                }

//              cout <<"</"<< fontName <<">"<<endl;
        }
        
        outFile << endl;
        
        return true;
}
//----------------------------------------------------------------------------------------
bool SVGFormat::WriteTextTags(ofstream &outFile)
{
        TextList::iterator i = definedText.begin();
        for(;i!=definedText.end();i++)
        {
                Text text = *i;
                
                string textName = "shape"+IntToStr(text.characterID);
                //cout << textName <<":"<<endl;
                
                outFile << endl << tab << tab << tab;
                outFile << "<g";
                outFile << " id=\""<<textName<<"\"";
                outFile << " transform=\"matrix("<<text.matrix.ToString()<<")\"";
                outFile << ">";

                int textRecordIndex=0;
        
                TextRecordList::iterator j = text.textRecordList.begin();
                for (;j!=text.textRecordList.end();j++)
                {
                        TextRecord textRecord= *j;

                        string textRecordName = "string"+IntToStr(textRecordIndex);
                        
                        outFile << endl << tab << tab << tab << tab;
                        outFile << "<g";
                        outFile << " id=\""<<textRecordName<<"\"";
                        outFile << " fill=\"" << textRecord.colour << "\"";
                        
                        float emSize = 1024 / 20;
                        float scale = textRecord.textHeight / emSize;
                        
                        outFile << " transform=\"translate("<<textRecord.xOffset << ", " << textRecord.yOffset << ")\"";
                        //<< scale << " 0 0 " << scale<< " " << 
                        outFile << ">";
                        
                        string fontName = "font"+IntToStr(textRecord.fontID);
                        
                        GlyphEntryList::iterator k = textRecord.glyphEntryList.begin();

                        float xPoint = 0;
                        
                        for (;k!=textRecord.glyphEntryList.end();k++)
                        {
                                GlyphEntry glyphEntry = *k;
                                string glyphName = "glyph"+IntToStr(glyphEntry.glyphIndex);
                                
                                outFile << endl << tab << tab << tab << tab << tab;
                                outFile << "<g transform=\"translate("<<xPoint<<",0)scale("<<scale<<") \"> <use xlink:href=\"#"<<fontName<<"-"<<glyphName<<"\"/></g>";
                                
                                xPoint += glyphEntry.glyphAdvance;
                        }
                        
                        outFile << endl << tab << tab << tab << tab;
                        outFile << "</g>";

                        textRecordIndex++;
                }

                outFile << endl << tab << tab << tab;
                outFile << "</g>";
                outFile << endl;
        }
        
        outFile << endl;
        
        return true;
}
//----------------------------------------------------------------------------------------
bool SVGFormat::WriteMarkerTag(ofstream &outFile, const string &markerName, const RGB &colour)
{
        outFile << endl;
        outFile << tab << tab << tab << "<marker id=\""<<markerName<<"\" viewBox=\"-10 0 0 10\" refX=\"-1\" refY=\"5\"" << endl;
    outFile << tab << tab << tab << "        markerUnits=\"strokeWidth\" orient=\"auto\"";
    outFile << " markerWidth=\"4\" markerHeight=\"3\">" << endl;
    outFile << tab << tab << tab << tab << "<polyline points=\"-10,0 0,5 -10,10 -7,5 -10,0\" fill=\""<<colour<<"\" />" << endl;
    outFile << tab << tab << tab << "</marker>" << endl;
//    outFile << endl;
}
//----------------------------------------------------------------------------------------
