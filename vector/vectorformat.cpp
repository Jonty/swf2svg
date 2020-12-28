//----------------------------------------------------------------------------------------
// Copyright: See definitions.h for License
//----------------------------------------------------------------------------------------
#include "vectorformat.h"
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
VectorFormat::VectorFormat()
{
 SetAsInput(false);
}
//----------------------------------------------------------------------------------------
VectorFormat::~VectorFormat()                          // Destructor
{
}
//----------------------------------------------------------------------------------------
bool VectorFormat::IncommingEventHandler(const int & msg, const string & strParam, int * intParam, float * floatParam, void * untypedPtr)
{
     
}
//----------------------------------------------------------------------------------------
bool VectorFormat::SetRequestAnalyzer(Analyzer *requestAnalyzerParam)
{
         requestAnalyzer = requestAnalyzerParam;
         
         loggingEnabled = requestAnalyzer->GetLoggingEnabled();
         
         return true;
}
//----------------------------------------------------------------------------------------
void VectorFormat::SetIOFileName(const string & fileName)
{
     ioFileName = fileName;
}
//----------------------------------------------------------------------------------------
string VectorFormat::GetIOFileName()
{
     return ioFileName;
}
//----------------------------------------------------------------------------------------
void VectorFormat::SetAsInput(const bool & nature) // True for input, False for output
{
     isInput = nature;
}
//----------------------------------------------------------------------------------------
bool VectorFormat::IsInput()
{
     return isInput;
}
//----------------------------------------------------------------------------------------
void VectorFormat::SetOutgoingObject(VectorFormat * vectorFormat)
{
     outgoingFormat = vectorFormat;
}
//----------------------------------------------------------------------------------------
void VectorFormat::DoConversion() // Should be called for the "input" object
{
if (IsInput())
  {
     Parse();
     outgoingFormat->FlushOutput();
  }
}
//----------------------------------------------------------------------------------------
void VectorFormat::TriggerOutputEvent(const int & msg, const string & strParam, int * intParam, float * floatParam, void * untypedPtr)
{
//     cout << "        (Triggering #" << msg << ") "<<  endl;
     outgoingFormat->IncommingEventHandler(msg,strParam,intParam,floatParam,untypedPtr);
}
//----------------------------------------------------------------------------------------
void VectorFormat::Parse()
{
 cout << "Error: Un-typed parser" << endl;
}
//----------------------------------------------------------------------------------------
void VectorFormat::FlushOutput()
{
 cout << "Error: Un-typed flusher" << endl;
}
//----------------------------------------------------------------------------------------
