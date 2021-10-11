#include <map>

namespace EagleTelemetry
{

    class CANMessage
    {

        int reverse_priority; //-The Standard CAN 11-bit identifier establishes the priority of the message.
                              //The lower the binary value, the higher its priority
        int extended_identifier;
        int data_bytes;
        std::map<std::string, std::map<std::string, std::string>> data; //mappa di coppie chiave-valore per rappresentare l'oggetto incapsulato
                                                                        //nel messaggio, simile agli oggetti js
    };

};
