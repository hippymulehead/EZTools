EZTools is a C++ lib for a lot of things that I do in weather software.  As a side effect it also works well
for some other internet projects as well.
It's licenced under the BSD Simple licence meaning you can use it however you like but you can't blame me for it if it
doesn't work right for you are it causes bad breaks.  I work hard to make sure it doesn't but am not legally liable.

Main repo can be found at https://github.com/hippymulehead/EZTools
If you find bugs, submit the bug fix to there or simply fork the project.

Requirements...

poco-foundation >= 1.6.1
poco-crypto >= 1.6.1
jsoncpp >= 0.10.5
curl >= 7.29.0

Compiling...

rm CMakeCache.txt
cmake .
// On Fedora or Centos
make clean && make package
// Other linux distros
make clean && make install