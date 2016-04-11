#define NDEBUG
#define NDEBUG1
#define NDEBUGPATH
#define NDEBUGPATH1
#define NDEBUGINTERFACE

#define DEADLINEMS 930//930
#define DEADLINES 9//9

#include "route.h"
#include "lib/lib_record.h"
#include <stdio.h>
#include <algorithm>
#include <sys/timeb.h>
#include <memory.h>
#include <string>
//#include <queue>

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

    for(int i=0; i<edge_num; i++)
    {
        link=new LINK;
        sscanf(topo[i],"%d,%d,%d,%d",&link->LinkID,&link->SourceID,&link->DestinationID,&link->Cost);
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
    }

    //Floyd();
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

    PATH pathTemp;
    pathTemp.cost=0;
    pathTemp.nodeIDs.push_back(SourceID);

    if(StartRouteSearch(pathTemp)==false)
    {
        cout<<"NA"<<endl;
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
    /*struct timeb rawtime;
    ftime(&rawtime);
    int out_ms = rawtime.millitm - msStart;
    unsigned long out_s = rawtime.time - sStart;
    if (out_ms < 0)
    {
        out_ms += 1000;
        out_s -= 1;
    }
    //cout<<out_s<<"  "<<out_ms<<endl;
    if((out_s==DEADLINES && out_ms>DEADLINEMS)||(out_s>DEADLINES))
    {
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
                for(int i=0; i<VSize; i++)
                {
                    if(binary_search(pathTemp.sortedNodeIDs.begin(),pathTemp.sortedNodeIDs.end(),V[i])==false) return false;
                }
                pathTemp.cost+=link.Cost;
                pathTemp.linkIDs.push_back(link.LinkID);
                pathTemp.nodeIDs.push_back(link.DestinationID);
                path=pathTemp;
    #ifndef NDEBUG1
                cout<<endl<<"cost: "<<path.cost<<"    s: "<<out_s<<"  ms: "<<out_ms<<endl;
    #endif // NDEBUG1
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
                        interface=SearchInterface(link.DestinationID,V[i],pathTemp.sortedNodeIDs);//查看返回的interface到底应该怎么用？？？？？？？？？？？？？？？？？？？？？？？？？
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
                    interface=SearchInterface(link.DestinationID,DestinationID,pathTemp.sortedNodeIDs);
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
    return false;*/
    return true;
}

