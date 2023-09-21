#include "EZApp.h"
#include "EZAppFull.h"
#include "EZAppHTTP.h"
#include "EZAppNagiosPlugin.h"
#include "EZCGI.h"
#include "EZConfig.h"
#include "EZCryptopp.h"
#include "EZCSV.h"
#include "EZDateTime.h"
#include "EZDice.h"
#include "EZDNS.h"
#include "EZDNSBackend.h"
#include "EZFiles.h"
#include "EZGetOpt.h"
#include "EZHTML.h"
#include "EZHTMLDocument.h"
#include "EZHTTP.h"
#include "EZIP.h"
#include "EZJSON.h"
#include "EZJSONDataBaseTypes.h"
#include "EZJSONDataTypes.h"
#include "EZLinux.h"
#include "EZLogger.h"
#include "EZMySQL.h"
#include "EZNagios.h"
#include "EZPersonalWeatherStation.h"
#include "EZProc.h"
#include "EZRandom.h"
#include "EZScreen.h"
#include "EZSpark.h"
#include "EZSyslog.h"
#include "EZUUID.h"
#include "EZWeather.h"
#include "nlohmann/json.hpp"
#include "EZUniversalLogger.h"
#include "EZNetworkTests.h"
#include "EZZipFiles.h"
#include "EZMySQLJSON.h"

using namespace std;

int main(int argc, char **argv) {
    EZMySQL::Database db;
    db.connect("192.168.1.42", "weatherd","weather", "IMin!!#$56");
    if (!db.connected()) {
        cout << "Not Connected";
        return 1;
    }
    auto root = db.queryAsJSON("select * from records where tempereture >= 100");
    if (!root) {
        cout << root.message() << endl;
        return 1;
    }
    cout << root.data.dump(4) << endl;
//    cout << root.data.dump(4) << endl;

    db.disconnect();

    return 0;

//    EZFiles::URI l("/home/mromans/Nextcloud/Programming/EZTools_6.0/EZTools/eztools/log.log");
//    EZUniversalLogger::Logger log(l,EZUniversalLogger::LOGLEVEL::DEBUG);
//    log << log.currentLogID() << EZUniversalLogger::logline({"Current Log Level", log.currentLogLevel()}) << endl;
//    log.writeLevel(EZUniversalLogger::LOGLEVEL::CRITICAL);
//    log << log.currentLogID() << EZUniversalLogger::logline({"This is a critical log"}) << endl;
//    log.writeLevel(EZUniversalLogger::LOGLEVEL::INFO);
//    log << log.currentLogID() << EZUniversalLogger::logline({"This is a info log","It has many elements",
//                                                             "but all work like a charm","and all work with regex",
//                                                             "and none follow the horrible apace standard"})
//                                                             << endl;
//    log.writeLevel(EZUniversalLogger::LOGLEVEL::DEBUG);
//    log.newID();
//    log << log.currentLogID() << EZUniversalLogger::logline({"This is a debug log"}) << endl;
//    log.writeLevel(EZUniversalLogger::LOGLEVEL::QUITE);
//    log << log.currentLogID() << EZUniversalLogger::logline({"This is a quite log"}) << endl;
//    log.defaultLogLevel();
//    log << log.currentLogID() << EZUniversalLogger::logline({"This is a default log line"}) << endl;
//
//    bool test = true;
//    EZAppFull::App app(argc, argv, "EZTools Test App");
//    EZFiles::URI logfile("eztools.log");
//    EZFiles::deleteFile(&logfile);
//    app.log.init(logfile,EZTools::LOGLEVEL::INFO);
//    app.log.writeLevel(EZTools::INFO);
//
//    // EZTools
//    auto eztools = EZTools::TEST();
//    cout << eztools.out();
//    app.log << eztools.out();
//    if (test & !eztools.wasSuccessful()) { test = false; }
//
//    // EZFiles
//    auto ezfiles = EZFiles::TEST();
//    cout << ezfiles.out();
//    app.log << ezfiles.out();
//    if (test & !ezfiles.wasSuccessful()) { test = false; }
//
//    // EZDNS
//    auto ezdns = EZDNS::TEST();
//    cout << ezdns.out();
//    app.log << ezdns.out();
//    if (test & !ezdns.wasSuccessful()) { test = false; }
//
//    //EZDice
//    auto ezdice = EZDice::TEST();
//    cout << ezdice.out();
//    app.log << ezdice.out();
//    if (test & !ezdice.wasSuccessful()) { test = false; }
//
//    // EZLinux
//    auto ezlinux = EZLinux::TEST();
//    cout << ezlinux.out();
//    app.log << ezlinux.out();
//    if (test & !ezlinux.wasSuccessful()) { test = false; }
//
//    // EZProc
//    auto ezproc = EZProc::TEST();
//    cout << ezproc.out();
//    app.log << ezproc.out();
//    if (test & !ezproc.wasSuccessful()) { test = false; }
//
//    // EZNagios
//    auto eznagios = EZNagios::TEST();
//    cout << eznagios.out();
//    app.log << eznagios.out();
//    if (test & !eznagios.wasSuccessful()) { test = false; }
//
//    // EZIP
//    auto ezip = EZIP::TEST();
//    cout << ezip.out();
//    app.log << ezip.out();
//    if (test & !ezip.wasSuccessful()) { test = false; }
//
//    if (test) {
//        app.close(EZTools::SUCCESS);
//    } else {
//        app.close(EZTools::FAIL);
//    }

    return 0;
}