#define NDEBUG
//#define NDEBUG1
#define DEADLINEMS 930//930
#define DEADLINES 9//9

#include "route.h"
#include "lib/lib_record.h"
#include <stdio.h>
#include <algorithm>
#include <sys/timeb.h>
#include <thread>

PATH path;
vector<LINK> links;
LINK *link;
vector<NODE> nodes(700);
unsigned short SourceID,DestinationID;
vector<unsigned short> V;//is V' . And we can sort it,so when we use ,we can save time.
int msStart;
unsigned long sStart;

//你要完成的功能总入口
void search_route(char *topo[5000], int edge_num, char *demand)
{
    StartTime();

#ifndef NDEBUG
    PrintTime();
#endif // NDEBUG

    for(int i=0;; i++)//input topo[
    {
        int num,countNum;
        num=0;
        countNum=0;
        for(int j=0;; j++)
        {
            if(topo[i][j]>='0' && topo[i][j]<='9')
            {
                num=num*10+topo[i][j]-'0';
            }
            else
            {
                switch(countNum)
                {
                case 0:
                    link=new LINK;
                    link->LinkID=num;
                    num=0;
                    countNum++;
                    break;
                case 1:
                    link->SourceID=num;
                    num=0;
                    countNum++;
                    break;
                case 2:
                    link->DestinationID=num;
                    num=0;
                    countNum++;
                    break;
                case 3:
                    link->Cost=num;
                    num=0;
                    countNum=0;
                    links.push_back(*link);
                    nodes[link->SourceID].nodeLinks.push_back(link);
                    nodes[link->SourceID].ID=link->SourceID;
                    nodes[link->DestinationID].ID=link->DestinationID;
                    break;
                }
                if(countNum==0)
                {
                    break;
                }
            }
        }
        if((int)links.size()==edge_num) break;
    }

    for(int i=0,num=0,countNum=0; ; i++) //input demand
    {
        if(demand[i]>='0' && demand[i]<='9')
        {
            num=num*10+demand[i]-'0';
        }
        else
        {
            switch(countNum)
            {
            case 0:
                SourceID=num;
                num=0;
                countNum++;
                break;
            case 1:
                DestinationID=num;
                num=0;
                countNum++;
                break;
            case 2:
                V.push_back(num);
                num=0;
                break;
            }
        }
        if(demand[i]=='\n' || demand[i]=='\0') break;
    }
    sort(V.begin(),V.end());

    PATH pathTemp;
    pathTemp.cost=0;

    StartThreadSearch(pathTemp);

    while(true)
    {
        struct timeb rawtime;
        ftime(&rawtime);
        int out_ms = rawtime.millitm - msStart;
        unsigned long out_s = rawtime.time - sStart;
        if (out_ms < 0)
        {
            out_ms += 1000;
            out_s -= 1;
        }
        if(out_s==DEADLINES && out_ms>DEADLINEMS)
        {
            if(path.cost==INF)
            {
                cout<<"NA"<<endl;
                break;
            }
            else
            {
                int linkSize;
                linkSize=path.linkIDs.size();
                for(int i=0; i<linkSize; i++)
                {
                    record_result((unsigned short)path.linkIDs[i]);
                }
#ifndef NDEBUG1
                cout<<endl<<path.cost<<endl<<endl;
#endif // NDEBUG1
                break;
            }
        }
    }

    /*unsigned short result[] = {2, 6, 3};//示例中的一个解

    for (int i = 0; i < 3; i++)
        record_result(result[i]);*/
}

NODE::NODE()
{
    isVisted=false;
    isSorted=false;
}

PATH::PATH()
{
    cost=INF;
}

int ThreadSearch(PATH pathTemp,int nodeID,int linkID)
{
    vector<unsigned short> pathNodesCopy;
    pathNodesCopy=pathTemp.nodeIDs;
    sort(pathNodesCopy.begin(),pathNodesCopy.end());
    if(binary_search(pathNodesCopy.begin(),pathNodesCopy.end(),nodeID)==false)
    {
        if(pathTemp.cost+links[linkID].Cost<path.cost)
        {
            pathTemp.cost+=links[linkID].Cost;
            pathTemp.linkIDs.push_back(linkID);
            pathTemp.nodeIDs.push_back(nodeID);
            if(nodeID!=DestinationID)
            {
                int linksSize;
                linksSize=nodes[nodeID].nodeLinks.size();
                for(int i=0; i<linksSize; i++)
                {
                    thread (ThreadSearch,pathTemp,nodes[nodeID].nodeLinks[i]->DestinationID,nodes[nodeID].nodeLinks[i]->LinkID).detach();
                }
            }
            else
            {
                vector<unsigned short> pathNodesCopy;
                pathNodesCopy=pathTemp.nodeIDs;
                sort(pathNodesCopy.begin(),pathNodesCopy.end());
                int nodesSize;
                nodesSize=V.size();
                bool isVVisited;
                isVVisited=true;
                for(int i=0; i<nodesSize; i++)
                {
                    if(binary_search(pathNodesCopy.begin(),pathNodesCopy.end(),V[i])==false)
                    {
                        isVVisited=false;
                        break;
                    }
                }
                if(isVVisited==true)
                {
                    path=pathTemp;
                }
            }
        }
    }
    return 0;
}

int StartThreadSearch(PATH pathTemp)
{
    pathTemp.nodeIDs.push_back(SourceID);
    int linksSize;
    linksSize=nodes[SourceID].nodeLinks.size();
    for(int i=0; i<linksSize; i++)
    {
        thread (ThreadSearch,pathTemp,nodes[SourceID].nodeLinks[i]->DestinationID,nodes[SourceID].nodeLinks[i]->LinkID).detach();
    }
    return 0;
}

bool IsInV(int ID)
{
    return binary_search(V.begin(),V.end(),ID);
}

bool CompareLink(const LINK *a,const LINK *b)
{
    bool isInVa,isInVb;
    isInVa=IsInV(a->DestinationID);
    isInVb=IsInV(b->DestinationID);
    if(isInVa==true && isInVb==false)  return true;
    if(isInVa==false && isInVb==true)  return false;
    //若都在，或都不在，则比较Cost
    if(a->DestinationID==SourceID) return true;
    return a->Cost<b->Cost;
}

#ifndef NDEBUG
void PrintTime()
{
    struct timeb rawtime;
    ftime(&rawtime);
    int out_ms = rawtime.millitm - msStart;
    unsigned long out_s = rawtime.time - sStart;
    if (out_ms < 0)
    {
        out_ms += 1000;
        out_s -= 1;
    }
    printf("used time is %lu s %d ms.\n", out_s, out_ms);
}
#endif // NDEBUG

void StartTime()
{
    struct timeb rawtime;
    ftime(&rawtime);
    msStart = rawtime.millitm;
    sStart = rawtime.time;
}
