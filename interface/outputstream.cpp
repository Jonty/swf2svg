//----------------------------------------------------------------------------------------
// Copyright: See definitions.h for License
//----------------------------------------------------------------------------------------
#include "outputstream.h"
//----------------------------------------------------------------------------------------
OutputStream::OutputStream()
{
/*     if (GL_LOGFILE_ENABLED)
     {
        string logFileName = ((string) AZ_ROOT + "/log.txt");
        logFile.open(logFileName.c_str(), ios::out);
        cout << "Logging at " << logFileName << endl;
     }*/
loggingEnabled = false;
}
//----------------------------------------------------------------------------------------
OutputStream& OutputStream::operator<<(const string &str)
{
    cout << str;
    if (loggingEnabled)
        logFile << str;
        return *this;
}
//----------------------------------------------------------------------------------------
OutputStream& operator<<(OutputStream baseOut, OutputStream &commingOut)
{
        return baseOut;
}
//----------------------------------------------------------------------------------------
OutputStream& endln(OutputStream &out)
{
        return (out << "\n");
}
//----------------------------------------------------------------------------------------
