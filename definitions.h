//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
// Copyright License: 
//----------------------------------------------------------------------------------------
#ifndef __DEFINITIONS__
#define __DEFINITIONS__
//----------------------------------------------------------------------------------------
#include "common.h"                      
#include <iostream>          
#include <iomanip>           
#include <string>            
#include <fstream>           
#include <vector>                        
#include <map>                           
#include <list>                          
#include "interface/outputstream.h"
#include "request/analyzer.h"
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
using namespace std;         
//----------------------------------------------------------------------------------------
#define AZ_NAME                 "Azarakhsh"                
#define AZ_VER                  "0.4.0"                    
#define AZ_LASTBUILDNUM         178                                
#define AZ_LASTBUILDDATE        "May 10, 2006"             

#define GL_DEBUG_MODE           true                       
#define GL_MEMCHECK                             false                                      
#define GL_MAXUNCOMRESSED       200000                     
#define GL_MAXMFLTAGSIZE        50000                      
#define GL_MAXSPACENUM                  30                                                 
#define GL_MAXFILLSTYLES        100                        
#define GL_MAXLINESTYLES        100                        
#define GL_MAXFILENAMELEN       100                        
#define GL_MAXSVGHEADER         20                         
#define GL_MAXSVGBODY           400                        
#define GL_MAXSVGFOOTER         20                         
#define GL_FLASHTWIPSPERPX              20                                                 
#define GL_PATHDATAWIDTH                40                                                 
#define GL_PATHDATATAB                  "\t\t\t\t\t"                       
#define GL_NOALPHA                              -1                                                 

#define MFL_TAG_END             0                          
#define MFL_TAG_SHOWFRAME       1                          
#define MFL_TAG_SETBKG          9                          
#define MFL_TAG_PROTECT         24                         
#define MFL_TAG_DEFINESHAPE     2                          
#define MFL_TAG_DEFINESHAPE2    22                         
#define MFL_TAG_DEFINESHAPE3    32                         
#define MFL_TAG_PLACEOBJECT     4                          
#define MFL_TAG_PLACEOBJECT2    26                         
#define MFL_TAG_DEFINEBITS      6                          
#define MFL_TAG_JPEGTABLES      8                          
#define MFL_TAG_DEFINEBITS2     21                         
#define MFL_TAG_DEFINEBITS3     35                         
#define MFL_TAG_DFBITSLOSSLESS  20                         
#define MFL_TAG_DFBITSLOSSLESS2 36                         
#define MFL_TAG_DEFINESPRITE    39                                                 
#define MFL_TAG_DEFINEFONT              10                                                 
#define MFL_TAG_DEFINETEXT              11                                                 
#define MFL_TAG_DEFINEFONTINFO  13                                                 

#define MFL_FILL_SOLID          0x00                       
#define MFL_FILL_LINEAR         0x10                       
#define MFL_FILL_RADIAL         0x12                       
#define MFL_FILL_REPEAT_BMP     0x40                       
#define MFL_FILL_CLIPPED_BMP    0x41                       
#define MFL_FILL_NS_REPEAT_BMP  0x42                       
#define MFL_FILL_NS_CLIPPED_BMP 0x43                       

#define MFL_PLACETAGHASMATRIX   0x1000                                     

#define EV_VALID                100                        
#define EV_FRAME                101                        
#define EV_SETBKG               102                        
#define EV_DEFINESHAPE          103                        
#define EV_PLACEOBJECT          106                        
#define EV_PLACEOBJECT2         107                        
#define EV_DEFINEFONT           108                        
#define EV_DEFINETEXT           109                        
#define EV_END                  300                        

#define RQ_MODE_CONVERT                 100                                                
#define RQ_MODE_DECOMPRESS              101                                                
#define RQ_MODE_GENERATEPATHS   102                                                

#define ERR_NOERROR                             0                                                  
#define ERR_SWFISSUE                    1                                                  
#define ERR_FILENOTEXIST                1                                                  
//----------------------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------------------
