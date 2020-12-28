//----------------------------------------------------------------------------------------
// Copyright: See definitions.h for License
//----------------------------------------------------------------------------------------
#include "../definitions.h"
#include "matrix.h"
#include "shapebasics.h"
//----------------------------------------------------------------------------------------
#ifndef __SHAPE__
#define __SHAPE__
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
class Shape
{
        public:
        
                Shape();
                void AddFillStyle(const FillStyle &, const int);
                void AddLineStyle(const LineStyle &, const int);
                void AddPath(const Path &);
                
                void ClearFillStyle();
                void ClearLineStyle();

                int version; // has been defined with which tag: DefinShape[1], 2 or 3?
                
            float x, y, width, height;
            int characterID;
            Matrix transformMatrix;
            
            int fillStyleCount;
            FillStyleVector fillStyle;
        
            int lineStyleCount;
            LineStyleVector lineStyle;
        
                int pathCount;
                
                bool isOnlyPathsDemonstration;
                
                PathVector path;
};
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
typedef vector<Shape> ShapeVector;
//----------------------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------------------
