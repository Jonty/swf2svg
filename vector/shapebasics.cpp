//----------------------------------------------------------------------------------------
// Copyright: See definitions.h for License
//----------------------------------------------------------------------------------------
#include "shapebasics.h"
//----------------------------------------------------------------------------------------
// LineStyle Class Implementation
//----------------------------------------------------------------------------------------
LineStyle::LineStyle()
{
        valid = false;
}
//----------------------------------------------------------------------------------------
bool LineStyle::operator==(const LineStyle &lineStyleRight) const
{
        // Both invalid
        if (!lineStyleRight.valid && !valid)
                return true;
                
        // Both valid and same properties
        if (lineStyleRight.valid && valid)
                return (lineStyleRight.colour == colour) && (lineStyleRight.lineWidth == lineWidth);
                
        // Otherwise they differ in validity, then not equal
        return false;
}
//----------------------------------------------------------------------------------------
// Gradient Class Implementation
//----------------------------------------------------------------------------------------
Gradient::Gradient()
{
        count = 0;
}
//----------------------------------------------------------------------------------------
bool GradientRecord::operator==(const GradientRecord &gradientRecordRight) const
{
        bool result =  (gradientRecordRight.colour==colour);
        result = result && (gradientRecordRight.ratio==ratio);
        
        return result;
}
//----------------------------------------------------------------------------------------
bool Gradient::operator==(const Gradient &gradientRight) const
{
        bool result = (gradientRight.count==count);
        //cout << "Result : " << result << endl;
        GradientRecordVector grv = gradientRight.gradientRecordVector;
        GradientRecordVector::const_iterator i=grv.begin();
        GradientRecordVector::const_iterator j=gradientRecordVector.begin();
        
        for (;((i!=grv.end())&&(result));i++)
        {
                GradientRecord gr1 = *i;
                GradientRecord gr2 = *j;
                result = result && (gr1 == gr2);
                j++;
        }
        
        //result = result && (gradientRight.matrix==matrix);

        return result;
}
//----------------------------------------------------------------------------------------
// FillStyle Class Implementation
//----------------------------------------------------------------------------------------
FillStyle::FillStyle()
{
        valid = false;
        colour = RGB(0,0,0);
        type = -1;
        bitmapCharacterID = 0;
}
//----------------------------------------------------------------------------------------
bool FillStyle::operator==(const FillStyle &fillStyleRight) const
{
        // Both invalid
        if (!fillStyleRight.valid && !valid)
                return true;

        // Both valid but different types
        if (fillStyleRight.type != type)
                return false;
                
        // Both valid and same properties
        if (fillStyleRight.valid && valid)
        {
                if (type==MFL_FILL_SOLID)
                        return (fillStyleRight.colour == colour);

                if (type==MFL_FILL_LINEAR || type==MFL_FILL_RADIAL)
                        return (fillStyleRight.gradient == gradient);
                        
                if (type==MFL_FILL_CLIPPED_BMP || type==MFL_FILL_NS_CLIPPED_BMP ||
                        type==MFL_FILL_REPEAT_BMP || type==MFL_FILL_NS_REPEAT_BMP)
                        return (fillStyleRight.bitmapCharacterID==bitmapCharacterID);
        }
                
        // Otherwise they differ in validity (or other unusual cases), then not equal
        return false;
}
//----------------------------------------------------------------------------------------
void FillStyle::SetRGB(const RGB &rgb)
{
        colour = rgb;
        valid = true;
}
//----------------------------------------------------------------------------------------
// RGB Class Implementation
//----------------------------------------------------------------------------------------
RGB::RGB()
{
        red=0; green=0; blue=0;
        alphaEnabled = false;
}
//----------------------------------------------------------------------------------------
float RGB::Opacity() const
{
        float opacity = ((float)Alpha()) / 255.0f;
        return opacity;
}
//----------------------------------------------------------------------------------------
RGB::RGB(const int &r, const int &g, const int &b, const int &a)
{
        SetRGB(r,g,b,a);
}
//----------------------------------------------------------------------------------------
void RGB::SetRGB(const int &r, const int &g, const int &b, const int &a)
{
        red=r; green=g; blue=b; alpha=a;

        if (a==-1 || a==255)
                alphaEnabled=false;
}
//----------------------------------------------------------------------------------------
void RGB::ImportAlpha(const RGB &rgbParam)
{
        alphaEnabled = rgbParam.alphaEnabled;
        alpha = rgbParam.alpha;
}
//----------------------------------------------------------------------------------------
void RGB::CombineWith(const RGB &rgbParam)
{
        red = (rgbParam.red + red) / 2;
        green = (rgbParam.green + green) / 2;
        blue = (rgbParam.blue + blue) / 2;
}
//----------------------------------------------------------------------------------------
int RGB::Red() const
{
        return red;
}
//----------------------------------------------------------------------------------------
int RGB::Green() const
{
        return green;
}
//----------------------------------------------------------------------------------------
int RGB::Blue() const
{
        return blue;
}
//----------------------------------------------------------------------------------------
int RGB::Alpha() const
{
        return alpha;
}
//----------------------------------------------------------------------------------------
string RGB::ToString() const
{
        string ret = ByteToHex(red)+ByteToHex(green)+ByteToHex(blue);
        return ret;
}
//----------------------------------------------------------------------------------------
int RGB::ToInt() const
{
        return blue+256*(green+256*(red));
}
//----------------------------------------------------------------------------------------
void RGB::FromInt(const int &colour)
{
        blue  = (colour && 0x0000FF);
        green = (colour && 0x00FF00) >> 8;
        green = (colour && 0xFF0000) >> 16;
}
//----------------------------------------------------------------------------------------
bool RGB::operator<(const RGB &rightRGB) const
{
        return (red < rightRGB.red);
}
//----------------------------------------------------------------------------------------
bool RGB::operator==(const RGB &rightRGB) const
{
        return (red == rightRGB.red) && (green == rightRGB.green) && (blue == rightRGB.blue) && (alpha == rightRGB.alpha);
}
//----------------------------------------------------------------------------------------
Point::Point()
{
        x = 0;
        y = 0;
}
//----------------------------------------------------------------------------------------
Point::Point(const float &paramX, const float &paramY)
{
        x = paramX;
        y = paramY;
}
//----------------------------------------------------------------------------------------
bool Point::operator==(const Point &param) const
{
        bool xFactor = CompareFP(param.x,x);
        bool yFactor = CompareFP(param.y,y);
        return (xFactor && yFactor);
}
//----------------------------------------------------------------------------------------
Line::Line()
{
        
}
//----------------------------------------------------------------------------------------
Line::Line(const float &x1,const float &y1,const float &x2,const float &y2,const bool &boolParam)
{
        from.x = x1;
        from.y = y1;
        to.x   = x2;
        to.y   = y2;
        isCurved = boolParam;
}
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
ostream &operator<<(ostream &output, const RGB &rgb)
{
        output << "#" << rgb.ToString();
        
        if (rgb.alphaEnabled)
        {
                output << " a: " << rgb.alpha;
        }
        
        return output;
}
//----------------------------------------------------------------------------------------
ofstream &operator<<(ofstream &output, const RGB &rgb)
{
        output << "#" << rgb.ToString();
        return output;
}
//----------------------------------------------------------------------------------------
ostream &operator<<(ostream &output, const FillStyle &fillStyle)
{
        bool valid = fillStyle.valid;

        output << "(FillStyle) valid: " << valid;
        
        if (valid)
        {
                int type = fillStyle.type;
                output << " type: " << type;
                
                if (type==MFL_FILL_SOLID)
                        output << " colour: " << fillStyle.colour ;

                if (type==MFL_FILL_LINEAR || type==MFL_FILL_RADIAL)
                {
                        int count = fillStyle.gradient.count;
                        output << " grad-count: " << count;
                        
                        if (count>0)
                        {
                                GradientRecord gradRec = fillStyle.gradient.gradientRecordVector[0];
                                output << " grad[0]: " << gradRec.colour;
                        }
                }
                
                if (type==MFL_FILL_CLIPPED_BMP || type==MFL_FILL_NS_CLIPPED_BMP ||
                        type==MFL_FILL_REPEAT_BMP || type==MFL_FILL_NS_REPEAT_BMP)
                        output << " bitmapCharacterID: " << fillStyle.bitmapCharacterID << endl;
        }
                
        return output;
}
//----------------------------------------------------------------------------------------
ostream &operator<<(ostream &output, const LineStyle &lineStyle)
{
        output << "(LineStyle) valid: " << lineStyle.valid << " colour: " << lineStyle.colour << " width:" << lineStyle.lineWidth;
        return output;
}
//----------------------------------------------------------------------------------------
ostream &operator<<(ostream &output, const Path &path)
{
        output << "d = \"" << path.pathData << "\"";
        if (path.lineStyle.valid) 
                output << " lineStyle:  " << path.lineStyle; 
        else
                output << " [noLineStyles]";
        
        if (path.fillStyle.valid) 
                output << " fillStyle:  " << path.fillStyle;
        else
                output << " [noFillStyles]";
        
        return output;
}
//----------------------------------------------------------------------------------------
ostream &operator<<(ostream &output, const Point &point)
{
        output << "[Point] (" << point.x << ", "<< point.y << ")";

        return output;
}
//----------------------------------------------------------------------------------------
ostream &operator<<(ostream &output, const Line &line)
{
        output << "[Line] (" << line.from.x << ", "<< line.from.y << ")";
        output << " (" << line.to.x << ", "<< line.to.y <<")";

        if (line.isCurved)
                output << " Curved Q(" << line.control.x << ", "<< line.control.y <<")";
        return output;
}
//----------------------------------------------------------------------------------------
string ProducePathDataBasic(const LineList &sp)
{
// Before attempting to write the path into the string trying to optimize its positionings
//LineList sp = lineListParam;
//OptimizeLineList(sp);

string str = "", strTest="";
Point lastPoint = Point(0xFFFFFFF,0xFFFFFF);

// Position of the last "\n"
int lastNewLine = 0;

bool isFirstLine = true;
Point firstPoint = Point(0,0);

for (LineList::const_iterator j=sp.begin();j!=sp.end();j++)
{
        Line line = *j;
        
        if (isFirstLine)
        {
                firstPoint = line.from;
                isFirstLine = false;
        }

        // Inserts new line to make paths more readable
        if (str.length()-lastNewLine>GL_PATHDATAWIDTH)
        {
                str += "\n";
                str += GL_PATHDATATAB;
                lastNewLine = str.length();
        }
                
        if (! (lastPoint==line.from))
        {
                str += "M "+ FloatToStr(line.from.x)+ " "+FloatToStr(line.from.y);
                strTest += "M "+ FloatToStr(line.from.x)+ " "+FloatToStr(line.from.y);
        }

        if (str!="") str+=" ";

        // Curved Line
        if (line.isCurved)
        {
                str += "Q "+FloatToStr(line.control.x)+ " "+FloatToStr(line.control.y)+" "+FloatToStr(line.to.x)+ " "+FloatToStr(line.to.y);
//                              strTest += " L "+FloatToStr(line.to.x)+ " "+FloatToStr(line.to.y);
                strTest += " L "+FloatToStr(line.control.x)+ " "+FloatToStr(line.control.y);
        }
        else
        // Vertical Line
        if (CompareFP(line.from.x,line.to.x))
                str += "V " + FloatToStr(line.to.y);
        else
        // Horizontal Line
        if (CompareFP(line.from.y,line.to.y))
                str += "H " + FloatToStr(line.to.x);
        // General Line
        else
                str += "L " + FloatToStr(line.to.x)+ " "+FloatToStr(line.to.y);
                
        lastPoint = line.to;
        
}

// Close the path if the path is "closed" in terms of flash spec
if (lastPoint==firstPoint)
{
        str += "z";
}

return str;
        
}
//----------------------------------------------------------------------------------------
/* RGB Test:
                         RGB rgb1(10,10,10);
                         RGB rgb2(20,20,20);
                         RGB rgb3(15,20,20);
                         RGB rgb4(10,20,20);
                         RGB rgb5(10,20,20);
                                                 
                                                 cout << endl;
                         cout << "RGB1: " << rgb1 << endl;
                         cout << "RGB2: " << rgb2 << endl;
                         cout << "RGB3: " << rgb3 << endl << endl;
                         cout << "RGB4: " << rgb4 << endl << endl;
                         cout << "RGB5: " << rgb5 << endl << endl;
                           
                         cout << "RGB1<RGB2: " << (rgb1<rgb2) << endl;
                         cout << "RGB2<RGB3: " << (rgb2<rgb3) << endl;
                         cout << "RGB1<RGB3: " << (rgb1<rgb3) << endl;
                         cout << "RGB1==RGB4: " << (rgb1==rgb4) << endl;
                         cout << "RGB5==RGB4: " << (rgb5==rgb4) << endl;
*/
//----------------------------------------------------------------------------------------
