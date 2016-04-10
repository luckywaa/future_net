#ifndef __ROUTE_H__
#define __ROUTE_H__

#include <iostream>
#include <vector>

using namespace std;

const int INF=999999;
const int NODESNUM=700;

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
    bool isSorted;
};
class PATH
{
public:
    vector<int> linkIDs;
    int cost;
    vector<unsigned short> nodeIDs;
    PATH();
    vector<unsigned short> sortedNodeIDs;
};

bool IsInV(int ID);
bool CompareLinkCost(const LINK *a,const LINK *b);
bool CompareLink(const LINK *a,const LINK *b);
void Floyd();
bool RouteSearch(PATH pathTemp,LINK link);
bool StartRouteSearch(PATH pathTemp);
vector<int> SearchInterface(int sourceNodeID,int destinationNodeID,vector<unsigned short> sortedNodeIDs);
int SearchLinkID(int iSourceID,int iDestinationID);

#ifndef NDEBUG
void PrintTime();
#endif // NDEBUG

void StartTime();


#endif
