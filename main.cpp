#include<iostream>
#include<thread>
#include<mutex>
#include<vector>
#include<unistd.h>

using namespace std;

void SellTickets(int ID);

vector<thread*> threads;
int tickets=100;
mutex my_mutex;

int main()
{
    for(int i=0; i<5; i++)
    {
        thread (SellTickets,i).detach();
        cout<<"ok0 "<<i<<endl;
    }
    while(true) {}


    return 0;
}

void SellTickets(int ID)
{
    while(true)
    {
        if(tickets>0)
        {
            cout<<"ID: "<<ID<<"  Sell:  No."<<tickets<<endl;
            tickets--;
        }
        else
        {
            break;
        }
    }
}
