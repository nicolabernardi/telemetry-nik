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
#include "client_lib.c"


#define MAX_DATA_BYTE 8
#define MAX_LOG_LENGTH_BYTE 100

#define TLM_ID 0xA0
#define TLM_ENABLE_MESSAGE "6601000000000000"
#define TLM_IDLE_MESSAGE "6600000000000000"

#define PILOT "default"
#define RACE "default"
#define CIRCUIT "default"

#define SERVER_SEND_BUFF_DIM 256
#define MSG_TO_SERVER_LEN 256*16
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8000

int s, file;
bool loop = true;
char buffer[MAX_DATA_BYTE*2+1]; //each byte is represented with 2 hex symbols, so the length is #byte*2 chars + string terminator
char log_row_buffer[MAX_LOG_LENGTH_BYTE];
int serversockfd;

char sock_buffer[MSG_TO_SERVER_LEN];
bool send_to_server = true;

enum State{
    IDLE = 0,
    ENABLED = 1
};


/* Handles program quitting/closing: closes CAN socket and log file */
void quit_handler(int signal){
    loop = false;
    if(s>0) close(s);
    if(file>0) close(file);
    if(serversockfd>0) close(serversockfd);
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
    int count_msg_server = 0;
    int pos_in_buffer = 0;
    //int pos_in_buffer = 1;
    //sock_buffer[0] = '[';
    //USO_NORMALE State currentState = IDLE;   //da rimettere questo
    State currentState = ENABLED;     //per debug 

    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    time_t rawtime;
    struct tm *info;
    time( &rawtime );
    info = localtime( &rawtime );
    char file_name[100];
    sprintf(file_name, "/home/ubuntu/can_logger/can_%0d-%02d-%02d_%02d:%02d:%02d.log", 1900+info->tm_year, info->tm_mon, info->tm_mday, info->tm_hour, info->tm_min, info->tm_sec);

    file = open(file_name, O_WRONLY|O_APPEND|O_CREAT, 0666);
    if (s < 0 || file < 0)
    {
        error_handler(strerror(errno), 1, true);
    }
    else{
    
        struct ifreq ifr;
        strcpy(ifr.ifr_name, "can0" );
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

        serversockfd = connectToServer(SERVER_IP, SERVER_PORT);
        if(serversockfd < 0){
            fprintf(stderr, "Error on socket to server: %d\n", serversockfd);
        }

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

                
                switch(currentState){
                    case IDLE:{
                        if(frame.can_id == TLM_ID && strcmp(buffer, TLM_ENABLE_MESSAGE) == 0)
                            currentState = ENABLED;
                        break;
                    }
                    case ENABLED:{

                        if(frame.can_id == TLM_ID && strcmp(buffer, TLM_IDLE_MESSAGE) == 0)
                            currentState = IDLE;
                        else{
                            //write to file
                            snprintf(log_row_buffer, MAX_LOG_LENGTH_BYTE, "(%ld.%06ld) %s %03X#%s\n", tv.tv_sec, tv.tv_usec, ifr.ifr_name, frame.can_id, buffer);
                            write(file, log_row_buffer, strlen(log_row_buffer));

                            //send to server
                            if(!(serversockfd < 0) && send_to_server){
                                char buffer_padded[16+1];
                                int len = strlen(buffer);
                                for(int i=0; i>16-len; i++)
                                    buffer_padded[i] = '0';
                                for(int i=16-len; i<17; i++){
                                    buffer_padded[i] = buffer[i-(16-len)];
                                }
                                printf("buffer_padded = %s\n", buffer_padded);
                                sprintf(sock_buffer+pos_in_buffer, "(%ld.%06ld; %03X#%s)", tv.tv_sec, tv.tv_usec, frame.can_id, buffer_padded);
                                pos_in_buffer = strlen(sock_buffer);
                                //roba sperimentale
                                /*if(sendMessage(sock_buffer, MSG_TO_SERVER_LEN, serversockfd) < 0){
                                        send_to_server = false; //il server è crashato; non faccio crashare questo programma
                                }
                                bzero(sock_buffer, MSG_TO_SERVER_LEN);
                                pos_in_buffer = 0;*/
                                //fine roba sperimentale
                                if(count_msg_server == 15){
                                    count_msg_server = 0;
                                    sprintf(sock_buffer+pos_in_buffer, "]");
                                    printf("dim.stringa = %d\n", strlen(sock_buffer));
                                    printf("%s\n", sock_buffer);
                                    if(sendMessage(sock_buffer, MSG_TO_SERVER_LEN, serversockfd) < 0){
                                        send_to_server = false; //il server è crashato; non faccio crashare questo programma
                                    }
                                    //printf("sockbuffer = %s\n", sock_buffer);
                                    bzero(sock_buffer, MSG_TO_SERVER_LEN);
                                    sock_buffer[0] = '[';
                                    pos_in_buffer = 1;
                                }else{
                                    count_msg_server++;
                                    sprintf(sock_buffer+pos_in_buffer, ",");
                                    pos_in_buffer++;
                                }
                            }
                        
                            counter++;

                            //update file every 500 messages
                            if(counter == 500){
                                counter = 0;
                                close(file);
                                //printf("closed file, now reopen\n"); //for debugging
                                file = open(file_name, O_WRONLY|O_APPEND);
                                if (file < 0)
                                {
                                    error_handler(strerror(errno), 4, true);
                                }
                            }
                        }
                        
                    }
                }

                

            }
        }
    }
    return 0; //not really useful
}