bool StartRouteSearch(PATH pathTemp)
{
    /*PATH pathTemp;
    pathTemp.cost=0;
    pathTemp.nodeIDs.push_back(SourceID);*/

    struct timeb rawtime;
    ftime(&rawtime);
    int out_ms = rawtime.millitm - msStart;
    unsigned long out_s = rawtime.time - sStart;
    if (out_ms < 0)
    {
        out_ms += 1000;
        out_s -= 1;
    }
    //cout<<out_s<<"  "<<out_ms<<endl;
    if((out_s==DEADLINES && out_ms>DEADLINEMS)||(out_s>DEADLINES))
    {
        if(path.cost<INF) return true;
        else return false;
    }

    vector<int> interfaces;
    bool isReach=false,isVReach=false;
    int VSize=V.size();
    for(int i=0; i<VSize; i++)
    {
        if(binary_search(pathTemp.sortedNodeIDs.begin(),pathTemp.sortedNodeIDs.end(),V[i])==false)
        {
#ifndef NDEBUGPATH1
            cout<<V[i]<<" is OK"<<endl;
            cout<<"before LINKS:";
            for(int ndp=0; ndp<pathTemp.linkIDs.size(); ndp++)
            {
                cout<<pathTemp.linkIDs[ndp]<<"-->";
            }
            cout<<endl;
            cout<<"before NODES:";
            for(int ndp=0; ndp<pathTemp.nodeIDs.size(); ndp++)
            {
                cout<<pathTemp.nodeIDs[ndp]<<"-->";
            }
            cout<<endl;
#endif // NDEBUGPATH1
            isVReach=true;
            vector<int> interface;
            interface=SearchInterface(pathTemp.nodeIDs[pathTemp.nodeIDs.size()-1],V[i],pathTemp.sortedNodeIDs);
            if(interface[0]==-1) return false;
            interface.pop_back();
            PATH myPathTemp=pathTemp;
            int iInterfaceSize=interface.size();
#ifndef NDEBUGPATH1
            cout<<"interface:";
#endif // NDEBUGPATH1
            for(int j=iInterfaceSize-1; j>-1; j--)
            {
#ifndef NDEBUGPATH1
                cout<<interface[j]<<" ";
#endif // NDEBUGPATH1
                int linkID=SearchLinkID(myPathTemp.nodeIDs[myPathTemp.nodeIDs.size()-1],interface[interface.size()-1]);
                myPathTemp.linkIDs.push_back(linkID);
                myPathTemp.cost+=links[linkID].Cost;
                if(myPathTemp.cost>path.cost) return false;
                myPathTemp.nodeIDs.push_back(interface[interface.size()-1]);
                myPathTemp.sortedNodeIDs.push_back(interface[interface.size()-1]);
                interface.pop_back();
            }
#ifndef NDEBUGPATH1
            cout<<endl;
#endif // NDEBUGPATH1
            if(myPathTemp.nodeIDs[myPathTemp.nodeIDs.size()-1]!=V[i])
            {
                int linkID=SearchLinkID(myPathTemp.nodeIDs[myPathTemp.nodeIDs.size()-1],V[i]);
                myPathTemp.linkIDs.push_back(linkID);
                myPathTemp.cost+=links[linkID].Cost;
                if(myPathTemp.cost>path.cost) return false;
                myPathTemp.nodeIDs.push_back(V[i]);
                myPathTemp.sortedNodeIDs.push_back(V[i]);
            }
            sort(myPathTemp.sortedNodeIDs.begin(),myPathTemp.sortedNodeIDs.end());
#ifndef NDEBUGPATH1
            cout<<"after  LINKS:";
            for(int ndp=0; ndp<myPathTemp.linkIDs.size(); ndp++)
            {
                cout<<myPathTemp.linkIDs[ndp]<<"-->";
            }
            cout<<endl;
            cout<<"after  NODES:";
            for(int ndp=0; ndp<myPathTemp.nodeIDs.size(); ndp++)
            {
                cout<<myPathTemp.nodeIDs[ndp]<<"-->";
            }
            cout<<endl<<endl;
#endif // NDEBUGPATH1
            bool isReachTemp= StartRouteSearch(myPathTemp);
            if(isReach==false && isReachTemp==true) isReach=true;
        }
        else
        {
#ifndef NDEBUGPATH1
            cout<<V[i]<<" is OUT"<<endl;
#endif // NDEBUGPATH1
        }
    }
    if(isVReach==false)
    {
        vector<int> interface;
        interface=SearchInterface(pathTemp.nodeIDs[pathTemp.nodeIDs.size()-1],DestinationID,pathTemp.sortedNodeIDs);
        if(interface[0]==-1) return false;
        interface.pop_back();
        int iInterfaceSize=interface.size();
        for(int j=iInterfaceSize-1; j>-1; j--)
        {
            int linkID=SearchLinkID(pathTemp.nodeIDs[pathTemp.nodeIDs.size()-1],interface[interface.size()-1]);
            pathTemp.linkIDs.push_back(linkID);
            pathTemp.cost+=links[linkID].Cost;
            pathTemp.nodeIDs.push_back(interface[interface.size()-1]);
            interface.pop_back();
        }
        if(pathTemp.nodeIDs[pathTemp.nodeIDs.size()-1]!=DestinationID)
        {
            int linkID=SearchLinkID(pathTemp.nodeIDs[pathTemp.nodeIDs.size()-1],DestinationID);
            pathTemp.linkIDs.push_back(linkID);
            pathTemp.cost+=links[linkID].Cost;
            if(pathTemp.cost>path.cost) return false;
            pathTemp.nodeIDs.push_back(DestinationID);
        }
        if(pathTemp.cost<path.cost)
        {
            path=pathTemp;
            return true;
        }
        else return false;
    }
    return isReach;


    /*int InterfacesSize;
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

    return isReach;*/
}

int SearchLinkID(int iSourceID,int iDestinationID)
{
    //int linkID=-1,cost=INF;
    int linkID=-1;
    vector<LINK*> linksTemp=nodes[iSourceID].nodeLinks;
    sort(linksTemp.begin(),linksTemp.end(),CompareLinkCost);
    int NodeLinksSize=linksTemp.size();
    for(int j=0; j<NodeLinksSize; j++)
    {
        if(linksTemp[j]->DestinationID==iDestinationID)
        {
            //cout<<"linkID = "<<linksTemp[j]->LinkID<<endl;
            return linksTemp[j]->LinkID;
        }
    }
    /*int NodeLinksSize=nodes[iSourceID].nodeLinks.size();
    for(int j=0; j<NodeLinksSize; j++)
    {
        if(nodes[iSourceID].nodeLinks[j]->DestinationID==iDestinationID)
        {
            if(nodes[iSourceID].nodeLinks[j]->Cost<cost)
            {
                linkID=nodes[iSourceID].nodeLinks[j]->LinkID;
                cost=nodes[iSourceID].nodeLinks[j]->Cost;
            }
        }
    }*/
   cout<<"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx linkID = "<<linkID<<endl;
    return linkID;
}

bool IsInV(int ID)
{
    return binary_search(V.begin(),V.end(),ID);
}

