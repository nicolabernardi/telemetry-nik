#include <linux/can.h>
#include <unistd.h>
#include <net/if.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>

using namespace std;

int main()
{

    const int s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    const int file = open("can.log", O_WRONLY|O_APPEND);
    if (s < 0 || file < 0)
    {
        perror("error");
    }
    else
    {
        //printf("i am here\n");

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

        while(true){
            nbytes = read(s, &frame, sizeof(struct can_frame));
            if (nbytes < 0) {
                fprintf(stderr, "Read\n");
                return 1;
            }
            printf("0x%03X [%d] ",frame.can_id, frame.can_dlc);
            for (int i = 0; i < frame.can_dlc; i++)
                printf("%02X ",frame.data[i]);
            printf("\r\n");

            struct timeval tv;
            ioctl(s, SIOCGSTAMP_OLD, &tv);
            printf("received at %ld.%ld\n", tv.tv_sec, tv.tv_usec*1000000);


            /* paranoid check ... */
            /*if (nbytes < sizeof(struct canfd_frame))
            {
                fprintf(stderr, "read: incomplete CAN frame\n");
                return 1;
            }*/

            //printf("data: %s", frame.data);
        }

        //EagleTelemetry::CANMessage message;
        //message.setId(frame.can_id);
        close(s);
    }
    return 0;
}