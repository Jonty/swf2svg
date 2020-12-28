//----------------------------------------------------------------------------------------
// Copyright: See definitions.h for License
//----------------------------------------------------------------------------------------
#include "../definitions.h"
//----------------------------------------------------------------------------------------
#ifndef __ANALYZER__
#define __ANALYZER__
//----------------------------------------------------------------------------------------
#define AN_EXCEPTION_ERROR              100                                     
#define AN_EXCEPTION_HINT               101                                     
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
class AnalyzerException
{
        public:
            AnalyzerException();
            AnalyzerException(const string &, const int &, const int &);
            
                string ErrorMessage() const;
                int ReturnCode() const;
                int Type() const;
                
        private:
                string errorMsg;
                int returnCode;
                int type;
                
};
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
class Analyzer
{
        public:
                Analyzer();
                
                string GetInputFileName();
                string GetOutputFileName();
                int GetRequestedAction() const;
                bool GetLoggingEnabled() const;
                bool GetVerboseMode() const;
                bool GetIndicatorEnabled() const;
                
                bool ParseCommandLineArguments(const int &, char *[]);
                
                void DealWithOption(string);

        private:
                string inputFileName;
                string outputFileName;
                string lastErrorMsg;

                int requestedAction;   // Defines the various requests possible to be made by user (e.g. only decompress, only paths)
                bool loggingEnabled;   // Indicates if logging events is enabled
                bool verboseEnabled;   // Indicates if logging events is enabled
                bool indicatorEnabled; // Specifies whether a "dot" progress indicator should be displayed or not
};        
//----------------------------------------------------------------------------------------
#endif // __ANALYZER__
//----------------------------------------------------------------------------------------
