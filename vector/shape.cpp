//----------------------------------------------------------------------------------------
// Copyright: See definitions.h for License
//----------------------------------------------------------------------------------------
#include "shape.h"
//----------------------------------------------------------------------------------------
Shape::Shape()
{
        pathCount = 0;
        isOnlyPathsDemonstration = false;
        ClearFillStyle();
        ClearLineStyle();
}
//----------------------------------------------------------------------------------------
/*
void Shape::AddPath(const string &pathData, const int &fillIndex, const int &lineIndex)
{
        //
        path[pathCount].hasLineStyle=true;
        if (lineIndex==0) 
        {
                path[pathCount].hasLineStyle=false; 
                path[pathCount].lineStyle.red   = 0xAA;
                path[pathCount].lineStyle.blue  = 0x00;
                path[pathCount].lineStyle.green = 0x0;
        } else
                path[pathCount].lineStyle = lineStyle[lineIndex-1];
                
        path[pathCount].hasFillStyle=true;
        if (fillIndex==0) path[pathCount].hasFillStyle=false; else
                path[pathCount].fillStyle = fillStyle[fillIndex-1];
        
        string pathD = pathData + " z";

        strcpy(path[pathCount].pathData,pathD.c_str());
//      cout << "Path: \"" << path[pathCount].pathData << "\" hasFillStyle: " << path[pathCount].hasFillStyle<< " hasLineStyle: " << path[pathCount].hasLineStyle << endl;
        pathCount++;
}*/
//----------------------------------------------------------------------------------------
void Shape::AddFillStyle(const FillStyle &fillStyleParam, const int index)
{
        if (fillStyle.size()<index+1)
        {
                FillStyle dummyFillStyle;
                dummyFillStyle.valid = false;
                fillStyle.push_back(dummyFillStyle);
        }

        fillStyle[index] = fillStyleParam;
        fillStyleCount++;
}
//----------------------------------------------------------------------------------------
void Shape::AddLineStyle(const LineStyle &lineStyleParam, const int index)
{
        if (lineStyle.size()<index+1)
        {
                LineStyle dummyLineStyle;
                dummyLineStyle.valid = false;
                lineStyle.push_back(dummyLineStyle);
        }

        lineStyle[index] = lineStyleParam;
        lineStyleCount++;
}
//----------------------------------------------------------------------------------------
void Shape::AddPath(const Path &pathParam)
{
        path.push_back(pathParam);
        pathCount++;
}
//----------------------------------------------------------------------------------------
void Shape::ClearFillStyle()
{
        fillStyle.clear();
        
        FillStyle dummyFillStyle;
        dummyFillStyle.valid = false;
        AddFillStyle(dummyFillStyle,0);
}
//----------------------------------------------------------------------------------------
void Shape::ClearLineStyle()
{
        lineStyle.clear();
        
        LineStyle dummyLineStyle;
        dummyLineStyle.valid = false;
        AddLineStyle(dummyLineStyle,0);
}
//----------------------------------------------------------------------------------------
