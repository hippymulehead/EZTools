#include "../EZTools/EZCryptopp.h"
#include "../EZTools/EZLinux.h"

int main() {
    cout << EZCryptopp::BitGen(42);
    cout << EZLinux::whoami() << endl;
    return 0;
}
