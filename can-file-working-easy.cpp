#include <linux/can.h>
#include <unistd.h>
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
#define LOG_LENGTH_BYTE 1000

int s;
FILE *file = NULL;
bool loop = true;
char buffer[MAX_DATA_BYTE*2]; //each byte is represented with 2 hex symbols, so 2 chars
char log_row_buffer[LOG_LENGTH_BYTE];

void quit_handler(int signal){
    loop = false;
    if(s>0) close(s);
    if(file != NULL) fclose(file);
}

int main()
{

    signal(SIGINT, quit_handler);
    signal(SIGKILL, quit_handler);
    signal(SIGQUIT, quit_handler);
    signal(SIGTSTP, quit_handler);

    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    file = fopen("can.log", "a");
    if (s < 0 || file < 0)
    {
        perror("error");
    }
    else
    {
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
                fprintf(stderr, "Read error: nothing to read\n");
                return 1;
            }

            if (nbytes < sizeof(struct can_frame))
            {
                fprintf(stderr, "read: incomplete CAN frame\n");
                return 1;
            }

            struct timeval tv;
            ioctl(s, SIOCGSTAMP_OLD, &tv);

            //printf("%d [%d] ",frame.can_id, frame.can_dlc);
            for (int i = 0; i < frame.can_dlc; i++){
                //printf("%02X ",frame.data[i]);
                sprintf(buffer+i*2, "%02X", frame.data[i]);
            }

            //printf("canid = %03X - ", frame.can_id);
            

            fprintf(file, "(%ld.%06ld) %s %03X#%s\n", tv.tv_sec, tv.tv_usec, ifr.ifr_name, frame.can_id, buffer);

            fflush(stdout);
        }

        //EagleTelemetry::CANMessage message;
        //message.setId(frame.can_id);
        
    }
    return 0;
}