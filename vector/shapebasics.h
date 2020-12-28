//----------------------------------------------------------------------------------------
// Copyright: See definitions.h for License
//----------------------------------------------------------------------------------------
#include "../definitions.h"
#include "matrix.h"
//----------------------------------------------------------------------------------------
#ifndef __SHAPEBASICS__
#define __SHAPEBASICS__
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
class Point
{
        friend ostream &operator<<(ostream &, const Point &);

        public:
                Point();
                Point(const float &, const float &);
                float x,y;
                bool operator==(const Point &) const;
};
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
struct RECT
{
        Point from, size;
};
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
class RGB
{
        friend ostream &operator<<(ostream &, const RGB &);
        friend ofstream &operator<<(ofstream &, const RGB &);

        public:
            RGB();
                RGB(const int &, const int &, const int &, const int & = GL_NOALPHA);

                void SetRGB(const int &, const int &, const int &, const int & = GL_NOALPHA);
                void ImportAlpha(const RGB &);
                void CombineWith(const RGB &);
                
                int Red() const;
                int Green() const;
                int Blue() const;
                int Alpha() const;
                float Opacity() const;

                string ToString() const;
                int ToInt() const;
                void FromInt(const int &);

                bool operator<(const RGB &) const;
                bool operator==(const RGB &) const;
                
                bool alphaEnabled;

        private:
                int red, green, blue;
                int alpha;
};
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
class GradientRecord
{
        public:
            bool operator==(const GradientRecord &) const;
            
                int ratio;
                RGB colour;
};        
//----------------------------------------------------------------------------------------
typedef vector<GradientRecord> GradientRecordVector;
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
class Gradient
{
        public:
                Gradient(); // Constructor
                
        bool operator==(const Gradient &) const;

                int count;
                GradientRecordVector gradientRecordVector;
                Matrix matrix;
};
//----------------------------------------------------------------------------------------
typedef list<PlaceTagInfo> PlaceTagInfoList;
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
class FillStyle
{
        friend ostream &operator<<(ostream &, const FillStyle &);

        public:
           FillStyle(); 
       bool operator==(const FillStyle &) const;
       void SetRGB(const RGB &rgb);

           bool valid;
       int type;
       RGB colour;
//       bool isFill0;
       Gradient gradient;
       int bitmapCharacterID; 
       
};
//----------------------------------------------------------------------------------------
typedef vector<FillStyle> FillStyleVector;
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
class LineStyle
{
        friend ostream &operator<<(ostream &, const LineStyle &);

        public:
           LineStyle(); 
       bool operator==(const LineStyle &) const;
           
           bool valid;
       int lineWidth;
       RGB colour;
};
//----------------------------------------------------------------------------------------
typedef vector<LineStyle> LineStyleVector;
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
class Path
{
        friend ostream &operator<<(ostream &output, const Path &path);

        public:
           FillStyle fillStyle;
           LineStyle lineStyle;
           
           string pathData;
};
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
class Line
{
        friend ostream &operator<<(ostream &, const Line &);

        public:
                Line();
                Line(const float &,const float &,const float &,const float &, const bool & = false);
                Point from, to;
                Point control;
                LineStyle lineStyle;
                FillStyle fillStyle0;
                bool isCurved;
};
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
typedef vector<Path> PathVector;
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
typedef list<Line> LineList;
//----------------------------------------------------------------------------------------
string ProducePathDataBasic(const LineList &);
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------------------
