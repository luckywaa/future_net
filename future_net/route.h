#ifndef __ROUTE_H__
#define __ROUTE_H__

#include <iostream>
#include <vector>

using namespace std;

void search_route(char *graph[5000], int edge_num, char *condition);

class LINK
{
public:
    int LinkID;
    int SourceID;
    int DestinationID;
    int Cost;
};
class NODE
{
public:
    unsigned short ID;
    vector<LINK*> nodeLinks;
    bool isVisted;
    NODE();
};
class PATH
{
public:
    vector<int> linkIDs;
    int cost;
    vector<unsigned short> nodeIDs;
    PATH();
};



bool DeepSearch(NODE &node,LINK *link);
bool StartDeepSearch();

#endif
