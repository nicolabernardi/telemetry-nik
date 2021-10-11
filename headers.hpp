#include <map>

namespace EagleTelemetry
{

    class CANMessage
    {

        int identifier;
        int data_bytes;
        std::map<std::string, std::map<std::string, std::string>> data; //mappa di coppie chiave-valore per rappresentare l'oggetto incapsulato
                                                                        //nel messaggio, simile agli oggetti js

        void setId(int id);
    };

};
