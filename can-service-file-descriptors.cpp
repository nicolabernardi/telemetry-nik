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

#define MAX_DATA_BYTE 8
#define MAX_LOG_LENGTH_BYTE 100

int s, file;
bool loop = true;
char buffer[MAX_DATA_BYTE*2+1]; //each byte is represented with 2 hex symbols, so the length is #byte*2 chars + string terminator
char log_row_buffer[MAX_LOG_LENGTH_BYTE];

void quit_handler(int signal){
    loop = false;
    if(s>0) close(s);
    if(file>0) close(file);
    exit(0);
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
        perror("error");
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
            perror("Bind");
            return 1;
        }

        int nbytes;
        struct can_frame frame;

        while(loop){
            nbytes = read(s, &frame, sizeof(struct can_frame));
            if (nbytes < 0) {
                fprintf(stderr, "Read error: read less than 0 bytes\n");
                return 2;
            }

            if (nbytes < sizeof(struct can_frame))
            {
                fprintf(stderr, "Read error: incomplete CAN frame\n");
                return 3;
            }

            struct timeval tv;
            ioctl(s, SIOCGSTAMP_OLD, &tv);

            //printf("%d [%d] ",frame.can_id, frame.can_dlc);
            for (int i = 0; i < frame.can_dlc; i++){
                //printf("%02X ",frame.data[i]);
                sprintf(buffer+i*2, "%02X", frame.data[i]);
            }
            buffer[frame.can_dlc*2+1] = 0; //string terminator

            //write to file
            snprintf(log_row_buffer, MAX_LOG_LENGTH_BYTE, "(%ld.%06ld) %s %03X#%s\n", tv.tv_sec, tv.tv_usec, ifr.ifr_name, frame.can_id, buffer);
            write(file, log_row_buffer, strlen(log_row_buffer));
            
            //printf("%s", log_row_buffer);

            counter++;

            //update file every 500 messages
            if(counter == 500){
                counter = 0;
                close(file);
                printf("closed file, now reopen\n");
                file = open("can.log", O_WRONLY|O_APPEND);
                if (file < 0)
                {
                    perror("error");
                }
            }
        }
    }
    return 0; //not really useful
}