bool CompareLinkCost(const LINK *a,const LINK *b)
{
    return a->Cost < b->Cost;
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

vector<int> SearchInterface(int sourceNodeID,int destinationNodeID,vector<unsigned short> sortedNodeIDs)
{
#ifndef  NDEBUGINTERFACE
    cout<<"start0 searchinterface"<<endl;
#endif // NDEBUGINTERFACE
    int nodeIDsSize;
    nodeIDsSize=allNodeIDs.size();
    int d[NODESNUM],pathd[NODESNUM][NODESNUM];
    bool final[NODESNUM];//若final[i] = 1则说明 顶点vi已在集合S中
    //memset(d,INF,NODESNUM*sizeof(int));
    memcpy(d,D[sourceNodeID],sizeof(D[sourceNodeID]));
    //memset(pathd,-1,NODESNUM*NODESNUM*sizeof(int));//设空路径
    memcpy(pathd,PATHD,sizeof(PATHD));
    memset(final,false,sizeof(final));
#ifndef  NDEBUGINTERFACE
    cout<<"start1"<<endl;
#endif // NDEBUGINTERFACE
    /*for(int i=0; i<nodeIDsSize; i++)//循环 初始化
    {
        //d[allNodeIDs[i]]=D[sourceNodeID][allNodeIDs[i]];
        if(d[allNodeIDs[i]]<INF && allNodeIDs[i]!=sourceNodeID)
        {
            pathd[sourceNodeID][allNodeIDs[i]]=sourceNodeID;
        }
    }*/
    d[sourceNodeID]=0;
    final[sourceNodeID]=true;//初始化 v0顶点属于集合S
    int sortedNodeSize;
    sortedNodeSize=sortedNodeIDs.size();
    for(int i=0; i<sortedNodeSize; i++)
    {
        final[sortedNodeIDs[i]]=true;
    }
    if(destinationNodeID!=DestinationID) final[DestinationID]=true;
#ifndef  NDEBUGINTERFACE
    cout<<"start2"<<endl;
#endif // NDEBUGINTERFACE

    //开始主循环 每次求得v0到某个顶点v的最短路径 并加v到集合S中
    for (int i = 1; i < nodeIDsSize; i++)//求n-1条最短路径
    {
        //选点
        int min;
        min=INF;
        int k;
        k=-1;
        for (int w = 1; w < nodeIDsSize; w++)
        {
            if (final[allNodeIDs[w]]==false && d[allNodeIDs[w]]<min) //如果w顶点在V-S中
            {
                //这个过程最终选出的点 应该是选出当前V-S中与S有关联边
                //且权值最小的顶点 书上描述为 当前离V0最近的点
                min=d[allNodeIDs[w]];
                k=w;
            }
        }
        final[allNodeIDs[k]]=true;//选出该点后加入到合集S中
        for (int j = 0; j < nodeIDsSize; j++)//更新当前最短路径和距离
        {
            /*在此循环中 v为当前刚选入集合S中的点
            则以点V为中间点 考察 d0v+dvw 是否小于 D[w] 如果小于 则更新
            比如加进点 3 则若要考察 D[5] 是否要更新 就 判断 d(v0-v3) + d(v3-v5) 的和是否小于D[5]
            */
            if (final[allNodeIDs[j]]==false && (d[allNodeIDs[k]]+D[allNodeIDs[k]][allNodeIDs[j]]<d[allNodeIDs[j]]))
            {
                d[allNodeIDs[j]] = d[allNodeIDs[k]] + D[allNodeIDs[k]][allNodeIDs[j]];
                pathd[sourceNodeID][allNodeIDs[j]] = allNodeIDs[k];
                //pathd[sourceNodeID][allNodeIDs[j]] = pathd[sourceNodeID][allNodeIDs[k]];
            }
        }
    }
#ifndef  NDEBUGINTERFACE
    cout<<"start3"<<endl;
#endif // NDEBUGINTERFACE
    vector<int> next;
#ifndef  NDEBUGINTERFACE
    cout<<"start4  "<<"sourceNodeID="<<sourceNodeID<<" destinationNodeID="<<destinationNodeID<<endl;
#endif // NDEBUGINTERFACE
    if(pathd[sourceNodeID][destinationNodeID]==-1)
    {
        next.push_back(-1);
        return next;
    }

    int v=destinationNodeID;
    while(v!=sourceNodeID)
    {
        next.push_back(v);
        v=pathd[sourceNodeID][v];
    }
    next.push_back(v);

#ifndef  NDEBUGINTERFACE
    cout<<"start5"<<endl;
#endif // NDEBUGINTERFACE
    //next=pathd[sourceNodeID][destinationNodeID];

    //next.push_back(pathd[sourceNodeID][destinationNodeID]);////////////////////////////////////

#ifndef  NDEBUGINTERFACE
    cout<<"start6"<<endl;
#endif // NDEBUGINTERFACE
    //if(next==sourceNodeID)
    /////////////////////////////////////////////////////////////////
    /*if(next[0]==sourceNodeID)
    {
        //next=destinationNodeID;
        next[0]=destinationNodeID;

    }
    else
    {
        while(pathd[sourceNodeID][next[next.size()-1]]!=sourceNodeID)
        {
            //next=pathd[sourceNodeID][next];
            if(pathd[sourceNodeID][next[next.size()-1]]==-1)
            {
                next[0]=-1;
                return next;
            }
            else
            {
                next.push_back(pathd[sourceNodeID][next[next.size()-1]]);
            }
        }
    }*/
#ifndef  NDEBUGINTERFACE
    cout<<"end serchinterface"<<endl;
#endif // NDEBUGINTERFACE
    return next;
}
