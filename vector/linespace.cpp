//----------------------------------------------------------------------------------------
// Copyright: See definitions.h for License
//----------------------------------------------------------------------------------------
#include "linespace.h"
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
LinePiece::LinePiece()
{
        lineList.clear();
}
//----------------------------------------------------------------------------------------
LinePiece::LinePiece(const LineList &lineList)
{
        SetLineList(lineList);
}
//----------------------------------------------------------------------------------------
bool LinePiece::SetLineList(const LineList &lineListParam)
{
        lineList = lineListParam;
        SetupBeginPoint();
        SetupEndPoint();
}
//----------------------------------------------------------------------------------------
LineList LinePiece::GetLineList()
{
        return lineList;
}
//----------------------------------------------------------------------------------------
bool LinePiece::Attach(const LinePiece &)
{

}
//----------------------------------------------------------------------------------------
bool LinePiece::SetupBeginPoint()
{
        Point firstPoint = Point(-1,-1);
        
        if (! lineList.empty())
        {
                LineList::const_iterator k = lineList.begin();
                Line firstLine = *k;
                firstPoint = firstLine.from;
        }
        
        beginPoint = firstPoint;
                
        return true;
}
//----------------------------------------------------------------------------------------
bool LinePiece::SetupEndPoint()
{
        Point lastPoint = Point(-1,-1);
        
        if (! lineList.empty())
        {
                LineList::const_iterator k = lineList.end();
                k--;
                Line lastLine = *k;
                lastPoint = lastLine.to;
        }

        endPoint = lastPoint;
                
        return true;
}
//----------------------------------------------------------------------------------------
Point LinePiece::GetBeginPoint()
{
        return beginPoint;
}
//----------------------------------------------------------------------------------------
Point LinePiece::GetEndPoint()
{
        return endPoint;
}
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
LineSpace::LineSpace()
{

}
//----------------------------------------------------------------------------------------
LineSpace::LineSpace(const LineList &originalLineListParam)
{
        SetLineList(originalLineListParam);
}
//----------------------------------------------------------------------------------------
bool LineSpace::SetLineList(const LineList &originalLineListParam)
{
        linePieceList.clear();
        originalLineList = originalLineListParam;
        return true;
}
//----------------------------------------------------------------------------------------
LineList LineSpace::GetLineList() const
{
        return originalLineList;
}
//----------------------------------------------------------------------------------------
string LineSpace::GetOptimizedPathData()
{
        return optimizedPathData;
}
//----------------------------------------------------------------------------------------
bool LineSpace::Optimize()
{
        static bool detailsFirstTime = true;
        if (LNS_VERBOSE) cout << endl << "$$$>>> Optimization started ..." << endl << endl;
//      originalLineList.clear();
//      Line line = Line(1,1,2,2);
//      originalLineList.push_back(line);


        LineList::iterator i=originalLineList.begin();
        Line lastLine;
        LineList currentLineList;
        
        // Initializing the lastLine so that the first line is continuous
        if (! originalLineList.empty())
        {
                lastLine.to = i->from;
        }

        while (i != originalLineList.end())
        {
                Line line = *i;

                // Still continuous
                if (lastLine.to==line.from)
                {
//                      if (LNS_VERBOSE) cout << "[Continue] " << endl;
                        currentLineList.push_back(line);
                } else
                // Continuity broken
                {
                        linePieceList.push_back(currentLineList);
                        currentLineList.clear();
                        currentLineList.push_back(line);
                }

                lastLine = line;
                i++;
        }
        
        if (! currentLineList.empty())
                linePieceList.push_back(currentLineList);
                
        // Dedicated for dumping optimized paths only for debugging
        ofstream outFile;
        
        if (detailsFirstTime)
        {
                outFile.open("details.svg", ios::out);;
                
                outFile << "<?xml version=\"1.0\" standalone=\"no\"?>" << endl;
                outFile << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\"" << endl;
                outFile << "                 \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">" << endl;
                
                outFile << "" << endl;
                outFile << "<svg version=\"1.1\"";
                outFile << endl << "     xmlns=\"http://www.w3.org/2000/svg\"" << endl;
                outFile         << "     xmlns:xlink=\"http://www.w3.org/1999/xlink\">" << endl;
        }

        LinePieceList::iterator i1=linePieceList.begin();
        while (i1!=linePieceList.end())
        {
                LinePiece linePiece1 = *i1;
                LinePieceList::iterator i2=i1;
                bool noAttachments = true;

                i2++;
                while (i2!=linePieceList.end())
                {
                        LinePiece linePiece2 = *i2;
                        Point viaPoint1 = linePiece1.GetEndPoint();
                        Point viaPoint2 = linePiece2.GetBeginPoint();
                        
                        // In case a line piece is continuing another one
                        if (viaPoint1==viaPoint2)
                        {
                                Point firstPoint = linePiece1.GetBeginPoint();
                                Point lastPoint = linePiece2.GetEndPoint();
                                
                                if (LNS_VERBOSE) cout << "%%% >>> Joint line pieces found from: ";
                                if (LNS_VERBOSE) cout << firstPoint << " via: ";
                                if (LNS_VERBOSE) cout << viaPoint1 << " to: ";
                                if (LNS_VERBOSE) cout << lastPoint << " (end)" << endl;
                                
                                LineList ll1 = linePiece1.GetLineList();
                                LineList ll2 = linePiece2.GetLineList();
                                ll1.splice(ll1.end(),ll2);
                                string str=ProducePathDataBasic(ll1);
                                
                                if (detailsFirstTime)
                                {
                                        /*
                                        outFile << "<circle  cx=\"" << firstPoint.x <<"\" cy=\""<<firstPoint.y<<"\" r=\"6\" fill=\"blue\"/>"<<endl;
                                        outFile << "<path d=\"";
                                        outFile << str <<"\"";
                                        outFile << " fill=\"#000000\" stroke-width=\"1\" stroke=\"black\" fill-opacity=\"0.0\" ";
                                        outFile << "/>" << endl;
                                        outFile << "<circle  cx=\"" << lastPoint.x <<"\" cy=\""<<lastPoint.y<<"\" r=\"4\" fill=\"red\"/>"<<endl;
                                        outFile << "<circle  cx=\"" << viaPoint1.x <<"\" cy=\""<<viaPoint1.y<<"\" r=\"2\" fill=\"green\"/>"<<endl;
                                        outFile << endl;
                                        */
                                }
                                
                                //*i2 = ll1;
                                i1 = linePieceList.erase(i1);
                                i1 = linePieceList.insert(i1,ll1);
                                i2 = linePieceList.erase(i2);
                                
                                noAttachments = false;
                                
                                linePiece1 = ll1;
                        } else
                                i2++;
                }

                if (noAttachments) i1++;

        }

        optimizedPathData = "";
        
        // Writing all the details of the line pieces
        LinePieceList::iterator j=linePieceList.begin();
        int count = 0;
        for (;j!=linePieceList.end();j++)
        {
                LinePiece linePiece = *j;
                LineList lineList = linePiece.GetLineList();
                string str = ProducePathDataBasic(lineList);

                Point firstPoint = linePiece.GetBeginPoint();
                Point lastPoint = linePiece.GetEndPoint();
                

                if (LNS_VERBOSE) cout << "piece["<<count<<"] : "<<endl<< "<path d=\""<<str<<"\"/>" << endl;
                
                if (optimizedPathData!="")
                {
                        optimizedPathData += "\n";
                        optimizedPathData += GL_PATHDATATAB;
                        optimizedPathData += " ";
                }
                
                optimizedPathData += str;
                
                if (detailsFirstTime)
                {
                        /*
                        outFile << "<circle  cx=\"" << firstPoint.x <<"\" cy=\""<<firstPoint.y<<"\" r=\"5\" fill=\"blue\"/>"<<endl;
                        outFile << "<path d=\"";
                        outFile << str <<"\"";
                        outFile << " fill=\"#000000\" stroke-width=\"1\" stroke=\"black\" fill-opacity=\"0.0\" ";
                        outFile << "/>" << endl;
                        outFile << "<circle  cx=\"" << lastPoint.x <<"\" cy=\""<<lastPoint.y<<"\" r=\"2\" fill=\"red\"/>"<<endl;
                        outFile << endl;
                        */
                }
                
                count++;
        }
        
                if (detailsFirstTime)
                {
                        outFile << "<path d=\"";
                        outFile << optimizedPathData <<"\"";
                        outFile << " fill=\"#000000\" stroke-width=\"1\" stroke=\"black\" fill-opacity=\"1.0\" ";
                        outFile << "/>" << endl;
                        outFile << endl;
                }
        if (LNS_VERBOSE) cout << "<!-- Path Count:" << count <<" -->" << endl;
        outFile << "<!-- Path Count:" << count <<" -->" << endl;
//*/
        outFile << "</svg>";
        
        detailsFirstTime = false;
        
        if (LNS_VERBOSE) cout << endl << "$$$>>> Optimization finished" << endl << endl;
        return true;
}
//----------------------------------------------------------------------------------------
