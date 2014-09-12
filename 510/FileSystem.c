#include "FileSystem.h"

#define DRIVE "N0:"
struct DRIVE_INFO{
  fsDriveInfo info;
  int64_t free_space;
  uint32_t serial;
  char label[12];
  bool is_initialized;
};

struct DRIVE_INFO drive;

bool fs_initialize(){
//  fsStatus x;
  bool result = true;
  
	if ( finit (DRIVE) != fsOK) // инициализировать диск
		return false;

  switch(fmount(DRIVE)){      //  монтировать диск

    case fsMediaError:        //  ошибка инициализации NFC или адресации ALE, CLE, NFC_BASE
      result = false;
    break;
    
    case fsNoFileSystem:      //  первый запуск прошивки(форматировать) или карта памяти испорчена(данные потеряны)
      //result = 
//    //x = fformat(DRIVE, "/L NANDNTKB /FAT32");
//    //  break не выставляется, чтобы после форматирования работа продолжалась
//    result &= x == fsOK;
    case fsOK:                //  успешный запуск, сбор информации
      result &= finfo(DRIVE, &drive.info) == fvol(DRIVE, drive.label, &drive.serial) == fsOK;
      break;
    
    default: 
      result  = false;        //  иные ошибки
    break;
  };
	
	return result;
}

void fs_uninitialize(){
//	funmount (DRIVE);
//	funinit (DRIVE);  
}
