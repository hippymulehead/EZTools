//
// Created by mromans on 5/16/19.
//

#include "EZLinux.h"

EZLinux::EZFileStat::EZFileStat(EZString filename) {
    Poco::Path p(filename);
    _path.swap(p);
    Poco::File f(_path);
    _file.swap(f);
    _isThere = _file.exists();
    if (_isThere) {
        _isReadable = _file.canRead();
        _isWriteable = _file.canWrite();
        _isReadable = _file.canRead();
        struct stat fileStat;
        if (stat(filename.c_str(), &fileStat) < 0)
            exit(EXIT_FAILURE);

        if (fileStat.st_mode & S_IRUSR) {
            _owner = _owner + 4;
        }
        if (fileStat.st_mode & S_IWUSR) {
            _owner = _owner + 2;
        }
        if (fileStat.st_mode & S_IXUSR) {
            _owner = _owner + 1;
        }
        if (fileStat.st_mode & S_IRGRP) {
            _group = _group + 4;
        }
        if (fileStat.st_mode & S_IWGRP) {
            _group = _group + 2;
        }
        if (fileStat.st_mode & S_IXGRP) {
            _group = _group + 1;
        }
        if (fileStat.st_mode & S_IROTH) {
            _other = _other + 4;
        }
        if (fileStat.st_mode & S_IWOTH) {
            _other = _other + 2;
        }
        if (fileStat.st_mode & S_IXOTH) {
            _other = _other + 1;
        }

        if (S_ISREG(fileStat.st_mode) == 1) { _FT = 1; }
        if (S_ISLNK(fileStat.st_mode) == 1) { _FT = 2; }
        if (S_ISDIR(fileStat.st_mode) == 1) { _FT = 3; }
        if (S_ISCHR(fileStat.st_mode) == 1) { _FT = 4; }
        if (S_ISBLK(fileStat.st_mode) == 1) { _FT = 5; }
        if (S_ISFIFO(fileStat.st_mode) == 1) { _FT = 6; }
        if (S_ISSOCK(fileStat.st_mode) == 1) { _FT = 7; }

        _size = fileStat.st_blksize;
        _links = fileStat.st_nlink;
        _inode = fileStat.st_ino;
        _hlinks = fileStat.st_nlink;
        _fowner = fileStat.st_uid;
        _gowner = fileStat.st_gid;
        _devid = fileStat.st_rdev;
        _blocksize = fileStat.st_blksize;
        _blocks = fileStat.st_blocks;
    }
}

EZLinux::EZSTATFILETYPE EZLinux::EZFileStat::fileType() {
    switch (_FT) {
        case 0:
            return UNK;
        case 1:
            return REG;
        case 2:
            return LNK;
        case 3:
            return DIR;
        case 4:
            return CHAR;
        case 5:
            return BLK;
        case 6:
            return FIFO;
        case 7:
            return SOCK;
        default:
            return UNK;
    }
}

EZString EZLinux::EZFileStat::fileTypeAsStr() {
    switch (_FT) {
        case 0:
            return "Unknown";
        case 1:
            return "Regular";
        case 2:
            return "Link";
        case 3:
            return "Directory";
        case 4:
            return "Character Device";
        case 5:
            return "Block Device";
        case 6:
            return "FIFO Device";
        case 7:
            return "Socket";
        default:
            return "Unknown";
    }
}

EZString EZLinux::EZFileStat::permsAsStr() {
    stringstream bs;
    bs << _owner << _group << _other;
    return bs.str();
}

int EZLinux::EZFileStat::permsAsInt() {
    EZString es = permsAsStr();
    return es.asInt();
}