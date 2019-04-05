**EZTools**

EZTools has a lot of handy tools for Linux as a shared lib.

**EZLinux.h**
_homedir()_ - returns the home directory of the user running your code
_fileExists(EZString name)_ - returns a true if the file exsists
_copyFile(EZString SRC, EZString DEST, bool overwrite = false)_ - Copy a file
_deleteFile(EZString filenameFullPath)_ - delete a file
_chownFile(EZString file_path, EZString user_name, EZString group_name)_ - chown a file
_systemHostname()_ - gets the system's hostname
_runningAsRoot()_ - returns true if you are running as root
_uid()_ - returns the uid of the person running the code
_whoami()_ - returns the username of the person running the code
_exec(const char \*cmd)_ - runs a command and returns the output as a string
_workingDir()_ - returns the working dir
_FilePerms_ - a simple class for viewing file permitions

**EZLogger** - A logging class that's easy to use.

**EZProc**
l*oadavg()* - returns a struct with the system's load
_MemInfo_ - a class for viewing memory stats
_kernelVersion()_ - returns the Linux kernel verion
_CPUInfo_ - a class to view all the cpu info about the system

**EZHTTP** - a libcurl based C++ class for simple use.  Use get(URL) and it returns a EZHTTPResponse object.  Built in JSON via jsoncpp

**EZSystemTime** - a lib center around getting the current date and time and displaying it in many formats

**EZString** - Includes std:string and extends it with many handy methods

**EZScreen** - clrscr() because I always forget how to do it without a system call
