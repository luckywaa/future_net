#ifndef __ROUTE_H__
#define __ROUTE_H__

#include <iostream>
#include <vector>

using namespace std;

const int INF=9999999;

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
};

bool DeepSearch(NODE &node,LINK *link);
bool StartDeepSearch();
bool IsInV(int ID);
bool CompareLink(const LINK *a,const LINK *b);

#ifndef NDEBUG
void PrintTime();
#endif // NDEBUG

void StartTime();

#endif
