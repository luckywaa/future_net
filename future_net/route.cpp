#define NDEBUG
#define NDEBUG1
#define DEADLINEMS 970//930
#define DEADLINES 9//9

#include "route.h"
#include "lib/lib_record.h"
#include <stdio.h>
#include <algorithm>
#include <sys/timeb.h>
#include <memory.h>

PATH path;
vector<LINK> links;
LINK *link;
vector<NODE> nodes(NODESNUM);
vector<unsigned short> allNodeIDs;
int D[NODESNUM][NODESNUM];
int PATHD[NODESNUM][NODESNUM];//全局变量自动初始化为0
unsigned short SourceID,DestinationID;
vector<unsigned short> V;//is V' . And we can sort it,so when we use ,we can save time.
int msStart;
unsigned long sStart;

//你要完成的功能总入口
void search_route(char *topo[5000], int edge_num, char *demand)
{
    StartTime();
    memset(D,INF,NODESNUM*NODESNUM*sizeof(int));
    memset(PATHD,-1,NODESNUM*NODESNUM*sizeof(int));

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
                    if(binary_search(allNodeIDs.begin(),allNodeIDs.end(),link->SourceID)==false)
                    {
                        allNodeIDs.push_back(link->SourceID);
                        sort(allNodeIDs.begin(),allNodeIDs.end());
                    }
                    if(binary_search(allNodeIDs.begin(),allNodeIDs.end(),link->DestinationID)==false)
                    {
                        allNodeIDs.push_back(link->DestinationID);
                        sort(allNodeIDs.begin(),allNodeIDs.end());
                    }
                    if(link->Cost<D[link->SourceID][link->DestinationID]) //生成Floyd算法的两个二位数组
                    {
                        D[link->SourceID][link->DestinationID]=link->Cost;
                        D[link->SourceID][link->SourceID]=0;
                        PATHD[link->SourceID][link->DestinationID]=link->SourceID;
                    }
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

    Floyd();
    //cout<<SearchInterface(0,14)<<endl;

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

#ifndef NDEBUG
    PrintTime();
#endif // NDEBUG

    if(StartRouteSearch()==false)
    {
        //cout<<"NA"<<endl;
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

bool RouteSearch(PATH pathTemp,LINK link)
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
    if(out_s==DEADLINES && out_ms>DEADLINEMS){
        if(path.cost<INF) return true;
        else return false;
    }
    if(binary_search(pathTemp.sortedNodeIDs.begin(),pathTemp.sortedNodeIDs.end(),link.DestinationID)==false)
    {
        if((pathTemp.cost+link.Cost)<path.cost)
        {
            if(link.DestinationID==DestinationID)
            {
                int VSize;
                VSize=V.size();
                for(int i=0;i<VSize;i++){
                    if(binary_search(pathTemp.sortedNodeIDs.begin(),pathTemp.sortedNodeIDs.end(),V[i])==false) return false;
                }
                pathTemp.cost+=link.Cost;
                pathTemp.linkIDs.push_back(link.LinkID);
                pathTemp.nodeIDs.push_back(link.DestinationID);
                path=pathTemp;
                return true;
            }
            else
            {
                pathTemp.cost+=link.Cost;
                pathTemp.linkIDs.push_back(link.LinkID);
                pathTemp.nodeIDs.push_back(link.DestinationID);
                pathTemp.sortedNodeIDs.push_back(link.DestinationID);
                sort(pathTemp.sortedNodeIDs.begin(),pathTemp.sortedNodeIDs.end());
                vector<int> interfaces;
                int VSize;
                VSize=V.size();
                for(int i=0; i<VSize; i++)
                {
                    if(binary_search(pathTemp.sortedNodeIDs.begin(),pathTemp.sortedNodeIDs.end(),V[i])==false)
                    {
                        int interface;
                        interface=SearchInterface(link.DestinationID,V[i]);//查看返回的interface到底应该怎么用？？？？？？？？？？？？？？？？？？？？？？？？？
                        if(interface==-1) return false;
                        if(binary_search(interfaces.begin(),interfaces.end(),interface)==false)
                        {
                            interfaces.push_back(interface);
                            sort(interfaces.begin(),interfaces.end());
                        }
                    }
                }
                if(interfaces.size()==0)
                {
                    int interface;
                    interface=SearchInterface(link.DestinationID,DestinationID);
                    if(interface==-1) return false;
                    interfaces.push_back(interface);
                }
                bool isReach;
                isReach=false;
                int InterfacesSize;
                InterfacesSize=interfaces.size();
                for(int i=0; i<InterfacesSize; i++)
                {
                    int linkID,cost;
                    linkID=-1;
                    cost=INF;
                    int NodeLinksSize;
                    NodeLinksSize=nodes[link.DestinationID].nodeLinks.size();
                    for(int j=0; j<NodeLinksSize; j++)
                    {
                        if(nodes[link.DestinationID].nodeLinks[j]->DestinationID==interfaces[i])
                        {
                            if(nodes[link.DestinationID].nodeLinks[j]->Cost<cost)
                            {
                                linkID=nodes[link.DestinationID].nodeLinks[j]->LinkID;
                                cost=nodes[link.DestinationID].nodeLinks[j]->Cost;
                            }
                        }
                    }
                    if(RouteSearch(pathTemp,links[linkID])==true) isReach=true;
                }
                return isReach;
            }
        }
    }
    return false;
}

bool StartRouteSearch()
{
    PATH pathTemp;
    pathTemp.cost=0;
    pathTemp.nodeIDs.push_back(SourceID);

    vector<int> interfaces;
    int VSize;
    VSize=V.size();
    for(int i=0; i<VSize; i++)
    {
        int interface;
        interface=SearchInterface(SourceID,V[i]);
        if(interface==-1) return false;
        if(binary_search(interfaces.begin(),interfaces.end(),interface)==false)
        {
            interfaces.push_back(interface);
            sort(interfaces.begin(),interfaces.end());
        }
    }

    bool isReach;
    isReach=false;
    int InterfacesSize;
    InterfacesSize=interfaces.size();
    for(int i=0; i<InterfacesSize; i++)
    {
        int linkID,cost;
        linkID=-1;
        cost=INF;
        int NodeLinksSize;
        NodeLinksSize=nodes[SourceID].nodeLinks.size();
        for(int j=0; j<NodeLinksSize; j++)
        {
            if(nodes[SourceID].nodeLinks[j]->DestinationID==interfaces[i])
            {
                if(nodes[SourceID].nodeLinks[j]->Cost<cost)
                {
                    linkID=nodes[SourceID].nodeLinks[j]->LinkID;
                    cost=nodes[SourceID].nodeLinks[j]->Cost;
                }
            }
        }
        if(RouteSearch(pathTemp,links[linkID])==true) isReach=true;
    }

    return isReach;
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

void Floyd()
{
    int nodeIDsSize;
    nodeIDsSize=allNodeIDs.size();
    for(int k=0; k<nodeIDsSize; k++)
    {
        for(int i=0; i<nodeIDsSize; i++)
        {
            for(int j=0; j<nodeIDsSize; j++)
            {
                if((D[allNodeIDs[i]][allNodeIDs[k]]+D[allNodeIDs[k]][allNodeIDs[j]])<D[allNodeIDs[i]][allNodeIDs[j]])
                {
                    D[allNodeIDs[i]][allNodeIDs[j]]=D[allNodeIDs[i]][allNodeIDs[k]]+D[allNodeIDs[k]][allNodeIDs[j]];
                    PATHD[allNodeIDs[i]][allNodeIDs[j]]=PATHD[allNodeIDs[k]][allNodeIDs[j]];
                    //PATHD[allNodeIDs[i]][allNodeIDs[j]]=allNodeIDs[k];
                }
            }
        }
    }
}

int SearchInterface(int sourceNodeID,int destinationNodeID)
{
    if(PATHD[sourceNodeID][destinationNodeID]==-1) return -1;
    int next;
    next=PATHD[sourceNodeID][destinationNodeID];
    if(next==sourceNodeID)
    {
        next=destinationNodeID;
    }
    else
    {
        while(PATHD[sourceNodeID][next]!=sourceNodeID)
        {
            next=PATHD[sourceNodeID][next];
        }
    }
    return next;
}
