#include "ParsePacket.h"
#include "MessageWithSerializator.h"

extern ThreadStatistics ts[MAX_THREADS];
extern ServerStatistics ss;
extern RegionInfo ri[MAX_REGIONS];
extern MapData md;

ParsePacket::ParsePacket()
{
}

ParsePacket::~ParsePacket()
{
}

void ParsePacket::Parse(UDPpacket* packet)
{
    MessageWithSerializator m(packet);
    s = m.getSerializator();

    gmPacketType packetType;
    s >> packetType;

    switch(packetType)
    {
        case server_stats:
            ParseServerStats();
            break;
        case thread_stats:
            ParseThreadStats();
            break;
        case region_info:
            ParseRegionInfo();
            break;
        case map_data_packet:
            ParseMapData();
            break;
        default:
            break;
    }
}

void ParsePacket::ParseServerStats()
{
    s >> ss.machine_cpu_usage;
    s >> ss.machine_mem_usage;
    s >> ss.process_cpu_usage;
    s >> ss.process_mem_usage;
    s >> ss.number_of_threads;
    s >> ss.number_of_players;

    s >> ss.average_regular_update_interval;
    s >> ss.average_real_regular_update_interval;
    s >> ss.bps_tcp_recv;
    s >> ss.bps_tcp_sent;
    s >> ss.bps_udp_recv;
    s >> ss.bps_udp_sent;
    s >> ss.tcp_total;
    s >> ss.udp_total;
    s >> ss.number_of_statistics;
}

void ParsePacket::PasreThreadStats()
{
    for (int i=0; i<ss.number_of_threads; i++)
    {
        s >> ts[i].number_of_regions;
        s >> ts[i].number_of_players;
        s >> ts[i].players_in_most_crowded_region;
    }
}

void ParsePacket::ParseRegionInfo()
{
    int num_regions = 0;

    for (int i=0; i<ss.number_of_threads; i++)
    {
        num_regions += ts.number_of_regions;
    }

    for (int i=0; i<num_regions; i++)
    {
        s >> ri[i].x;
        s >> ri[i].y;
        s >> ri[i].sizex;
        s >> ri[i].sizey;
        s >> ri[i].num_players;
        s >> ri[i].thread;
    } 
}

void ParsePacket::ParseMapData()
{
    s >> md.num_threads;
    s >> md.regular_update_interval;

    s >> md.algorithm_name;
    s >> md.overloaded_level;
    s >> md.light_level;

    s >> md.stats_interval;

    s >> md.display_all_warnings;
    s >> md.display_quests;
    s >> md.display_actions;
    s >> md.display_user_on_off;
    s >> md.display_migrations;

    s >> md.mapx;
    s >> md.mapy;
    s >> md.regmaxx;
    s >> md.regmaxy;
    s >> md.regminx;
    s >> md.regminy;

    s >> md.blocks;
    s >> md.resources;
    s >> md.min_res;
    s >> md.max_res;

    s >> md.player_min_life;
    s >> md.player_max_life;
    s >> md.player_min_attr;
    s >> md.player_max_attr;

    s >> md.quest_first;
    s >> md.quest_between;
    s >> md.quest_min;
    s >> md.quest_max;
    s >> md.quest_bonus;
}
