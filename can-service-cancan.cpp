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

int s, file;
bool loop = true;
char buffer[MAX_DATA_BYTE*2]; //each byte is represented with 2 hex symbols, so 2 chars
char log_row_buffer[LOG_LENGTH_BYTE];

void quit_handler(int signal){
    loop = false;
    if(s>0) close(s);
    if(file>0) close(file);
}

int main()
{

    signal(SIGINT, quit_handler);
    signal(SIGKILL, quit_handler);
    signal(SIGQUIT, quit_handler);
    signal(SIGTSTP, quit_handler);

    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    file = open("can.log", O_WRONLY|O_APPEND);
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
            printf("i am here\n");
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
            
            //memset(log_row_buffer, 0, LOG_LENGTH_BYTE);
            sprintf(log_row_buffer, "(%ld.%ld) %s %03X#%s\n", tv.tv_sec, tv.tv_usec, ifr.ifr_name, frame.can_id, buffer);
            //sprintf(log_row_buffer, "%s\n", buffer);
            printf("(%ld.%ld) %s %03X#%s\n", tv.tv_sec, tv.tv_usec, ifr.ifr_name, frame.can_id, buffer);
            //printf("(%ld.%ld) %s %03X#%s\n", tv.tv_sec, tv.tv_usec, ifr.ifr_name, frame.can_id, buffer);
           
            //write(file, log_row_buffer, strlen(log_row_buffer));
            
            //printf("%s\n", log_row_buffer);
            fflush(stdout);
            sleep(1);
            //printf("sto cazzo vai a capo\n");

            
            //printf("received at %ld.%ld\n", tv.tv_sec, tv.tv_usec*1000000);

            //printf("data: %s", frame.data);
        }

        //EagleTelemetry::CANMessage message;
        //message.setId(frame.can_id);
        
    }
    return 0;
}