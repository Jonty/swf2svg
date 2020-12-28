//----------------------------------------------------------------------------------------
// Copyright: See definitions.h for License
//----------------------------------------------------------------------------------------
#include "analyzer.h"
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
AnalyzerException::AnalyzerException()
{
}           
//----------------------------------------------------------------------------------------
AnalyzerException::AnalyzerException(const string &strParam, const int &intParamCode, const int &intParamType)
{
        errorMsg = strParam;
        returnCode = intParamCode;
        type = intParamType;
}
//----------------------------------------------------------------------------------------
int AnalyzerException::Type() const
{
        return type;
}
//----------------------------------------------------------------------------------------
int AnalyzerException::ReturnCode() const
{
        int code=returnCode;
        
        if (type==AN_EXCEPTION_HINT)
                code = 0;
                
        return code;
}
//----------------------------------------------------------------------------------------
string AnalyzerException::ErrorMessage() const
{
        string errorStr="";
        
        if (type==AN_EXCEPTION_ERROR)
                errorStr += "Error Occured: ";
                
        errorStr += errorMsg;
        
        return errorStr;
}
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
Analyzer::Analyzer()
{
        // Setting up default options
        
        requestedAction = RQ_MODE_CONVERT;
        loggingEnabled = false;
        verboseEnabled = false;
        indicatorEnabled = true;
}
//----------------------------------------------------------------------------------------
string Analyzer::GetInputFileName()
{
        return inputFileName;
}
//----------------------------------------------------------------------------------------
string Analyzer::GetOutputFileName()
{
        return outputFileName;
}
//----------------------------------------------------------------------------------------
bool Analyzer::ParseCommandLineArguments(const int &argc, char *argv[])
{
        if (argc<2)
        {
                string errorMessage="No input file was specified";

                throw AnalyzerException(errorMessage,ERR_NOERROR,AN_EXCEPTION_HINT);

                return false;
        } else
        {
                string param1 = argv[1];
                
                if ((param1=="-h") || (param1=="-help") || (param1=="--help"))
                {
                        string appName = argv[0];
                        string newLine = "\n\t\t";
                        string consoleMesssage = "";
                        consoleMesssage += "Usage:\n\t"+appName+" <inputfile> [<outputfile>] [option1] [option2] ...\n\n";
                        consoleMesssage += "\tOptions:\n";
                        consoleMesssage += newLine+"--log: Enable logging details of the conversion in \"log.txt\"";
                        consoleMesssage += newLine+"--nodots: Disables progression dots to be displayed";
                        consoleMesssage += newLine+"--verbose: Shows details of the conversion on the console";
                        consoleMesssage += newLine+"--onlypaths, --genpaths:";
                        consoleMesssage += newLine+"             Generate only paths of the flash file with no";
                        consoleMesssage += newLine+"             fill colours (for demonstration only)";
                        consoleMesssage += newLine+"--decompress:";
                        consoleMesssage += newLine+"             Decompresses the flash file (if compressed) in";
                        consoleMesssage += newLine+"             \"decompress.swf\" with no other conversion";
                        throw AnalyzerException(consoleMesssage,ERR_NOERROR,AN_EXCEPTION_HINT);
                }
                else
                {
                        string inputArg = argv[1];
                        if (inputArg.find("swf")!=string::npos)
                        {
                                string errorMessage="License agreement for Macromedia Shockwave ";
                                errorMessage+="Flash specification prevents reading of files with '.swf' on their file name. ";
                                errorMessage+="Please change the file name so that it does not include any 'swf' on it and try again";
        
                                throw AnalyzerException(errorMessage,ERR_SWFISSUE,AN_EXCEPTION_ERROR);
        
                                return false;
                        }
                        
                ifstream inFile;
        
                inFile.open(inputArg.c_str(), ios::in);
                
                    if (! inFile)
                    {
                                throw AnalyzerException("File \""+inputArg+"\" does not exist",ERR_FILENOTEXIST,AN_EXCEPTION_ERROR);
        
                        return false;
                    }
                     
                        inputFileName = inputArg;
                        
                        int lastDot = inputArg.rfind('.');
                        
                        outputFileName = inputArg.substr(0,lastDot)+".svg";
                        
                        for (int j=2;j<argc;j++)
                        {
                                //cout << "arg["<<j<<"] : "<<argv[j]<<endl;
                                string param = argv[j];
                                DealWithOption(param);
                        }
                }
        } // end else (argc<2)
/*      
        if (GL_DEBUG_MODE)
                requestedAction = RQ_MODE_GENERATEPATHS;
//*/
        return true;
}
//----------------------------------------------------------------------------------------
void Analyzer::DealWithOption(string param)
{
        if ((param=="--onlypaths")||(param=="--genpaths"))
        {
                requestedAction = RQ_MODE_GENERATEPATHS;
                cout << "<OnlyPaths> option enabled: Only paths will be generated" << endl;
        }
        else
        if (param=="--decompress")
        {
                requestedAction = RQ_MODE_GENERATEPATHS;
                cout << "<OnlyDecompress> option enabled: Decompresses the flash file with no conversion taking place" << endl;
        }
        else
        if (param=="--nodots")
        {
                indicatorEnabled = false;
                //cout << "<NoDots> option enabled: No progression dots will be displayed" << endl;
        }
        else
        if (param=="--verbose")
        {
                verboseEnabled = true;
                cout << "<Verbose> option enabled: Details of the conversion will be displayed on the console" << endl;
        }
        else
        if (param=="--log")
        {
                loggingEnabled = true;
                cout << "<Log> option enabled: Full details of the conversion will be logged" << endl;
        }
        else
        {
                cout << "Error: Command option \"" << param << "\" undefined therefore ignored" << endl;
        }
}
//----------------------------------------------------------------------------------------
int Analyzer::GetRequestedAction() const
{
        return requestedAction;
}
//----------------------------------------------------------------------------------------
bool Analyzer::GetLoggingEnabled() const
{
        return loggingEnabled;
}
//----------------------------------------------------------------------------------------
bool Analyzer::GetVerboseMode() const
{
        return verboseEnabled;
}
//----------------------------------------------------------------------------------------
bool Analyzer::GetIndicatorEnabled() const
{
        return indicatorEnabled;
}
//----------------------------------------------------------------------------------------
