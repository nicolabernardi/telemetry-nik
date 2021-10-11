#include <linux/can.h>
#include <sys/socket.h>
#include <iostream>

using namespace std;

int main()
{
    int s, nbytes;
    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0)
    {
        cerr << "error";
    }
    else
    {
        struct canfd_frame frame;
        nbytes = read(s, &frame, sizeof(struct canfd_frame));
        if (nbytes < 0)
        {
            perror("can raw socket read");
            return 1;
        }

        /* paranoid check ... */
        if (nbytes < sizeof(struct can_frame))
        {
            fprintf(stderr, "read: incomplete CAN frame\n");
            return 1;
        }

        struct timeval tv;
        ioctl(s, SIOCGSTAMP, &tv);

        EagleTelemetry::CANMessage message;
        message.setId(frame.can_id);
    }
}