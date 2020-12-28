//----------------------------------------------------------------------------------------
// Copyright: See definitions.h for License
//----------------------------------------------------------------------------------------
#ifndef __SVGFORMAT__
#define __SVGFORMAT__
//----------------------------------------------------------------------------------------
#include "../definitions.h"
#include "vectorformat.h"
#include "font.h"
#include "text.h"
//----------------------------------------------------------------------------------------
#define SVG_VERBOSE false
#define tab "    "
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
class SVGFormat: public VectorFormat
{
      public:
             SVGFormat();                           // Constructor
             ~SVGFormat();                          // Destructor

             // Handles messages received by this later
             virtual bool IncommingEventHandler(const int &, const string &, int *, float *, void *);
             
      protected:
                 bool WriteFontTags(ofstream &);
                 bool WriteTextTags(ofstream &);
                 bool WriteMarkerTag(ofstream &, const string &, const RGB &);
                 
             // Flags (to be set false at init)
             bool fl_Validated, fl_FrameDefined, fl_Ended;
             bool fl_BackgroundSet;
             
             float width, height;
             string backgroundStr;
             
             //ShapeList placedShape;
             int placedShapeCount;
             PlaceTagInfoList placedShape;
             
             ShapeVector definedShape;
             FontList definedFont;
             TextList definedText;

             int definedShapeCount;
             
             int headerCount;
             int bodyCount;
             int footerCount;
             
             virtual void Parse();
             virtual void FlushOutput();
             
};
//----------------------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------------------
