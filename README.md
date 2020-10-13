# Two load balancing algorithms to be implemented
## 1 - Spread
Spread is a dynamic load balancing algorithm that aims at optimizing
the overall load balancing through a uniform load spread in
the server network. Load shedding is triggered when the number
of players exceeds a single-server’s capacity (in terms of CPU or
bandwidth towards its clients) just as in our main dynamic partitioning
algorithm.\
This algorithm, however, attempts to uniformly spread the players
across all participating servers through global reshuffling of regions
to servers. The algorithm is meant to be an extreme where
the primary concern is the global optimum in terms of the smallest
difference between the lowest and highest loaded server in the
system. There are no attempts at locality preservation in either
network proximity or region adjacency. The algorithm is a binpacking
algorithm that needs global load information for all participating
servers. The algorithm starts with one empty bin per server,
then in successive steps takes a region that is currently mapped at
any of the servers and places it in the bin with the current lightest
load. This is done iteratively until all regions have been placed
into bins. After the global reshuffle schedule is created, each bin is
assigned to a particular server and the corresponding region migrations
occur. While the algorithm could be adapted to include only
a subset of servers (e.g., just neighbors and their neighbors) into
the region reshuffle process, we currently involve all servers in this
process.

## 2 - Lightest
Lightest is a dynamic load balancing algorithm that attempts to
optimize the cost of remapping by prioritizing shedding load to a
single server instead of several servers. The algorithm does not take
network proximity in the game (i.e., to neighbors) into account.
Furthermore, clustering of adjacent regions is maintained whenever
possible but is of secondary concern compared to load shedding to
a single server.\
An overloaded server tries to shed load directly to the lightest
loaded node known. The precondition is that this node’s load has
to be below Light load th. Note that our definition of Light load th
is such that a single such node should be able to accommodate a
sufficient load shed from an overloaded node. While this is true in
most cases, depending on the actual distribution of players to regions,
if some regions are more overloaded than others, a careful
load shedding schedule should be constructed to maximize the load
to be shed. The lightest loaded node may in fact be a neighbor, but
the Lightest algorithm does not give preference to neighbors when
shedding load except in case of load ties. Instead, the overloaded
node prefers shedding a sufficient portion of its load to a single
server even if this server is remote and even if this implies some
declustering for the shedded regions. Regions of the overloaded
node are scanned in-order and placed into a bin to be assigned to
the lightly loaded node. Thus, Lightest attempts to keep region
clusters together by scanning adjacent regions in sequence. On the
other hand, if a region cannot be placed in the bin without exceeding
the safe load threshold, a subsequent region is selected, hence
sacrificing on region locality.

# VNC set up guild

- Install Tiger vnc
    * MacOS :  
    ```sh
    $ brew cask install tigervnc-viewer
    ```
    * Windows: TBD
    * Linux: TBD
  
- SSH into one of the ug machine and run:
    ```sh
    $ ssh username@ugxxx.eecg.toronto.edu
    $ ece297vnc start
    ```
- Follow the instruction in command window to start a VNC session:
    eg: for **Mac**
    1. On your local machine run:
    ```sh
    $ ssh -L 5901:127.0.0.1:5901 username@ugxxx.eecg.toronto.edu
    ```
    2. Then open TigerVNC and connect to:
    ```sh
    127.0.0.1:5901
    ```

# Compile SimMud

- Run make under the root directory
    ```sh
    $ make
    ```
    
# Start Server and clIENT

- To start server, run:
    ```sh
    $ ./server config_demo.ini 'port'
    ```
- To start client, run:
    ```sh
    $ ./client --gui 'server port'
    ```
# Original README

1. BUILDING THE GAME FROM SOURCES

Make sure all the needed libraries are installed:
	- SDL - http://www.libsdl.org/
	- SDL_net - http://www.libsdl.org/projects/SDL_net/
	- OpenGL

The script "sdl-config" must be present and return the valid path to the SDL header and libraries. Otherwise manually edit the CXXFLAGS and LDFLAGS in parameters in the Makefile. Also change the compiler name in the makefile if it is not g++.

Build:	make
Clean:	make clean

Run server:		./server <config_file> <server_port>
Run client:		./client [--gui] <server_ip:server_port>

2. CONFIGURATION FILE

The relevant settings that can be specified in the configuration file and their meaning follow:


[Server]

server.number_of_threads = 4				//	number of threads used by the server
server.regular_update_interval = 50			//	number of milliseconds between 2 consecutive updates of the world (form server to clients)

server.balance = static						//	algorithm used for load balancing
server.load_balance_limit = 10				//	number of seconds between 2 consecutive load re-distributions of regions to threads

[Map]

map.width = 16								//	size of the map ( in number of client-areas-of-interest )
map.height = 16

map.region_min_width = 4					//	size of a region ( in number of client-areas-of-interest )
map.region_min_height = 4

map.blocks = 50								//	number of blocks on the map for every 1000 cells
map.resources = 30							//	number of resources on the map for every 1000 cells

map.min_res = 1								//	min/max value of a resource at initialization
map.max_res = 10



[Player]

player.min_life = 20						//	min/max value of a players life at initialization
player.max_life = 60

[Quest]

quest.bonus = 10							//	bonus given to the players that have reached the quest area

quest.between = 20							//	time between quests

quest.min = 40								//	min/max duration of a quest (seconds)
quest.max = 90
