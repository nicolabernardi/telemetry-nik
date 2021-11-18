#include <linux/can.h>
#include <unistd.h>
#include <stdlib.h>
#include <net/if.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include "naked_generator/out/bms/c/bms.h"
#include "naked_generator/out/Primary/c/Primary.h"
#include "naked_generator/out/Secondary/c/Secondary.h"
#include "includes_generator/out/bms/c/bms.h"
#include "includes_generator/out/Primary/c/Primary.h"
#include "includes_generator/out/Secondary/c/Secondary.h"

#define MAX_DATA_BYTE 8
#define MAX_LOG_LENGTH_BYTE 100

#define PILOT "default"
#define RACE "default"
#define CIRCUIT "default"

int s, file;
bool loop = true;
char buffer[MAX_DATA_BYTE*2+1]; //each byte is represented with 2 hex symbols, so the length is #byte*2 chars + string terminator
char log_row_buffer[MAX_LOG_LENGTH_BYTE];


/* Handles program quitting/closing: closes CAN socket and log file */
void quit_handler(int signal){
    loop = false;
    if(s>0) close(s);
    if(file>0) close(file);
    exit(0);
}

/* Error handler: writes error to console and to error log, quits program */
void error_handler(const char *err_message, int no, bool quit){
    fprintf(stderr, "%s", err_message);
    int errfile = open("err.log", O_WRONLY|O_APPEND|O_CREAT, 0666);
    char errbuffer[500];
    sprintf(errbuffer, "%ld %d %s\n", (unsigned long int)time(NULL), no, err_message);
    write(errfile, errbuffer, strlen(errbuffer));
    close(errfile);
    if(quit) exit(no);
}

/*

    Prints to the open file indicated through the file descriptor fd the following header:

    *** EAGLE-TRT
    *** Telemetry Log File
    *** <DATE>

    *** Pilot: <PILOT>
    *** Race: <RACE>
    *** Circuit: <CIRCUIT>
*/
void print_file_header(int fd){
    char headerbuffer[300];
    time_t curtime;
    time(&curtime);
    snprintf(headerbuffer, 300, "\n*** EAGLE-TRT\n*** Telemetry Log File\n*** %s\n*** Pilot: %s\n*** Race: %s\n*** Circuit: %s\n\n", 
        ctime(&curtime), PILOT, RACE, CIRCUIT);
    write(fd, headerbuffer, strlen(headerbuffer));
}

int main()
{

    signal(SIGINT, quit_handler);
    signal(SIGKILL, quit_handler);
    signal(SIGQUIT, quit_handler);
    signal(SIGTSTP, quit_handler);

    int counter=0;

    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    file = open("can.log", O_WRONLY|O_APPEND|O_CREAT, 0666);
    if (s < 0 || file < 0)
    {
        error_handler(strerror(errno), 1, true);
    }
    else{
    
        struct ifreq ifr;
        strcpy(ifr.ifr_name, "vcan0" );
        ioctl(s, SIOCGIFINDEX, &ifr);
        

        struct sockaddr_can addr;
        memset(&addr, 0, sizeof(addr));
        addr.can_family = AF_CAN;
        addr.can_ifindex = ifr.ifr_ifindex;
        if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            error_handler(strerror(errno), 2, true);
        }

        int nbytes;
        struct can_frame frame;
        int error_count = 0;

        print_file_header(file);

        while(loop){
            nbytes = read(s, &frame, sizeof(struct can_frame));
            if (nbytes < 0) {
                
                error_count++;
                if(error_count < 10)
                    error_handler("Read error: read less than 0 bytes\n", 3, false);
                else //continuos error, it's better to stop here
                    error_handler("Read error: read less than 0 bytes\n", 3, true);
            }else{


                error_count = 0; //if all goes well, reset error counting: i want to keep track of only consecutive repeated errors

                struct timeval tv;
                ioctl(s, SIOCGSTAMP_OLD, &tv);

                for (int i = 0; i < frame.can_dlc; i++){
                    sprintf(buffer+i*2, "%02X", frame.data[i]);
                }
                buffer[frame.can_dlc*2+1] = 0; //string terminator

                //write to file
                snprintf(log_row_buffer, MAX_LOG_LENGTH_BYTE, "(%ld.%06ld) %s %03X#%s\n", tv.tv_sec, tv.tv_usec, ifr.ifr_name, frame.can_id, buffer);
                write(file, log_row_buffer, strlen(log_row_buffer));
                

                counter++;

                //update file every 500 messages
                if(counter == 500){
                    counter = 0;
                    close(file);
                    //printf("closed file, now reopen\n"); //for debugging
                    file = open("can.log", O_WRONLY|O_APPEND);
                    if (file < 0)
                    {
                        error_handler(strerror(errno), 4, true);
                    }
                }

            }
        }
    }
    return 0; //not really useful
}