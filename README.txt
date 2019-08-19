EZTools is a C++ lib for a lot of things that I do in weather software.  As a side effect it also works well
for some other internet projects as well.
It's licenced under the BSD Simple licence meaning you can use it however you like but you can't blame me for it.

Main repo can be found at https://github.com/hippymulehead/EZTools
If you find bugs, submit the bug fix to there or simply fork the project.

Requirements...

poco-foundation >= 1.6.1
poco-crypto >= 1.6.1
jsoncpp >= 0.10.5
curl >= 7.29.0

Compiling...

cmake .
// On Fedora or Centos
make clean && make package
// Other linux distros
make clean && make install

If there's no description here, it's not really ready yet or its a basic struct or enum.

EZTools.h
    namespace EZCrypto
        makeRSAKeys - Simple method to make RSA Keys
    namespace EZRandom
        EZRand - Creates a random device and methods to pull better randoms from
    namespace EZTools
        EZString - This is really an any who's base is std::string
            Includes regex and a lot of extras built in
        EZError - Error type for EZTools
        EZDBCreds - A simple way to store database creds so you don't have to write extra code.
        EZKeyValueMap - A swift like keyValueMap (no where near complete)
        EZMiniVector - A much smaller version of std::vector that only includes the most used parts.
    namespace EZDateTimeFunctions
        EZSystemTime - A simple way to pull the system time and get everything you need including predefined formats.
        EZDateTime -
        EZTimestamp -
        EZTimer - A simple timer that can show how long things take.
EZScreen.h
    namespace EZScreen
        EZMenuObject -
        EZScreenControl - a more simple way of doing cursor movement and color without the need for termcap
EZProc.h
    namespace EZProc
        loadavg - gets the load avg
        MemInfo - gets info about the memory in the system
        kernelVersion - gets the running kernel version
        CPUInfo - returns a LOT of info about your CPU/s
EZLogger.h
    EZLogger - creates an ostream logging service.  Very simple yet works very well.
EZLinux.h
    namespace EZLinux
        homeDir - return your home dir
        currentDir - returns the current dir
        tempDir - returns the system's temp dir
        nullDir - returns the system's null dir
        environmentVar - Look up an env var
        path - returned as a vector
        hostname - returns the hostname of the system
        runningAsRoot - returns a bool
        uid - return the uid of the person running the command
        whoami - returns the username of the person running the command
        exec - runs an app and returns the output as an EZString
        chownFile - chowns a file
        EZFileStat - a lot better info than C stat.
        readFile - returns the contents of the file as an EZString
        copyFile - copy a file
        moveFile - move a file safely
        deleteFile - delete a file
EZHTTP.h
    namespace EZHTTPFunctions
        EZHTTP - Simple C++ wrapper around libcurl
        EZHTTPResponse - the response object for EZHTTP
EZGetOpt
    namespace EZGetOpt
        GetOpt - a C++ replacement for getopt that also builds your -h help system in the process
EZFileManager
    This is in mass development at this time.  If you use it, know that it could change.
EZConfig.h
    EZConfig - reads a json config file in to a Json::Value (jsoncpp)