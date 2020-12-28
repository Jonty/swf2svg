//----------------------------------------------------------------------------------------
// Copyright: See definitions.h for License
//----------------------------------------------------------------------------------------
#include "../definitions.h"
#include "shape.h"
#include <sstream>
//----------------------------------------------------------------------------------------
#ifndef __VECTORFORMAT__
#define __VECTORFORMAT__
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
class VectorFormat
{
      public:
             VectorFormat();                           // Constructor
             ~VectorFormat();                          // Destructor

             // Handles messages received by this later
             virtual bool IncommingEventHandler(const int &, const string &, int *, float *, void *);
//             void         ParseInput();
             
             bool                 SetRequestAnalyzer(Analyzer*); // Must be called after creating a VectorFormat

             void         SetIOFileName(const string &);
             string       GetIOFileName();
             
             void         SetAsInput(const bool &); 
             bool         IsInput();
             
             void         SetOutgoingObject(VectorFormat *);
             
             void         DoConversion();
             
      protected:
             string ioFileName;  
             bool isInput;       
             bool loggingEnabled;
             
             void TriggerOutputEvent(const int &, const string &, int *, float *, void *);

             virtual void Parse();
             virtual void FlushOutput();
             
             VectorFormat* outgoingFormat;
             
                         Analyzer *requestAnalyzer;
                                     
};
//----------------------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------------------

