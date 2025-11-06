/* Accepts the following arguments: 
   the first argument is a full path to a file (writefile); 
   the second argument is a text string which will be written within this file (writestr) 
 
   Creates a new file with name and path writefile with content writestr, 
   overwriting any existing file and creating the path if it doesn’t exist. 
*/
 
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h> 
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>

int main(int argc, char **argv){

    // setup syslog logging using the LOG_USER facility   
    openlog(NULL, 0, LOG_USER);

    // exits with value 1 error and print statements if any of the arguments were not specified
    if(argc != 3){
        syslog(LOG_ERR, "Directory path or text string not specified.\n");
        syslog(LOG_ERR, "Usage: %s <writefile> <writestr>", argv[0]);
        return 1;
    }

    // argv[0] = program name
    // argv[1] = writefile
    // argv[2] = writestr 
    const char* writefile = argv[1];
    const char* writestr = argv[2];

    // assumes directory is created by the caller 
    syslog(LOG_DEBUG, "Writing %s to %s", writestr, writefile);

    int fd = open(writefile, O_WRONLY | O_CREAT, S_IRWXU|S_IRWXG|S_IRWXO);
    if(fd == -1){
        syslog(LOG_ERR, "Failed to open file %s", writefile);
        return 1;
    }

    size_t str_size = strlen(writestr)+1; 
    ssize_t bytes_written = write(fd, writestr, str_size);
    if(bytes_written != str_size){
        syslog(LOG_ERR, "Failed to write to file %s", writefile);
        close(fd);
        return 1;
    }
    
    close(fd);
    
    return 0; 
}