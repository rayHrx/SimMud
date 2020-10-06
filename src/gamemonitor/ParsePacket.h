#include <wx/wx.h>
#include "General.h"
#include "../utils/Serializator.h"

class ParsePacket
{
private:
    Serializator *s;

    void ParseServerStats();
    void ParseThreadStats();
    void ParseRegionInfo();
    void ParseMapData();
public:
    ParsePacket();
    ~ParsePacket();

    void Parse(UDPpacket* packet);
};
