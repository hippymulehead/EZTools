EZTools

EZTools has a lot of handy tools for Linux as a shared lib.

EZLinux.h
homedir() - returns the home directory of the user running your code
fileExists(EZString name) - returns a true if the file exsists
copyFile(EZString SRC, EZString DEST, bool overwrite = false) - Copy a file
deleteFile(EZString filenameFullPath) - delete a file
chownFile(EZString file_path, EZString user_name, EZString group_name) - chown a file
systemHostname() - gets the system's hostname
runningAsRoot() - returns true if you are running as root
uid() - returns the uid of the person running the code
whoami() - returns the username of the person running the code
exec(const char \*cmd) - runs a command and returns the output as a string
workingDir() - returns the working dir
FilePerms - a simple class for viewing file permitions

EZLogger - A logging class that's easy to use.

EZProc
loadavg() - returns a struct with the system's load
MemInfo - a class for viewing memory stats
kernelVersion() - returns the Linux kernel verion
CPUInfo - a class to view all the cpu info about the system

EZHTTP - a libcurl based C++ class for simple use.  Use get(URL) and it returns a EZHTTPResponse object.  Built in JSON via jsoncpp

EZSystemTime - a lib center around getting the current date and time and displaying it in many formats

EZString - Includes std:string and extends it with many handy methods

EZScreen - clrscr() because I always forget how to do it without a system call
