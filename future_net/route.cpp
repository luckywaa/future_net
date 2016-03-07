#include "route.h"
#include "lib_record.h"
#include <stdio.h>

#include <iostream>
#include <vector>

using namespace std;

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

PATH path,pathTemp;
vector<LINK> links;
LINK *link;
vector<NODE> nodes(700);
unsigned short SourceID,DestinationID;
vector<unsigned short> V;//is V' . And we can sort it,so when we use ,we can save time.

bool DeepSearch(NODE &node,LINK *link);
bool StartDeepSearch();

//你要完成的功能总入口
void search_route(char *graph[5000], int edge_num, char *condition)
{

    for(int i=0;; i++)//input graph
    {
        int num,countNum;
        num=0;
        countNum=0;
        for(int j=0;; j++)
        {
            if(graph[i][j]>='0' && graph[i][j]<='9')
            {
                num=num*10+graph[i][j]-'0';
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
    for(int i=0,num=0,countNum=0;; i++)//input condition
    {
        if(condition[i]>='0' && condition[i]<='9')
        {
            num=num*10+condition[i]-'0';
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
        if(condition[i]=='\n') break;
    }
    if(StartDeepSearch()==false)
    {
        cout<<"NA"<<endl;
    }
    else
    {
        for(int i=0; i<(int)path.linkIDs.size(); i++)
        {
            record_result((unsigned short)path.linkIDs[i]);
        }
    }

    /*unsigned short result[] = {2, 6, 3};//示例中的一个解
    for (int i = 0; i < 3; i++)
        record_result(result[i]);*/
}
NODE::NODE()
{
    isVisted=false;
}
PATH::PATH()
{
    cost=9999999;
}
bool DeepSearch(NODE &node,LINK *link)
{
    if(node.isVisted==false)
    {
        if((pathTemp.cost+link->Cost)<path.cost)
        {
            pathTemp.linkIDs.push_back(link->LinkID);
            pathTemp.nodeIDs.push_back(node.ID);
            pathTemp.cost+=link->Cost;
            if(node.ID!=DestinationID)
            {
                node.isVisted=true;
                bool isFind;
                isFind=false;
                for(int i=0; i<(int)node.nodeLinks.size(); i++)
                {
                    if(DeepSearch(nodes[node.nodeLinks[i]->DestinationID],node.nodeLinks[i])==true) isFind=true;
                }
                pathTemp.linkIDs.erase(--pathTemp.linkIDs.end());
                pathTemp.nodeIDs.erase(--pathTemp.nodeIDs.end());
                pathTemp.cost-=link->Cost;
                node.isVisted=false;
                return isFind;
            }
            else
            {
                bool isVVisted;
                isVVisted=false;
                for(int i=0; i<(int)V.size(); i++)
                {
                    bool isJVisted;
                    isJVisted=false;
                    for(int j=0; j<(int)pathTemp.nodeIDs.size(); j++)
                    {
                        if(V[i]==pathTemp.nodeIDs[j])
                        {
                            isJVisted=true;
                            break;
                        }
                    }
                    if(isJVisted==false) break;
                    else if(i==(int)(V.size()-1))
                    {
                        isVVisted=true;
                        break;
                    }
                }
                if(isVVisted==true)
                {
                    path=pathTemp;
                }
                pathTemp.linkIDs.erase(--pathTemp.linkIDs.end());
                pathTemp.nodeIDs.erase(--pathTemp.nodeIDs.end());
                pathTemp.cost-=link->Cost;
                node.isVisted=false;
                return isVVisted;
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}
bool StartDeepSearch()
{
    pathTemp.nodeIDs.push_back(SourceID);
    pathTemp.cost=0;
    nodes[SourceID].isVisted=true;
    bool isFind;
    isFind=false;
    for(int i=0; i<(int)nodes[SourceID].nodeLinks.size(); i++)
    {
        if(DeepSearch(nodes[nodes[SourceID].nodeLinks[i]->DestinationID],nodes[SourceID].nodeLinks[i])==true) isFind=true;
    }
    return isFind;
}
