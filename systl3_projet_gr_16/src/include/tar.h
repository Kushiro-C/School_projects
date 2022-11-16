#define BLOCKSIZE 512

// SOURCE : description(https://en.wikipedia.org/wiki/Tar_(computing))

/*header */

struct header {
                                    //position
    char name[100];                 //0         File name
    char mode[8];                   //100       File mode
    char uid[8];                    //108       Owner's numeric user ID
    char gid[8];                    //116       Group's numeric user ID
    char size[12];                  //124       File size in bytes (octal base)
    char mtime[12];                 //136       Last modification time in numeric Unix time format (octal)
    char chksum[8];                 //148       Checksum for header record
    char typeflag;                  //156       file type
    char linkname[100];             //157       Name of linked file (0 = Normal file, 1 = hard link, 2 = symbolic link)
    char magic[6];                  //257       UStar indicator "ustar" then NUL
    char version[2];                //263       UStar version "00"
    char uname[32];                 //265       Owner user name
    char gname[32];                 //297       Owner group name
    char devmajor[8];               //329       Device major number
    char devminor[8];               //337       Device minor number
    char prefix[155];               //345       Filename prefix
    char junk[12];                  //500       end
};


// SOURCE : TP1

#define TMAGIC   "ustar"        /* ustar and a null */
#define TMAGLEN  6
#define TVERSION "00"           /* 00 and no null */
#define TVERSLEN 2

/* ... */

#define OLDGNU_MAGIC "ustar  "  /* 7 chars and a null */
