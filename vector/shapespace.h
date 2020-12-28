//----------------------------------------------------------------------------------------
// Copyright: See definitions.h for License
//----------------------------------------------------------------------------------------
#include "../definitions.h"
#include "shapebasics.h"
#include "linespace.h"
//----------------------------------------------------------------------------------------
#ifndef __SHAPESPACE__
#define __SHAPESPACE__
//----------------------------------------------------------------------------------------
#define SHS_VERBOSE false
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
class SubSpaceInfo
{
        public:
                SubSpaceInfo(); // Constructor
                int index;
                bool multipleLineStyles;
                LineStyle lineStyle;            
                FillStyle fillStyle;
};
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
class SubSpace
{
        public:
                LineList lineList;
                SubSpaceInfo info;
};
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
typedef vector<SubSpace> SubSpaceVector;
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
class ShapeSpace
{
        friend ostream &operator<<(ostream &, const ShapeSpace &);

        public:
                ShapeSpace();
                bool AddLine(Line, const FillStyle &);
                void FlushPaths(PathVector &) const;
                void PrintLines() const;
                
        private:
                int spaceCount;
                int FindSpaceIndex(const FillStyle &);
                int AddSubSpace(SubSpaceInfo);
                string ProducePathDataOptimized(const LineList &) const;
//              bool TrickyLineList(const LineList &, FillStyle &) const;
                string GetOptimizedPathData(const LineList &) const;
                
                typedef  multimap< RGB, SubSpaceInfo, less<RGB> > SpaceFills;
                SpaceFills spaceFills;
//              LineList space[GL_MAXSPACENUM];
                SubSpaceVector subSpace;
                
        
};
//----------------------------------------------------------------------------------------
#endif /*SHAPESPACE_*/
//----------------------------------------------------------------------------------------
