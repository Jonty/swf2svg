//----------------------------------------------------------------------------------------
// Copyright: See definitions.h for License
//----------------------------------------------------------------------------------------
#include "shapespace.h"
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
SubSpaceInfo::SubSpaceInfo()
{
        multipleLineStyles = false;
        index = -1;
}
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
ShapeSpace::ShapeSpace()
{
        spaceCount = 0;
}
//----------------------------------------------------------------------------------------
int ShapeSpace::FindSpaceIndex(const FillStyle &fillParam)
{
        RGB fillRGB = fillParam.colour;
        SpaceFills::const_iterator iter = spaceFills.find(fillRGB);
        int index = -1;
        bool notFound = true;
        
        while (iter!=spaceFills.end() && (iter->first.Red()==fillRGB.Red()) && notFound)
        {
                if (iter->first==fillRGB)
                {
                   SubSpaceInfo info = iter->second;
                   if (info.fillStyle==fillParam)
                   {
                                index = info.index;
                                notFound = false;
                   }
                }
                iter++;
        }
        
//      if (SHS_VERBOSE) cout << "$$ fill: " << fill << endl;
//      if (SHS_VERBOSE) cout << "$$ index: " << index << endl;
        
        return index;
}
//----------------------------------------------------------------------------------------
int ShapeSpace::AddSubSpace(SubSpaceInfo info)
{
        RGB fillRGB = info.fillStyle.colour;
        if (SHS_VERBOSE) if (SHS_VERBOSE) cout << "[N]" << spaceCount;
        info.index = spaceCount;
        spaceFills.insert(SpaceFills::value_type(fillRGB,info));
        SubSpace sub;

        sub.info = info;
        subSpace.push_back(sub);
        
//      if (SHS_VERBOSE) cout << "Fill RGB: " << fill << " inserted as index: " << spaceCount << endl;
        return spaceCount++;
}
//----------------------------------------------------------------------------------------
bool ShapeSpace::AddLine(Line lineParam, const FillStyle &fillStyle)
{
        // Translating lines to point trans
        /*
        lineParam.from.x += 30;
        lineParam.from.y += 30;
        lineParam.to.x += 30;
        lineParam.to.y += 30;
        */
//      if (SHS_VERBOSE) cout << "ADD LINE:"<<endl;     
/*      
        RGB fillRGB = RGB(0,0,0);
        if (fillStyle.valid)
                fillRGB = fillStyle.colour;
*/
        int spaceIndex = FindSpaceIndex(fillStyle);

        // The case that the fill colour is new so we need to create a new space
        if (spaceIndex==-1)
        {
                SubSpaceInfo info;
                info.lineStyle = lineParam.lineStyle;
                info.multipleLineStyles = false;
                info.fillStyle = fillStyle;
                if (SHS_VERBOSE) cout << "<< F" << info.fillStyle.valid << " >>";
                spaceIndex = AddSubSpace(info);
        } else
        // Already have the subspace but the line style differs
        if (! (subSpace[spaceIndex].info.lineStyle == lineParam.lineStyle))
        {
                if (SHS_VERBOSE) cout << " [[[[[ MULTIPLE LINE STYLES IN ONE SUBSPACE ]]]]] " << endl;
                if (SHS_VERBOSE) cout << "            LineStyle already in space["<< spaceIndex<<"]: ";
                if (SHS_VERBOSE) cout << subSpace[spaceIndex].info.lineStyle << endl;
                if (SHS_VERBOSE) cout << "            Line to be added has: ";
                if (SHS_VERBOSE) cout << lineParam.lineStyle << endl; 
                subSpace[spaceIndex].info.multipleLineStyles = true;
        }
                
        if (SHS_VERBOSE) cout << "Adding: " << lineParam<< " to space["<<  spaceIndex << "]"<< endl;
        
//      subSpace[spaceIndex].lineList.push_back(Line(1,1,1,1));
        
        LineList *lineList = &subSpace[spaceIndex].lineList;
        
        LineList::iterator i=lineList->begin();
        bool lineAttached = false;

        while (i != lineList->end() && !lineAttached)
        {
                Line line = *i;
                // lineParam directing at line
                if (lineParam.to==line.from)
                {
                        if (SHS_VERBOSE) cout << "lineParam directing at line" << endl;
                        lineList->insert(i,lineParam);
                        lineAttached = true;
                } else
                // lineParam is continuing line
                if (lineParam.from==line.to)
                {
                        if (SHS_VERBOSE) cout << "lineParam is continuing line" << endl;
                        LineList::iterator j = i;
                        j++;
                        lineList->insert(j,lineParam);
                        lineAttached = true;
                }
                        
                i++;
        }
        
        if (!lineAttached)
        {
                lineList->push_back(lineParam);
        }
        
//      if (SHS_VERBOSE) cout << "space["<<spaceIndex<<"].size: " << space[spaceIndex].size() << endl;
        return true;
}
//----------------------------------------------------------------------------------------
string ShapeSpace::ProducePathDataOptimized(const LineList &lineListParam) const
{
        LineList sp = lineListParam;
        //return ProducePathDataBasic(sp,tab);
        return GetOptimizedPathData(sp);
}
//----------------------------------------------------------------------------------------
void ShapeSpace::PrintLines() const
{
        for(SpaceFills::const_iterator i = spaceFills.begin(); i!= spaceFills.end();i++)
        {
                int spaceIndex = i->second.index;
                LineList sp = subSpace[spaceIndex].lineList;
                if (SHS_VERBOSE) cout << "Fill: " << i->first << " #" << spaceIndex << endl;
                for (LineList::iterator j=sp.begin();j!=sp.end();j++)
                {
                        Line line = *j;
                        if (SHS_VERBOSE) cout << "              > " << line << endl;
                }
        }
        
}
//----------------------------------------------------------------------------------------
/*
bool ShapeSpace::TrickyLineList(const LineList &lineList, FillStyle &trickyFillParam) const
{
bool tricky=true;
bool firstLine=true;
FillStyle trickyFillStyle;

for (LineList::const_iterator j=lineList.begin();(j!=lineList.end())&&tricky;j++)
{
        Line line = *j;
        
//      if (SHS_VERBOSE) cout << endl << "<< Tricked Fill: " << line.fillStyle0 << endl;
        if (firstLine)
        {
                trickyFillStyle = line.fillStyle0;
                firstLine = false;
                tricky = trickyFillStyle.valid;
        } else
                tricky = (trickyFillStyle==line.fillStyle0);
}

if (tricky) trickyFillParam = trickyFillStyle;

return tricky;
}
//*/
//----------------------------------------------------------------------------------------
string ShapeSpace::GetOptimizedPathData(const LineList &lineList) const
{
        if (SHS_VERBOSE) cout << "$>> Optimizing LineList ..." << endl;
//      lineList.empty();
        LineSpace lp(lineList);
        lp.Optimize();
        return lp.GetOptimizedPathData();
}
//----------------------------------------------------------------------------------------
void ShapeSpace::FlushPaths(PathVector &pathVector) const
{
        /*
//      Sorting all the sub spaces acording to their info.index
//  to correctly put them on top of each other
        SubSpaceVector subSpVector;
        for(SpaceFills::const_iterator i = spaceFills.begin(); i!= spaceFills.end();i++)
        {
                int spaceIndex = i->second.index;
                SubSpace sp = subSpace[spaceIndex];
        }
        */
        
        for(SubSpaceVector::const_iterator i = subSpace.begin(); i!= subSpace.end();i++)
        {
                SubSpace sp = *i;
//              if (SHS_VERBOSE) cout << "Fill: " << i->first << " #" << spaceIndex << endl;
                
//              RGB fillRGB = i->first;
                FillStyle fillStyle = sp.info.fillStyle;
                LineStyle lineStyle = sp.info.lineStyle;
        
                if (sp.info.multipleLineStyles)
                {
                        if (GL_DEBUG_MODE) if (SHS_VERBOSE) cout << endl << "Multiple linestyles detected in one sub space[<<spaceIndex<<] at fill style >> " << fillStyle << endl;
                        
                        typedef multimap< RGB, int, less <RGB> > RGBIntMap;
                        RGBIntMap lineStyleIndex;
                        
                        // onlyFill: a subspace dedicated to draw only the fill proportion without any line strokes
                        SubSpace onlyFill;
                        onlyFill.info.fillStyle = fillStyle;

                        SubSpaceVector subs;

                        for (LineList::const_iterator j=sp.lineList.begin();j!=sp.lineList.end();j++)
                        {
                                Line line = *j;
                                
                                // Checking if we have an appropriate subspace for this lineStyle
                                RGBIntMap::const_iterator iter = lineStyleIndex.find(line.lineStyle.colour);
                                int index = -1;
                                
                                bool notFound=true;
                                while (iter!=lineStyleIndex.end() && (iter->first.Red()==line.lineStyle.colour.Red()) && notFound)
                                {
                                        if (subs[iter->second].info.lineStyle==line.lineStyle)
                                        {
                                           index = iter->second;
                                           notFound = false;
                                        }
                                        iter++;
                                }                               

//                              int index = lineStyleIndex[line.lineStyle.colour];
//                              if (SHS_VERBOSE) cout << "index: " << index << endl;
                                
                                // If we haven't added a subspace for the linestyle
                                if (index==-1)
                                {
                                        SubSpace tempSP;
                                        tempSP.info.fillStyle = fillStyle;
                                        tempSP.info.lineStyle = line.lineStyle;
                                        tempSP.info.multipleLineStyles = false;
                                        subs.push_back(tempSP);
                                        index = subs.size()-1;
                                        
                                        // Adding the index to multimap for future refrence                                     
                                        lineStyleIndex.insert(RGBIntMap::value_type(line.lineStyle.colour,index));
                                        
                                        if (SHS_VERBOSE) cout << "sub[" << index << "] created for lineStyle:  " << tempSP.info.lineStyle << endl;
                                }
                                
                                subs[index].lineList.push_back(line);
                                onlyFill.lineList.push_back(line);
//                              if (SHS_VERBOSE) cout << "Index: " << index << endl;
                        }
                        
                        if (SHS_VERBOSE) cout << "Generating each new SubSpace pathData..." << endl;                    

                        // First Adding the "fill" only to the path vector 
                        string pathData = ProducePathDataOptimized(onlyFill.lineList);
                        Path path;
                        path.fillStyle = onlyFill.info.fillStyle;
                        path.lineStyle = onlyFill.info.lineStyle;
                        path.pathData = pathData;
        
                        if (SHS_VERBOSE) cout << "path[" << pathVector.size() << "] = " << path << "  ( $$ Fill only $$ )" << endl;
                        pathVector.push_back(path);

                        for (SubSpaceVector::const_iterator j=subs.begin();j!=subs.end();j++)
                        {
                                SubSpace sub = *j;


                                string pathData = ProducePathDataOptimized(sub.lineList);
                                Path path;
//                              path.fillStyle = sub.info.fillStyle;
/*
                                // Disabling "TrickyLineList" detection system
                                // Find out if the path is "tricky"
                                // FillStyle trickyFill;
                                // bool trickyPath = TrickyLineList(sub.lineList, trickyFill);
                                if (trickyPath)
                                {
                                        path.fillStyle = trickyFill;
                                } else
                                        path.fillStyle.valid = false;
//*/                    
                                // Always without fill style since we have defined the fill bit ealier  
                                path.fillStyle.valid = false;
                                path.lineStyle = sub.info.lineStyle;
                                path.pathData = pathData;

                                if (SHS_VERBOSE) cout << "path[" << pathVector.size() << "] = " << path;

                            //if (SHS_VERBOSE) cout << " >> tricky: " <<  trickyPath << "  Fill: " << trickyFill << endl;
                                
                                pathVector.push_back(path);
                        }
                        
                } // end if (sp.info.multipleLineStyles)
                else
                {
                        if (GL_DEBUG_MODE) if (SHS_VERBOSE) cout << endl << "Single linestyle sub space[<<spaceIndex<<] at fill style >> " << fillStyle << endl;
                                
                        string pathData = ProducePathDataOptimized(sp.lineList);

                        Path path;
                        path.fillStyle = fillStyle;
                        path.lineStyle = lineStyle;
                        path.pathData = pathData;

                        if (SHS_VERBOSE) cout << "path[" << pathVector.size() << "] = " << path << endl;
        
                        pathVector.push_back(path);
                }
/*
                path.fillStyle.valid = hasFillStyle;
                path.fillStyle.colour = fillRGB;
                path.lineStyle = lineStyle;
                path.pathData = strTest;

                pathVector.push_back(path); */

        }
}
//----------------------------------------------------------------------------------------
ostream &operator<<(ostream &output, const ShapeSpace &space)
{
        space.PrintLines();
        return output;
}
//----------------------------------------------------------------------------------------
/* ShapeSpace Test
                         if (SHS_VERBOSE) cout << endl;
                         if (SHS_VERBOSE) cout << endl;
                         
                         ShapeSpace *space = new ShapeSpace();
                         Line line1 = Line(20,20,15,15);
                         Line line2 = Line(10,10,20,20);
                         Line line3 = Line(15,15,10,10);

                         RGB fill1 = RGB(10,10,10);
                         RGB fill2 = RGB(15,10,10);
                         
                         space->AddLine(line3,fill1);
                         space->AddLine(line2,fill1);
                         space->AddLine(line1,fill1);
                         
                         space->PrintLines();
*/
//----------------------------------------------------------------------------------------
/*  Multimap test

        RGB fill1 = RGB(10,10,9);
        RGB fill2 = RGB(10,10,10);
        RGB fill3 = RGB(10,9,9);
        
        spaceFills.insert(SpaceFills::value_type(fill1,50));
        spaceFills.insert(SpaceFills::value_type(fill2,60));

        int index1=0,index2=0;
        
        index1 = FindSpaceIndex(fill1);
        index2 = FindSpaceIndex(fill2);
        int index3 = FindSpaceIndex(fill3);


        if (SHS_VERBOSE) cout << "spaceFills[" << fill1 << "] = " <<  index1 << endl;
        if (SHS_VERBOSE) cout << "spaceFills[" << fill2 << "] = " <<  index2 << endl;
        if (SHS_VERBOSE) cout << "spaceFills[" << fill3 << "] = " <<  index3 << endl;

*/      
//----------------------------------------------------------------------------------------
