#import <Foundation/Foundation.h>

#import "get_app_path.h"


typedef unsigned long SearchPathDirectory;
typedef unsigned long SearchPathDomainMask;

const char * getUserApplicationSupportDir(void) {
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSArray *URLs = [fileManager URLsForDirectory:(NSSearchPathDirectory)NSApplicationSupportDirectory inDomains:(NSSearchPathDomainMask)NSUserDomainMask];
    if (URLs.count == 0) return NULL;

    NSURL *URL = [URLs objectAtIndex:0];
    NSString *path = URL.path;

    return path.fileSystemRepresentation;
}
