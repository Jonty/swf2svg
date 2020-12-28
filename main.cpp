//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
// Copyright: See definitions.h for License
//----------------------------------------------------------------------------------------
#include "definitions.h"
#include "vector/svgformat.h"
#include "vector/flashformat.h"
//----------------------------------------------------------------------------------------
using namespace std;
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
int main(int argc, char * argv[])
{
//      endln.GetType();
    cout << "Welcome to " << AZ_NAME << " version: " << AZ_VER << " (build "<<AZ_LASTBUILDNUM<< ") on " << AZ_LASTBUILDDATE << endl;
/*    
    if ( GL_LOGFILE_ENABLED)
       cout << "Logging event is enabled" << endl;
*/
        // Processing command line parameters to identify input/output file names and other options
        Analyzer analyzer;

        try
        {
        //cout << endl;
                analyzer.ParseCommandLineArguments(argc,argv);
        cout << endl;
        }
        catch (AnalyzerException e)
        {
                cout << endl;
                cout << e.ErrorMessage() << endl;
                cout << endl << "Terminating ..." << endl;
                
                return e.ReturnCode();
        }
        
        string inputFileName = analyzer.GetInputFileName();
        string outputFileName = analyzer.GetOutputFileName();
        
        if (analyzer.GetVerboseMode()) cout << "Flash input file: " << inputFileName << endl;
        if (analyzer.GetVerboseMode()) cout << "SVG output file: " << outputFileName << endl;
        
//    return 0;

if (GL_MEMCHECK && GL_DEBUG_MODE)
{       
        
    cout << " Shape Size: " << sizeof(Shape) << endl;
    cout << " ShapeSpace Size: " << sizeof(ShapeSpace) << endl;
    cout << " Path Size: " << sizeof(Path) <<  endl;
    cout << " FillStyle Size: " << sizeof(FillStyle) <<endl;
    cout << " LineStyle Size: " << sizeof(LineStyle) << endl;
    cout << " Matrix Size: " << sizeof(Matrix) << endl;
    cout << " RGB Size: " << sizeof(RGB) << endl;
    cout << endl;
    cout << " SVGFormat Size: " << sizeof(SVGFormat) << endl;
    cout << " FlashFormat Size: " << sizeof(FlashFormat) << endl;
    
} else
{
        try{
        
           SVGFormat *svg = new SVGFormat();
           FlashFormat *swf = new FlashFormat();

           // Setting up the source of configuration variables: analyzer
           swf->SetRequestAnalyzer(&analyzer);
           svg->SetRequestAnalyzer(&analyzer);

           swf->SetAsInput(true);
           swf->SetIOFileName(inputFileName);
           swf->SetOutgoingObject(svg);
           
           svg->SetIOFileName(outputFileName);
           
           swf->DoConversion();
        
           cout << endl << "Done. " << endl << endl;
        
        }
        catch(char* str)
        {
           cout << "Error: " << str << endl;
        }
        catch(...)
        {
           cout << "Error! " << endl;
        }
}
    // Pausing the system to see the output window
    if (GL_DEBUG_MODE) 
    {
//      system("PAUSE");
    }
    // Successfull termination of the programme
    return EXIT_SUCCESS;
}
