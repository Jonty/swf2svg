//----------------------------------------------------------------------------------------
// Copyright: See definitions.h for License
//----------------------------------------------------------------------------------------
#include "../definitions.h"
#include "shapebasics.h"
#include <list>
#include <map>
//#include <algorithm>
//----------------------------------------------------------------------------------------
#define LNS_VERBOSE false
//----------------------------------------------------------------------------------------
#ifndef __LINESPACE__
#define __LINESPACE__
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
class LinePiece
{
        public:
                LinePiece();
                LinePiece(const LineList &);
                
                bool SetLineList(const LineList &);
                LineList GetLineList();
                Point GetBeginPoint();
                Point GetEndPoint();

                bool Attach(const LinePiece &);
                
        private:
                LineList lineList;
                Point beginPoint, endPoint;

                bool SetupBeginPoint();
                bool SetupEndPoint();
};
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
typedef list<LinePiece> LinePieceList;
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
class LineSpace
{
        public:
                LineSpace();
                LineSpace(const LineList &);
                
                bool SetLineList(const LineList &);
                LineList GetLineList() const;

                bool Optimize();
                string GetOptimizedPathData();
                
        private:
                LineList originalLineList;
                string optimizedPathData;
                LinePieceList linePieceList;
};
//----------------------------------------------------------------------------------------
#endif /*_LINESPACE_*/
