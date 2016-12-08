#include"Hieron_local.h"
#include<sys/time.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<cstring>
#include<sstream>
#include<fstream>
#include<list>
#include<cmath>
bool ** Hieron_local::conn_array; // store the connectivity of the local networkl; set by readTopo
double ** Hieron_local::band_array;//store the band array of the local network; set by readTopo
int ** Hieron_local::delay_array;//store the delay array of the local network; set by readTopo
vector<int> Hieron_local::marginal_nodes;//store the virtual nodes of the local network; set by readTopo
int Hieron_local::domainID;//store the domain id of the local network; set in the constructor
string Hieron_local::local_ip;
int Hieron_local::local_port;
string Hieron_local::root_ip;
int Hieron_local::root_port;
int Hieron_local::root_socket;
string Hieron_local::centralized_ip;
int Hieron_local::centralized_port;
int Hieron_local::centralized_socket;
int Hieron_local::switchCount;//store the number of the switches in this network; set by readTopo
vector<edge> Hieron_local::edgeList;//store the edge message;set by readTopo
vector<node> Hieron_local::nodeList;//store the node message;set by readTopo
vector<int> Hieron_local::switchID;//store the switchID of each switch in this domain;set by readTopo
vector<int> Hieron_local::switchID1;//of domain1
vector<int> Hieron_local::switchID2;
vector<int> Hieron_local::switchID3;
vector<int> Hieron_local::switchID4;
vector<int> Hieron_local::switchID5;
vector<int> Hieron_local::allSwitchID;
map<int,int> Hieron_local::index_switch;//store the name(4096,7865..) of the switch with the index id(0,1,2,3..) in the local network;set by readTopo
map<int,int> Hieron_local::switch_index;//store the reverse as index_switch
vector<domain_requirement> Hieron_local::domain_requirement_list;//store the requirement of this domain
vector<host_require> Hieron_local::host_require_list;//store the requirement of each host require in this domain
vector<host_require_actuality> Hieron_local::virtual_link_list;
vector<host_require_actuality> Hieron_local::host_require_actuality_list; //store the actuality requirement of each host in this domain
vector<host_require_actuality> Hieron_local::last_TE_require_allocated_list; //store the require allocated in last TE both in local and root 
vector<host_require_actuality> Hieron_local::this_time_require_in_last_TE_predict; //store the require allocated in last TE both in local and root 
vector<domain_requirement_transmission> Hieron_local::merged_require;
vector<pair<int,int> > Hieron_local::delay_band_list;//store the delay and bandwidth of the virtual link
float Hieron_local::poisson_mean;
float Hieron_local::pareto_alpha;
float Hieron_local::pareto_xm;
float Hieron_local::percent_virtual_link;
int Hieron_local::flow_amount_devide_band;
int Hieron_local::mergeTimes;
bool Hieron_local::socketID[100];
int Hieron_local::condition;
int Hieron_local::conut_inter_domain_requirement;
int Hieron_local::num_inter_domain_requirement_receive;
double Hieron_local::rate_inter_domain_requirement;
vector<double> Hieron_local::usage;
vector<double> Hieron_local::usage_mean;
vector<double> Hieron_local::usage_link;
bool ** Hieron_local::conn_array_copy;//store the copy of the conn_array
double ** Hieron_local::band_array_copy;//store the copy of the band_array
bool Hieron_local::first_print = true;
bool Hieron_local::afterMerge = false;
pthread_mutex_t Hieron_local::mergelock;
pthread_mutex_t Hieron_local::host_requirement_lock;
vector<host_require_actuality> Hieron_local::intra_require_list;
vector<host_require_actuality> Hieron_local::inter_require_list;
vector<host_require_actuality> Hieron_local::all_require_to_centralized;
string Hieron_local::TE_time_local;
int Hieron_local::count_TE;
long Hieron_local::all_start_time;
long Hieron_local::all_end_time;
ofstream Hieron_local::output_error_log;
vector<int*> Hieron_local:: path_send_to_centralized;
vector<int> Hieron_local::path_length_send_to_centralized;
vector<host_require_actuality> Hieron_local::intra_require_left;
int Hieron_local::count_inter_sent_to_root;

Hieron_local::Hieron_local(int domID)
{
	domainID = domID;
	switchCount = 0;
}
void Hieron_local::readTopo()
{
	int _size = -1;//the size of switches in the network
	/*************store the switch id of the local network**************/
	ifstream infile1;
	ifstream infile_marginal_nodes;
	int SID1;
	switch(domainID)
	{
		case 1 : {
 				infile1.open("Topo1_with_bandwidth");
				infile_marginal_nodes.open("marginalNode1");
		//		cout<<"Topo1_with_bandwidth has been open"<<endl;
				break;
 
			}
		case 2 : {
 				infile1.open("Topo2_with_bandwidth");
				infile_marginal_nodes.open("marginalNode2");
		//		cout<<"Topo2_with_bandwidth has been open"<<endl;
				break;
			}
		case 3 : {
 				infile1.open("Topo3_with_bandwidth");
				infile_marginal_nodes.open("marginalNode3");
		//		cout<<"Topo3_with_bandwidth has been open"<<endl;
				break;
			}
		case 4 : {
 				infile1.open("Topo4_with_bandwidth");
				infile_marginal_nodes.open("marginalNode4");
		//		cout<<"Topo3_with_bandwidth has been open"<<endl;
				break;
			}
		case 5 : {
 				infile1.open("Topo5_with_bandwidth");
				infile_marginal_nodes.open("marginalNode5");
		//		cout<<"Topo3_with_bandwidth has been open"<<endl;
				break;
			}
	}
	if(infile_marginal_nodes.is_open())
	{
		while(infile_marginal_nodes.good()&& !infile_marginal_nodes.eof())
		{
			int marginalNode;
			char buffer[1024];
			infile_marginal_nodes.getline(buffer,1024);
			string message;
			message = buffer;
		//	cout<<"message is "<<message<<endl;
			istringstream ss(message);
			ss>>marginalNode;
			
			marginal_nodes.push_back(marginalNode);
		}
	}
	marginal_nodes.pop_back();
	/*
	vector<int>::iterator ii = marginal_nodes.begin();
	while(ii != marginal_nodes.end())
	{
		cout<<*ii<<endl;
		ii++;
	}
	*/
	if(infile1.is_open())
	{
		while(infile1.good() && !infile1.eof())
		{
			_size++;
			char buffer[1024];
			infile1.getline(buffer,1024);
			string message;
			message = buffer;
		//	cout<<"message is "<<message<<endl;
			istringstream ss(message);
			ss>>SID1;
		//	cout<<"index is "<<_size<<" SID is "<<SID1<<endl;
			switch_index[SID1] = _size;
			index_switch[_size] = SID1;
			switchID.push_back(SID1);

		}
	}
	switchID.pop_back();
	map<int,int>::iterator it = index_switch.begin();
	while(it != index_switch.end())
	{
		if(it->first == _size)
		{
			index_switch.erase(it);
		}
		it++;
	}
	switch_index[SID1] = _size-1;
//	cout<<" hh "<<switch_index[SID1]<<endl;
//	cout<<index_switch[_size-1]<<endl;

//	cout<<"_size is "<<_size<<endl;
//	Initialize the size of switchCount,the three arraies:conn_array,band_array,delay_array
	switchCount = _size;
	conn_array = new bool *[switchCount];
	band_array = new double *[switchCount];
	delay_array = new int *[switchCount];
	conn_array_copy = new bool *[switchCount];
	band_array_copy = new double *[switchCount];
	for(int i=0; i < switchCount; i++)
	{
		conn_array[i] = new bool[switchCount]();
		band_array[i] = new double[switchCount]();
		delay_array[i] = new int[switchCount]();
		conn_array_copy[i] = new bool[switchCount]();
		band_array_copy[i] = new double[switchCount]();
	}
	/*
	for(int i = 0; i<switchCount; i++)
		for(int j = 0; j < switchCount; j++)
		{
			conn_array[i][j] = false;
			band_array[i][j] = 0;
			delay_array[i][j] = 0;
			conn_array_copy[i][j] = 0;
			band_array_copy[i][j] = 0;
		}
	*/
	ifstream infile;
	switch(domainID)
	{
		case 1 : {
				infile.open("Topo1_with_bandwidth");
			//	cout<<"Topo1_with_bandwidth has been open"<<endl;
				break;
			}
		case 2 : {
				infile.open("Topo2_with_bandwidth");
		   //	cout<<"Topo2_with_bandwidth has been open"<<endl;
				break;
			}
		case 3 : {
				infile.open("Topo3_with_bandwidth");
			//	cout<<"Topo3_with_bandwidth has been open"<<endl;
				break;
			}
		case 4 : {
				infile.open("Topo4_with_bandwidth");
			//	cout<<"Topo3_with_bandwidth has been open"<<endl;
				break;
			}
		case 5 : {
				infile.open("Topo5_with_bandwidth");
			//	cout<<"Topo3_with_bandwidth has been open"<<endl;
				break;
			}
	}
	int SID;
	int count = 0;
	if(infile.is_open())
	{
		while(infile.good() && !infile.eof())
		{
			if(count == _size)break;
			char buffer[1024];
			string message;
			infile.getline(buffer,1024);
			message = buffer;
		//	cout<<"message is "<<message<<endl;
			istringstream ss(message);
			ss>>SID;
		//	switch_index[SID] = count;
		//	index_switch[count] = SID;
			count++;
			node tempNode;
			tempNode.sid = SID;
			do
			{
				string stemp;
				ss>>stemp;
			//	cout<<"stemp is "<<stemp<<endl;
				if(stemp[0] == '<')
				{
					int sid2;
					int band;
					int delay;
					string sband;
					ss>>sband;
					stringstream ss_sid2;

					ss_sid2<<stemp.substr(1,strlen(stemp.c_str())-2);
					ss_sid2>>sid2;
					if(sband[0] != '(')
						cout<<"sband error!"<<endl;
					stringstream ss_band;
					ss_band<<sband.substr(1,strlen(sband.c_str())-2);
					ss_band>>band;

					ss>>sband;
					if(sband[0] != '{')
						cout<<"sband delay error!"<<endl;
					stringstream ss_delay;
					ss_delay<<sband.substr(1,strlen(sband.c_str())-2);
					ss_delay>>delay;

					tempNode.neighbour.push_back(sid2);
					edge tempEdge;
					tempEdge.sid1 = SID;
					tempEdge.sid2 = sid2;
					tempEdge.band = band;
					tempEdge.delay = delay;

					map<int,int>::iterator iter1 = switch_index.find(SID);
					map<int,int>::iterator iter2 = switch_index.find(sid2);
					if(iter1 != switch_index.end() && iter2 != switch_index.end())
					{
						int index1 = iter1->second;
						int index2 = iter2->second;
					//	cout<<"SID is "<<SID<<" "<<"index1 is "<<index1<<endl;
					//	cout<<"sid2 is "<<sid2<<" "<<"index2 is "<<index2<<endl;
					//	cout<<"band is "<<band<<endl;
						if(band < 0)
						{
							printf("**********band should not < 0**********\n\n");
						}
						band_array[index1][index2] = band;
						conn_array[index1][index2] = true;
						band_array[index2][index1] = band;
						conn_array[index2][index1] = true;
						delay_array[index1][index2] = delay;
						delay_array[index2][index1] = delay;
					}

					edgeList.push_back(tempEdge);
				}
			}while(ss);
			nodeList.push_back(tempNode);
		}
	}
	infile.close();
	for(int i = 0; i < switchCount; i++)
		for(int j = 0; j < switchCount; j++)
		{
			conn_array_copy[i][j] = conn_array[i][j];
			band_array_copy[i][j] = band_array[i][j];
		}
//	memcpy(conn_array_copy,conn_array,switchCount*switchCount);
//	memcpy(band_array_copy,band_array,switchCount*switchCount*4);
	cout<<"-------------after read topo---------------"<<endl;
	/***********display the conn_array************/
	/*
	for(int i=0; i < switchCount; i++)
	{
		for(int j =0; j < switchCount; j++)
			cout<<conn_array[i][j]<<" ";
		cout<<endl;
	}
	*/
	/**************display the delay_array****************/
	/*
	for(int i=0; i < switchCount; i++)
		if(conn_array[switchCount-1][i])
		{
			cout<<"the message between "<<switchCount-1<<" and "<<i<<" is : band "<<band_array[switchCount-1][i]<<" delay "<<delay_array[switchCount-1][i]<<endl;
		}
	*/
	/********display the node of the local network*********/
	/*	
	cout<<"display the node of the local network"<<endl;
	vector<node>::iterator iter = nodeList.begin();
	while(iter != nodeList.end())
	{
		cout<<iter->sid;
		vector<int>::iterator iter1 = iter->neighbour.begin();
		while(iter1 != iter->neighbour.end())
		{
			cout<<" "<<*iter1;
				iter1++;
		}
		cout<<endl;
		iter++;
	}
	*/
	/***********display the map of node and index **************/
	/*
	cout<<"display the map of node and index"<<endl;
	map<int,int>::iterator iter_map = switch_index.begin();
	while(iter_map != switch_index.end())
	{
		cout<<iter_map->first<<" "<<iter_map->second<<endl;
		iter_map++;
	}
	*/
	/**************display the map of index and node****************/
/*	
	map<int,int>::iterator iter_map1 = index_switch.begin();
	while(iter_map1 != index_switch.end())
	{
		cout<<iter_map1->first<<" "<<iter_map1->second<<endl;
		iter_map1++;
	}
*/	
}

void Hieron_local::getOtherDomain()
{
	/**********store the nodes of domain1**********/
	ifstream infile;
	infile.open("Topo1_with_bandwidth");
	if(infile.is_open())
	{
		while(infile.good()&& !infile.eof())
		{
			int nodeID;
			char buffer[1024];
			infile.getline(buffer,1024);
			string message;
			message = buffer;
		//	cout<<"message is "<<message<<endl;
			istringstream ss(message);
			ss>>nodeID;
			switchID1.push_back(nodeID);
			allSwitchID.push_back(nodeID);
		}
	}
	switchID1.pop_back();
	allSwitchID.pop_back();
	infile.close();

	/*************store the nodes of domain2***********/
	infile.open("Topo2_with_bandwidth");
	if(infile.is_open())
	{
		while(infile.good()&& !infile.eof())
		{
			int nodeID;
			char buffer[1024];
			infile.getline(buffer,1024);
			string message;
			message = buffer;
		//	cout<<"message is "<<message<<endl;
			istringstream ss(message);
			ss>>nodeID;
			switchID2.push_back(nodeID);
			allSwitchID.push_back(nodeID);
		}
	}
	switchID2.pop_back();
	allSwitchID.pop_back();
	infile.close();

	/*************store the nodes of domain3***********/
	infile.open("Topo3_with_bandwidth");
	if(infile.is_open())
	{
		while(infile.good()&& !infile.eof())
		{
			int nodeID;
			char buffer[1024];
			infile.getline(buffer,1024);
			string message;
			message = buffer;
		//	cout<<"message is "<<message<<endl;
			istringstream ss(message);
			ss>>nodeID;
			switchID3.push_back(nodeID);
			allSwitchID.push_back(nodeID);
		}
	}
	switchID3.pop_back();
	allSwitchID.pop_back();
	infile.close();

	/*************store the nodes of domain4***********/
	infile.open("Topo4_with_bandwidth");
	if(infile.is_open())
	{
		while(infile.good()&& !infile.eof())
		{
			int nodeID;
			char buffer[1024];
			infile.getline(buffer,1024);
			string message;
			message = buffer;
		//	cout<<"message is "<<message<<endl;
			istringstream ss(message);
			ss>>nodeID;
			switchID4.push_back(nodeID);
			allSwitchID.push_back(nodeID);
		}
	}
	switchID4.pop_back();
	allSwitchID.pop_back();
	infile.close();

	/*************store the nodes of domain5***********/
	infile.open("Topo5_with_bandwidth");
	if(infile.is_open())
	{
		while(infile.good()&& !infile.eof())
		{
			int nodeID;
			char buffer[1024];
			infile.getline(buffer,1024);
			string message;
			message = buffer;
		//	cout<<"message is "<<message<<endl;
			istringstream ss(message);
			ss>>nodeID;
			switchID5.push_back(nodeID);
			allSwitchID.push_back(nodeID);
		}
	}
	switchID5.pop_back();
	allSwitchID.pop_back();
	infile.close();
	
	cout<<"switchID1 has "<<switchID1.size()<<" elements"<<endl;
	cout<<"switchID2 has "<<switchID2.size()<<" elements"<<endl;
	cout<<"switchID3 has "<<switchID3.size()<<" elements"<<endl;
	cout<<"switchID4 has "<<switchID4.size()<<" elements"<<endl;
	cout<<"switchID5 has "<<switchID5.size()<<" elements"<<endl;
	cout<<"Total switch has "<<allSwitchID.size()<<endl;
	
}


int Hieron_local::get_switchCount()
{
	return switchCount;
}

int Hieron_local::dijkstra(int start,int end,int * path,int delay,int& delay_path)
{
//	for(int i =0; i<switchCount;i++)
/*	{
		for(int j =0;j<switchCount;j++)
			cout<<delay_array[start][j]<<" ";
		cout<<endl;
	}*/
	/*
	for(int i =0; i<switchCount;i++)
	{
		for(int j =0;j<switchCount;j++)
			cout<<conn_array[i][j]<<" ";
		cout<<endl;
	}*/

	//cout<<"in dijkstra "<<start<<"->"<<end<<" : "<<delay<<endl;
	int* dist = new int[switchCount];
	bool * used = new bool[switchCount];
	int delay_count = 0;
	//path has been initialized in the TE function
	for(int i =0;i<switchCount;i++)
		path[i] = -1;

	for(int i = 0; i < switchCount; i++)
		used[i] = false;
	used[start] = true;

	unsigned int INFINITY0 = 0xFFFFFFF;

	for(int i = 0; i < switchCount; i++)
	{
		if(conn_array[start][i])
		{
			dist[i] = delay_array[start][i];
			path[i] = start;
		}
		else dist[i] = INFINITY0;
	}
	/*if(conn_array[start][end])
	{
		path[end] = start;
		return 0;
	}*/
	int count = 1;
	while(count<switchCount)
	{
		int min_dist = INFINITY0;
		int min_index = -1;
		for(int i =0; i<switchCount; i++)
		{
			if(used[i])continue;
			//cout<<"dist[i] is "<<dist[i]<<" min_dist is "<<min_dist<<endl;
			if(dist[i] < min_dist)
			{
				min_dist = dist[i];
				min_index =  i;
			}
		}
	//	cout<<"min index is "<<min_index<<endl;
		if(min_dist == INFINITY0 || min_index == -1)
		{
			cout<<"Dijkstra : Can not find a trace from "<<start<<" to "<<end<<endl;
			return -1;
		}
		/*if(min_index == end)
		{
		//	cout<<"?"<<endl;
			if(conn_array[start][end])
			{
				if(delay_array[start][end] > delay)
				{
					cout<<"Dijkstra : delay can not meet!"<<endl;
				}
				else
				{
					printf("delay between %d and %d is %f\n",start,end,delay_array[start][end]);
					return 1;
				}
			}
			else
			{
				cout<<"Dijkstra : the path between start and end can not be found!"<<endl;
				return -1;
			}
		}
		else*/
		{
		//	cout<<"in else"<<endl;
			used[min_index] = true;
			for(int j = 0; j <  switchCount; j++)
			{
				if(used[j])continue;
				if(conn_array[min_index][j] && dist[min_index] + delay_array[min_index][j] < dist[j])
				{
					path[j] = min_index;
					dist[j] = dist[min_index] + delay_array[min_index][j];
				//	cout<<"j is "<<j<<" dist[j] is "<<dist[j]<<" path[j] is "<<path[j]<<endl;

				}
			}

		}
		if(min_index == end)break;
		
		count++;
	}
	int result=0;
	if(path[end] == -1)result = -1;
	else
	{
		int temp = end;
		while(temp != start)
		{
			delay_count += delay_array[temp][path[temp]];
			if(delay_array[temp][path[temp]] > 100)
				{
					printf("from %d to %d,the delay is %d\n",index_switch[temp],index_switch[path[temp]],delay_array[temp][path[temp]]);
				}
			temp = path[temp];
		}
		if(delay < delay_count)
		{
			cout<<"delay is "<<delay<<" delay_count is "<<delay_count<<endl;
			cout<<"Dijkstra : delay can not be met"<<endl;
			result = -1;
		}
		else
		{
			delay_path = delay_count;
		}
	//	if(result != -1)cout<<"start is "<<start<<" end is "<<end<<" ; delay is "<<delay<<" ; delay_count is "<<delay_count<<endl;

	}
	if(result != -1)
	{
		int e = end;
		/*cout<<"----------------The path is---------"<<endl;
		while(e != start)
		{
			cout<<e<<" ";
			e = path[e];

		}
		cout<<start<<endl<<endl;*/
		e = end;
		while(e != start)
		{
			if(!conn_array[e][path[e]])
				printf("%d can not connect to %d\n",e,path[e]);
			//else 
			//	printf("%d can connect to %d\n",e,path[e]);

			if(!conn_array[path[e]][e])
				printf("%d can not connect to %d\n",path[e],e);
			//else 
			//	printf("%d can connect to %d\n",path[e],e);
			e = path[e];

		}
	}
	delete[] used;
	delete[] dist;
	return result;
	
}

void Hieron_local::TE_fun(vector<host_require_actuality> &host_require_actuality_list)
{
	
	double delta = 0.01;
	double x = 0;
//	cout<<"host require size is "<<host_require_actuality_list.size()<<endl;
	/*
	cout<<"before allocating, the band array is "<<endl;
	for(int i =0 ;i < _size; i++)
	{
		for(int j = 0; j<_size; j++)
			cout<<band_array[i][j]<<" ";
		cout<<endl;
	}
	*/
	/*struct switch_pair_index
	{
		int sid1;
		int sid2;
		int index;
		switch_pair_index():sid1(0),sid2(0),index(-1){};
		switch_pair_index(int s1,int s2,int ind):sid1(s1),sid2(s2),index(ind){};
	};
*/
	/********path array is used to store the shortes path of requirement i******/
//	vector<int> path;
//	calculate the shortest path of each switch sid pair
	vector<int *> paths;
	vector<int>delays;
	vector<switch_pair_index> switch_pairs;
	vector<host_require_actuality>::iterator iter_require_list = host_require_actuality_list.begin();
	int count = 0;
	int total_path = 0;
	bool has_path = false;
	while(iter_require_list != host_require_actuality_list.end())
	{
		int start = iter_require_list->src_sid;
		int end = iter_require_list->dst_sid;
		/*As the src and dst sid is the index in the array
		map<int,int>::const_iterator iter1,iter2;
		iter1 = switch_index.find(iter_require_list->src_sid);
		iter2 = switch_index.find(iter_require_list->dst_sid);
		int start,end;
		if(iter1 != switch_index.end() && iter2 != switch_index.end())*/
		{
		//	start = iter1->second;
		//	end = iter2->second;
			int *path = new int[switchCount];
			int result;
			int delay = iter_require_list->delay_require;
			int delay_path;
			result = dijkstra(start,end,path,delay,delay_path);
			if(iter_require_list->band_require < 0.000000001 || result < 0 )
			{
			//	printf("in initial, can not find the path %d -> %d\n",iter_require_list->src_sid,iter_require_list->dst_sid);
				iter_require_list->finished = true;//In this TE,the require not need to compute
				switch_pair_index spi(iter_require_list->src_sid,iter_require_list->dst_sid,count,delay,false);
				spi.finished = true;
				switch_pairs.push_back(spi);
			/*	paths.push_back(path);
				count++;
				iter_require_list++;
				continue;*/
			}
			else
			{
				if(result >= 0)
				{
					has_path = true;
				/*	cout<<"hah path is : "<<endl;
					int m = start;
					int n = end;
					while(n != m)
					{
						cout<<n<<" ";
						n = path[n];
					}
					cout<<m<<endl;*/
				//	iter_require_list->latest_path_delay = delay_path;
				}
				switch_pair_index spi(iter_require_list->src_sid,iter_require_list->dst_sid,count,delay,false);
				switch_pairs.push_back(spi);
			}
			paths.push_back(path);
			delays.push_back(delay_path);
			count++;
		}
		/*
		else
		{
			cout<<"can not find the index of "<<iter1->first<<" or "<<iter2->first<<endl;
		}*/
		iter_require_list++;
	}
	if(!has_path)
	{
		cout<<"There is no path at the original state of all requirement"<<endl;
		return;
	}
	total_path = count;
	/*********allocate band*********/
	double total = 0;
	while(true)
	{
		x = delta;
		bool continue_ = false;
		vector<host_require_actuality>::iterator iter_require_list = host_require_actuality_list.begin();
		/********find if need to compute the shortest path of each switch pair*********/
		vector<switch_pair_index>::iterator iter_pair =  switch_pairs.begin();
		int jj = 0;
		bool canNotAll = false;
		int total_pair = 0;
		int canNot = 0;
		vector<host_require_actuality>::iterator iter_temp_host = host_require_actuality_list.begin();
		while(iter_pair != switch_pairs.end())
		{
			if(!iter_pair->finished)total_pair++;
			if(!iter_pair->finished && iter_pair->need_compute)
			{
				int index = iter_pair->index;
				int start,end;
				{
					start = iter_pair->sid1;
					end = iter_pair->sid2;
					int *path = new int[switchCount];
					int delay = iter_pair->delay;
					int path_delay;
					int result = dijkstra(start,end,path,delay,path_delay);
					if(result == -1)
					{
				 		cout<<"can not find the path from "<<iter_pair->sid1<<" to "<<iter_pair->sid2<<endl;
						canNot++;
						iter_pair->finished = true;
						iter_temp_host->finished = true;
			 		}
					else
					{
					//	cout<<"("<<iter_pair->sid1<<","<<iter_pair->sid2<<","<<index<<")"<<endl;	
					 	list<int> p;
						int j = end;
						while(j != start)
						{
							p.push_front(j);
							j = path[j];
						}
						p.push_front(j);
			
					//	cout<<"path is : ";	
						list<int>::iterator it = p.begin();
						while(it != p.end())
						{
						//	cout<<*it<<" ";
							it++;
						}
					//	cout<<endl;
						paths[index] = path;	
						delays[index] = path_delay;
						iter_pair->need_compute = false;
		
					}
					jj++;
				 }
			//	else cout<<"can not find the index of "<<iter_pair->sid1<<" or "<<iter_pair->sid2<<endl;
			}
			iter_pair++;
			iter_temp_host++;
		}
		if(total_pair == canNot)canNotAll = true;
		if(canNotAll)break;
		/***********allocate band to each link***********/
		iter_require_list = host_require_actuality_list.begin();
		int count = -1;
		bool allFinished = true;
		bool noMoreBand = true;
		while(iter_require_list != host_require_actuality_list.end())
		{
			if(iter_require_list->finished)
			{
			//	cout<<"the finish band of ("<<iter_require_list->src_sid<<","<<iter_require_list->dst_sid<<","<<iter_require_list->priority<<") is : "<<iter_require_list->band_now<<endl;

				iter_require_list++;
				++count;
				continue;
			}
			else allFinished = false;
			int start = iter_require_list->src_sid;
			int end = iter_require_list->dst_sid;
			int * path = paths[++count];

			vector<pair<int,int> > vp;
			int j = end;
			int k = path[j];
			bool allowed = true;
			while(j != start)
			{
				double allowcate_band = iter_require_list->priority*x;
				if(band_array[k][j] - iter_require_list->priority*x <0.00000010)
				{
				//	cout<<"here k j "<<k<<" "<<j<<endl;
					vp.push_back(make_pair<int,int>(k,j));
					allowed = false;
				//	break;
				}
				j = k;
				k = path[j];
			}
		//	cout<<"x is "<<x<<endl;
			if(!allowed)
			{
			//	cout<<"in !allowed now the x is "<<x<<endl;
				/****remove to the situation as before****/
				if(iter_require_list != host_require_actuality_list.begin())
				{
					iter_require_list--;
					count--;
					while(iter_require_list != host_require_actuality_list.begin())
					{
						if(iter_require_list->finished)
						{
							iter_require_list--;
							--count;
							continue;
						}
						int start = iter_require_list->src_sid;
						int end = iter_require_list->dst_sid;
						
						int * path = paths[count--];
						int j1 = end;
						int k1 = path[j1];
						int p = end;
						
					//	cout<<"***************"<<endl;
					//	cout<<"count is "<<count<<endl;
					//	cout<<p<<" ";
						while(p != start)
						{
							p = path[p];
						//	cout<<p<<" ";
						}
					//	cout<<endl;
					//	cout<<"***************"<<endl;
						/******reset the band_now of each iter_require********/
						/************reset the band_array*************/
						double allocate_band = iter_require_list->priority*x;
					//	list<int> queue;
						while(j1 != start)
						{
						//	queue.push_front(j1);
							band_array[j1][k1] += allocate_band;
							band_array[k1][j1] += allocate_band;
							j1 = k1;
							k1 = path[j1];
						}
					//	queue.push_front(j1);
					//	insert_path_band(queue,iter_require_list,allocate_band,0);
						iter_require_list->band_now -= allocate_band;
						
						iter_require_list--;
					}
					/*******reset the first iter_require********/
					if(!iter_require_list->finished)
					{
						int start = iter_require_list->src_sid;
						int end = iter_require_list->dst_sid;
						int * path = paths[count--];
						int j2 = end;
						int k2 = path[j2];
						/******reset the band_now of each iter_require********/
						/************reset the band_array*************/
						double allocate_band = iter_require_list->priority*x;
					//	list<int> queue;
						while(j2 != start)
						{
						//	queue.push_front(j2);
						/*	if((j2 == 1 && k2 == 5) || (j2 == 5 && k2 == 1))
							{
								cout<<"*******2******"<<endl;
								cout<<"count is "<<count+1<<" j is "<<j2<<" k is "<<k2<<endl;
								cout<<"*************"<<endl;
							}*/
							band_array[j2][k2] += allocate_band;
							band_array[k2][j2] += allocate_band;
							j2 = k2;
							k2 = path[j2];
						}
					//	queue.push_front(j2);
						/*****minus the band of the path*****/
					//	insert_path_band(queue,iter_require_list,allocate_band,0);
						iter_require_list->band_now -= allocate_band;
					}

				}
				if(delta == 0.01)
				{
					//can not allocate anymore,then set the link as non-link	
					/*******change the connectivity of the graph********/
					//cout<<"in delta == 0.01"<<endl;
					vector<pair<int,int> >::iterator iter_vp = vp.begin();
					while(iter_vp != vp.end())
					{
						j = iter_vp->first;
						k = iter_vp->second;
						conn_array[k][j] = conn_array[j][k] = false;
						iter_vp++;
					}
				//cout<<" hh "<<k<<" "<<j<<endl;
					/******reset all the afftected switches pair to need_compute = true********/
					vector<switch_pair_index>::iterator iter_pair =  switch_pairs.begin();
					vector<host_require_actuality>::iterator iter_host = host_require_actuality_list.begin();
					int index_ = -1;	
					while(iter_pair != switch_pairs.end())
					{
					
						if(iter_pair->finished)
						{
						//	cout<<"in this continue"<<endl;
							iter_pair++;
							iter_host++;
							++index_ ;
							continue;
						}
						int start = iter_pair->sid1;
						int end = iter_pair->sid2;
						int * path = paths[++index_];
						int path_delay = delays[index_];
						int j3 = end;
						int k3 = path[j3];
						while(j3 != start)
						{
							vector<pair<int,int> >::iterator iter_vp = vp.begin();
							bool goout = false;
							while(iter_vp != vp.end())
							{
								j = iter_vp->first;
								k = iter_vp->second;
								if((j3 == j && k3 == k) || (j3 == k && k3 == j) && !iter_pair->finished)
								{
									iter_pair->need_compute = true;
									goout = true;
									break;
								}
								iter_vp++;
							}
						//	cout<<endl;
							if(goout)break;

							j3 = k3;
							k3 = path[j3];
						}
						if(iter_pair->need_compute)
						{
							j3 = end;
							list<int> queue;
							while(j3 != start)
							{
								queue.push_front(j3);
								j3 = path[j3];
							}
							queue.push_front(j3);
							insert_path_band(queue,iter_host,iter_host->band_now,path_delay,1);
						}
						
						iter_pair++;
						iter_host++;

					}
					total = 0;
					continue_ = true;
					x = 0;
			//	return;
				}

				else//delta != 0.01 
				{
					delta = delta/2;
					continue_ = true;
				}

				break;
			}
			else
			{

				j = end;
				k = path[j];
				double allocate_band = iter_require_list->priority*x;
				iter_require_list->band_now += allocate_band;
			//	list<int> queue;
				while(j != start)
				{
				//	queue.push_front(j);
					if(fabs(band_array[k][j] - iter_require_list->priority*x) < 0.00000001)
					{
						cout<<"Error band should allocatedble but not!!"<<endl;
					}
				/*	if((k==1 && j==0) || (j ==1 && k == 0))
					{
						cout<<"*************"<<endl;
						cout<<" k j are "<<k<<" "<<j<<" allocate band is "<<allocate_band<<endl;
						cout<<"*************"<<endl;
					}*/
					band_array[k][j] -= allocate_band;
					band_array[j][k] -= allocate_band;
					if(band_array[k][j] < 0)cout<<"***********band should not < 0 in here!*******"<<endl;
					vector<host_require_actuality>::iterator end_iter = host_require_actuality_list.end();
					end_iter--;
					
					if(band_array[k][j] < 0.000000001 && iter_require_list == end_iter)
					{
						//band_array[k][j] = band_array[j][k] = 0;
						conn_array[k][j] = conn_array[j][k] = false;
						vector<switch_pair_index>::iterator iter_pair =  switch_pairs.begin();
						vector<host_require_actuality>::iterator iter_host = host_require_actuality_list.begin();
						int index_ = -1;
						while(iter_pair != switch_pairs.end())
						{
							if(iter_pair->finished)
							{
								iter_pair++;
								iter_host++;
								index_++;
								continue;
							}
							int start = iter_pair->sid1;
							int end = iter_pair->sid2;
							int * path = paths[++index_];
							int path_delay = delays[index_];
							int j3 = end;
							int k3 = path[j3];
							while(j3 != start)
							{
								if((j3 == j && k3 == k) || (j3 == k && k3 == j) && !iter_pair->finished)
								{
									iter_pair->need_compute = true;
									break;

								}
								j3 = k3;
								k3 = path[j3];
							}
							if(iter_pair->need_compute)
							{
								j3 = end;
								list<int> queue;
								while(j3 != start)
								{
									queue.push_front(j3);
									j3 = path[j3];
								}
								queue.push_front(j3);
								insert_path_band(queue,iter_require_list,iter_require_list->band_now,path_delay,1);
							}
						
							iter_pair++;
							iter_host++;
						}
					}
					

					noMoreBand = false;
					j = k;
					k = path[j];
				}
				//queue.push_front(j);
				//insert_path_band(queue,iter_require_list,allocate_band,1);
			}

			iter_require_list++;
		}
		/***if after this allocating some require band's need is met,then set the finished to true'***/
		if(iter_require_list == host_require_actuality_list.end())
		{
			total += x;
		//	cout<<"total is "<<total<<" and x is "<<x<<endl;
			int cc = -1;
			iter_require_list = host_require_actuality_list.begin();
			vector<switch_pair_index> :: iterator iter_pp = switch_pairs.begin();
			while(iter_require_list != host_require_actuality_list.end())
			{
				cc++;
				if(iter_require_list->band_now >= iter_require_list->band_require && !iter_require_list->finished)
				{
					iter_require_list->finished = true;
					iter_pp++;
					/******give back the overhead band to the link*******/
					if(iter_require_list->band_now >= iter_require_list->band_require)
					{
						double give_back = iter_require_list->band_now - iter_require_list->band_require;
					/*	cout<<"*************************"<<endl;
						cout<<"band_now is "<<iter_require_list->band_now<<" band_require is "<<iter_require_list->band_require<<" give back is "<<give_back<<endl;
						cout<<"*************************"<<endl;
					*/
						int * path = paths[cc];
						int path_delay = delays[cc];
						int start = iter_require_list->src_sid;
						int end = iter_require_list->dst_sid;
						int j2 = end;
						int k2 = path[j2];
						/******reset the band_now of each iter_require********/
						/************reset the band_array*************/
						iter_require_list->band_now -= give_back;
						list<int> queue;
						while(j2 != start)
						{
							queue.push_front(j2);
						//	cout<<"ori is "<<band_array[j2][k2]<<" "<<band_array[k2][j2]<<endl;
							band_array[j2][k2] += give_back;
							band_array[k2][j2] += give_back;
						//	cout<<"j2 is "<<j2<<" k2 is "<<k2<<" band is "<<band_array[j2][k2]<<endl;
							j2 = k2;
							k2 = path[j2];
						}
						queue.push_front(j2);
						insert_path_band(queue,iter_require_list,iter_require_list->band_now,path_delay,1);
						//cout<<"now the band is "<<iter_require_list->band_now<<endl;
						
					}
					/*
					vector<switch_pair_index> :: iterator spi_iter = switch_pairs.begin();
					while(spi_iter != switch_pairs.end())
					{
						if(spi_iter->sid1 == iter_require_list->src_sid && spi_iter->sid2 == iter_require_list->dst_sid)
						{
							spi_iter->finished = true;
							break;
						}
						spi_iter++;
					}*/
				}
				iter_require_list++;
				iter_pp++;
			}
		}
		
		if(allFinished/* || (noMoreBand && x == 0.01) */|| canNotAll)
		{
			if(allFinished)cout<<"break while as allFinsihed"<<endl;
			else 
			cout<<"break while as noMoreBand"<<endl;
			break;
		}
		
		if(!continue_)delta *= 2;
	}
	/*****after allocating the band array is*******/
	/*
	cout<<endl;
	cout<<"after allocating, the band array is "<<endl;
	for(int i =0 ;i < switchCount; i++)
	{
		for(int j = 0; j<switchCount; j++)
			cout<<band_array[i][j]<<" ";
		cout<<endl;
	}
	*/
	//display the result of band and delay after TE
	/*	
	vector<host_require_actuality>::iterator ihra = host_require_actuality_list.begin();
	while(ihra != host_require_actuality_list.end())
	{
		display_host_requirement(ihra);
		ihra++;
	}
	*/

	/*
	cout<<"now the connectivity is "<<endl;
	for(int i = 0; i < 6; i++)
	{
		for(int j =0 ; j <6; j++)
			cout<<conn_array[i][j]<<" ";
		cout<<endl;

	}
	*/


}

void Hieron_local::insert_path_band(list<int> queue,vector<host_require_actuality>::iterator iter_require_list,double allocate_band,int delay,int kind)
{
//	cout<<endl<<endl;
//	cout<<"~~~~~~~~~~~~~~~~~~~"<<endl;
//	cout<<"band is "<<allocate_band<<endl;
	int * p = new int[queue.size()];
	int _size = queue.size();
	for(int in = 0; in < _size; in++)
	{
		p[in] = queue.front();
	//	cout<<p[in]<<" ";
		queue.pop_front();

	}
//	cout<<endl;
	if(kind == 1)
	{
	/*	vector<pair<pair<int *,double>,int> > :: iterator iter_pb1 = iter_require_list->path_band.begin();
		while(iter_pb1 != iter_require_list->path_band.end())
		{
			allocate_band -= iter_pb1->first.second;
			iter_pb1++;
		}
*/
		vector<path_band_delay> :: iterator iter_pbd = iter_require_list->path_band_delays.begin();
		while(iter_pbd != iter_require_list->path_band_delays.end())
		{
			allocate_band -= iter_pbd->band;
			iter_pbd++;
		}
	}

	if(iter_require_list->path_band_delays.size() == 0)
	{
		if(kind == 0)
		{
			cout<<"Error, do not have the path while now wanting minus it"<<endl;
		}
		else if(kind == 1&&allocate_band<=0)return;
/*		pair<int*, double>pb(p,allocate_band);
		pair<pair<int*,double>,int>path(pb,_size);
		iter_require_list->path_band.push_back(path);*/
		path_band_delay pbd(_size,p,allocate_band,delay);
		iter_require_list->path_band_delays.push_back(pbd);
	}
	else
	{
		bool has = false;
	//	vector<pair<pair<int *,double>,int> > :: iterator iter_pb = iter_require_list->path_band.begin();
		vector<path_band_delay>::iterator iter_pbd = iter_require_list->path_band_delays.begin();
		while(iter_pbd != iter_require_list->path_band_delays.end())
		{
			if(_size != iter_pbd->path_length)
			{
				iter_pbd++;
				continue;
			}
			else
			{
				int i = 0;
				for(i = 0; i< _size; i++)
				{
					if(p[i] != (iter_pbd->path)[i])
					{
						break;
					}
				}
				if( i == _size)has = true;
			}
			if(has)
			{
			//	cout<<"has the path "<<endl;
				if(kind == 1)
				{
					return;
					allocate_band += iter_pbd->band;
					if(allocate_band <= 0)cout<<"error,band should > 0"<<endl;
					iter_pbd->band += allocate_band;
				}
				else 
					iter_pbd->band -= allocate_band;
					
				break;
			}
			iter_pbd++;
		}
		if(!has)
		{
			if(kind == 0)
			{
				cout<<"Error, do not have the path while now wanting minus it"<<endl;
			}
			else if(kind == 1 && allocate_band <= 0)return;
			/*
			pair<int*, double>pb(p,allocate_band);
			pair<pair<int*,double>,int>path(pb,_size);
			iter_require_list->path_band.push_back(path);
			*/
			path_band_delay pbd(_size,p,allocate_band,delay);
			iter_require_list->path_band_delays.push_back(pbd);
		}
	}
//	cout<<"~~~~~~~~~~~~~~~~~~~~~"<<endl<<endl;

}
//这里就是requirement的产生之处。每个local_domain产生5个，有n个domain,可以产生5n个。
void Hieron_local::collectRequirement1()
{
	host_require_actuality hr1;
	hr1.src_sid = 0;
	hr1.dst_sid = 4;
	hr1.priority = 3;
	hr1.delay_require = 30;
	hr1.band_require = 60;
	hr1.remain_amount = 100;

	host_require_actuality hr2;
	hr2.src_sid = 0;
	hr2.dst_sid = 4;
	hr2.priority = 1;
	hr2.delay_require = 70;
	hr2.band_require = 30;
	hr2.remain_amount = 100;


	host_require_actuality hr3;
	hr3.src_sid = 3;
	hr3.dst_sid = 4;
	hr3.priority = 2;
	hr3.delay_require = 90;
	hr3.band_require = 60;
	hr3.remain_amount = 100;

	host_require_actuality hr4;
	hr4.src_sid = 3;
	hr4.dst_sid = 4;
	hr4.priority = 3;
	hr4.delay_require = 10;
	hr4.band_require = 30;
	hr4.remain_amount = 100;

	host_require_actuality hr5;
	hr5.src_sid = 3;
	hr5.dst_sid = 4;
	hr5.priority = 1;
	hr5.delay_require = 5;
	hr5.band_require = 20;
	hr5.remain_amount = 100;

	host_require_actuality_list.push_back(hr1);
	host_require_actuality_list.push_back(hr2);
	host_require_actuality_list.push_back(hr3);
	host_require_actuality_list.push_back(hr4);
	host_require_actuality_list.push_back(hr5);
}

void Hieron_local::display_host_requirement(const vector<host_require_actuality>:: iterator iter)
{
	cout<<endl<<endl;
	cout<<"****************************"<<endl;
	cout<<"src_sid : "<<iter->src_sid<<endl;
	cout<<"dst_sid : "<<iter->dst_sid<<endl;
	cout<<"src_domain : "<<iter->src_domain<<endl;
	cout<<"dst_domain : "<<iter->dst_domain<<endl;
	cout<<"delay : "<<iter->delay_require<<endl;
	cout<<"priority : "<<iter->priority<<endl;
	cout<<"remain_amount : "<<iter->remain_amount<<endl;
	cout<<"finished : "<<iter->finished<<endl;
	cout<<"band_now : "<<iter->band_now<<endl;
	cout<<"band_require :"<<iter->band_require<<endl;
	cout<<"last start time : "<<iter->last_start_time<<endl;
	cout<<"finish_time_predict : "<<iter->finish_time_predict<<endl;
	cout<<"each path and the band and delay of each path : "<<endl;
/*	vector<pair<pair<int *,double>,int>  >::iterator iter_pair = iter->path_band.begin();
	while(iter_pair != iter->path_band.end())
	{
		for(int i = 0; i < iter_pair->second; i++)
			cout<<(iter_pair->first.first)[i]<<" ";
		cout<<": "<<iter_pair->first.second<<endl;
		iter_pair++;
	}
	*/
	vector<path_band_delay>::iterator iter_pbd = iter->path_band_delays.begin();
	while(iter_pbd != iter->path_band_delays.end())
	{
		for(int i =0; i < iter_pbd->path_length; i++)
			cout<<iter_pbd->path[i]<<" ";
		cout<<": ("<<iter_pbd->band<<","<<iter_pbd->delay<<")"<<endl;
		iter_pbd++;
	}
	cout<<"****************************"<<endl<<endl;
}
void Hieron_local::display_domain_requirement(const vector<domain_requirement>:: iterator iter)
{
	cout<<endl<<endl;
	cout<<"****************************"<<endl;
	cout<<"src_sid : "<<iter->src_domain<<endl;
	cout<<"dst_sid : "<<iter->dst_domain<<endl;
	cout<<"priority : "<<iter->priority<<endl;
	cout<<"remain_amount : "<<iter->remain_amount<<endl;
	cout<<"finished : "<<iter->finished<<endl;
	cout<<"band_now : "<<iter->band_now<<endl;
	cout<<"band_require : "<<iter->band_require<<endl;
	cout<<"each path and the band of each path : "<<endl;
	vector<pair<pair<int *,delay_band>,int>  >::iterator iter_pair = iter->path_band.begin();
	while(iter_pair != iter->path_band.end())
	{
		for(int i = 0; i < iter_pair->second; i++)
			cout<<(iter_pair->first.first)[i]<<" ";
		cout<<": "<<iter_pair->first.second.delay<<iter_pair->first.second.band<<endl;
		iter_pair++;
	}
	cout<<"****************************"<<endl<<endl;
}

vector<host_require_actuality>& Hieron_local::getHostRequireActuality()
{
	return host_require_actuality_list;
}

vector<pair<int,int> > Hieron_local::getDelayBandList()
{
	return delay_band_list;
}

void Hieron_local::getVirtualLink()
{
	unsigned int INFINITY0 = 0xFFFFFFF;
	//vector<host_require_actuality> virtual_link_list;
	vector<int>::iterator iter_marginal_nodes1 = marginal_nodes.begin();
	while(iter_marginal_nodes1 != marginal_nodes.end())
	{
		vector<int>::iterator iter_marginal_nodes2 = marginal_nodes.begin();
		while(iter_marginal_nodes2 != iter_marginal_nodes1)
			iter_marginal_nodes2++;
		iter_marginal_nodes2++;
		while(iter_marginal_nodes2 != marginal_nodes.end())
		{
			int src_sid;
			int dst_sid;
			map<int,int>::const_iterator iter_src = switch_index.find(*iter_marginal_nodes1);
			map<int,int>::const_iterator iter_dst = switch_index.find(*iter_marginal_nodes2);
			if(iter_src != switch_index.end() && iter_src != switch_index.end())
			{
				src_sid = iter_src->second;
				dst_sid = iter_dst->second;
				//cout<<"src_sid is "<<src_sid<<endl;
				//cout<<"dst_sid is "<<dst_sid<<endl;
			}
			else
			{
				cout<<"In getVirtualLink function, \ncan not find the index of the marginal nodes: "<<*iter_marginal_nodes1<<" or "<<*iter_marginal_nodes2<<endl;
				return;
			}
			host_require_actuality hr;
			hr.src_sid = src_sid;
			hr.dst_sid = dst_sid;
			hr.src_domain = hr.dst_domain = domainID;
			hr.priority = 1;
			hr.delay_require = INFINITY0;
			hr.band_require = INFINITY0;
			hr.remain_amount = INFINITY0;
			virtual_link_list.push_back(hr);
			iter_marginal_nodes2++;
		}
		iter_marginal_nodes1++;
	}
	TE_fun(virtual_link_list);
	
	vector<host_require_actuality>::iterator iter = virtual_link_list.begin();
	
	while(iter != virtual_link_list.end())
	{
		//set the virtual edge index to sid
	//	display_host_requirement(iter);
		iter->src_sid = index_switch[iter->src_sid];
		iter->dst_sid = index_switch[iter->dst_sid];
		iter++;
	}
	

}

void Hieron_local::minus_band_allocated_2_virtual_link()
{
	cout<<endl;
	cout<<"Start the minus_band_allocated_2_virtual_link..."<<endl;
	vector<host_require_actuality>::iterator iter = virtual_link_list.begin();
	while(iter != virtual_link_list.end())
	{
		vector<path_band_delay>::iterator iter_pbd = iter->path_band_delays.begin();
		while(iter_pbd != iter->path_band_delays.end())
		{
			for(int i = 1; i < iter_pbd->path_length; i++)
			{
				int index1 = iter_pbd->path[i-1];
				int index2 = iter_pbd->path[i];
			//	printf("before minus ,the band between %d  and %d is %f:%f and minus band is %f\n",index1,index2,band_array[index1][index2],band_array[index2][index1],percent_virtual_link*iter_pbd->band);
			//	give back the 1 - percent band back to band
				band_array[index1][index2] += (1 - percent_virtual_link)*iter_pbd->band;
				band_array[index2][index1] += (1 - percent_virtual_link)*iter_pbd->band;
				if(band_array[index1][index2] < 0)cout<<"----------band < 0--------------"<<endl;
			//	printf("percent is = %f, iter_pbd->band = %f,minus is %f ; after minus,the band between %d  and %d is %f:%f\n\n",percent_virtual_link,iter_pbd->band,(1 - percent_virtual_link)*iter_pbd->band,index1,index2,band_array[index1][index2],band_array[index2][index1]);
			}
			iter_pbd++;
		}
		iter++;

	}
	cout<<"Finish the minus_band_allocated_2_virtual_link."<<endl<<endl;

}

float Hieron_local::Pareto(float alpha,float xm)
{
	float R;
	R = (float)rand()/(float)(RAND_MAX);
	float result = 0;
	result = xm*(float)(pow(alpha/(xm * R),(float)1/(alpha + 1)));
	//if(result > 100)result = 100;
//	output_error_log<<"xm is "<<xm<<" R is "<<R<<" band is "<<result<<endl;
	return result;
}
int Hieron_local::Poisson(float mean)
{
	float R;
	float sum = 0;
	int i;
	i = -1;
	float z;
	while(sum <= mean)
	{
		R=(float)rand()/(float)(RAND_MAX);
		z = -log(R);
		sum+=z;
		i++;
	}
	return i;
}
void Hieron_local::set_ip_and_port()
{
	
	ifstream infile;
	infile.open("IPandPort");
	if(infile.is_open())
	{
		while(infile.good()&& !infile.eof())
		{
			int domain;
			char buffer[1024];
			infile.getline(buffer,1024);
			string message;
			message = buffer;
		//	cout<<"message is "<<message<<endl;
			istringstream ss(message);
			ss>>domain;
			if(domain == domainID)
			{
				ss>>local_ip;
				ss>>local_port;
			}
			else if(domain == 0)
			{
				ss>>root_ip;
				ss>>root_port;
			}
			else if(domain == 10)
			{
				ss>>centralized_ip;
				ss>>centralized_port;
			}
		}
	}
	printf("root ip:port %s:%d; local ip:port %s:%d\n",root_ip.c_str(),root_port,local_ip.c_str(),local_port);
	infile.close();
}
void Hieron_local::set_pareto_alpha(float alpha)
{
	pareto_alpha = alpha;
}
void Hieron_local::set_poisson_mean(float mean)
{
	poisson_mean = mean;
}

void Hieron_local::set_pareto_xm(float xm)
{
	pareto_xm = xm;
}

void Hieron_local::set_flow_amount_devide_band(int fadb)
{
	flow_amount_devide_band = fadb;
}

void Hieron_local::set_mergeTimes(int t)
{
	mergeTimes = t;
	output_error_log<<"mergeTimes is "<<mergeTimes<<endl;
}

void Hieron_local::set_percent_virtual_link(float f)
{
	percent_virtual_link = f;
}

void Hieron_local::collect_requirements()
{
	cout<<"~~~~~~~~~~~~~~~~~~~in collectting requirements~~~~~~~~~~~~~~~~~~~~"<<endl;
	timeval tv;
	gettimeofday(&tv,NULL);
	long start_time = tv.tv_sec;
	long last_time = 0;

	int count_time = 0;
//	while(true)
	{
		timeval tv1;
		gettimeofday(&tv1,NULL);
		long now_time = tv1.tv_sec;
		if(last_time != now_time && (now_time - start_time)%300 == 0)//set the 30 to 300 after the test
		{
			count_time++;
		//	output_error_log<<"start_time is "<<start_time<<" now time is "<<now_time<<endl;
			collect_requirement_every5minutes(now_time);//need to be careful whether this function takes time more than 300
			double count_predict = 0;
			double use_this_time = 0;
			last_time = now_time;
		}
	}

}

void Hieron_local::collect_requirement_every5minutes(long start_seconds)
{
	cout<<endl;
	cout<<"Start collect requiements every 5  minutes...."<<endl;
	output_error_log<<"Start collect requirements every 5 minutes."<<endl;
	int count_require = Poisson(poisson_mean);
	if(count_require < 90)
		count_require = 90 + domainID;
	output_error_log<<"the require size is "<<count_require<<endl<<endl;
	int a = count_require/30;
	int b = count_require%30;
	int count_every_10seconds[30];
	for(int i =0; i < 30; i++)
	{
		count_every_10seconds[i] = a;
		if(i < b)count_every_10seconds[i] += 1;
	//	output_error_log<<(i + 1)*10<<" seconds has requirements : "<<count_every_10seconds[i]<<endl;
	}
	
	timeval tv;
	gettimeofday(&tv,NULL);
	long seconds = tv.tv_sec;
	long last_seconds = 0;
	int kth = 0;
	while(seconds < start_seconds + 300)
	{
		if(last_seconds != seconds && (seconds - start_seconds)%10 == 0)
		{
			pthread_mutex_lock(&host_requirement_lock);
			collect_requirement_every10seconds(count_every_10seconds[kth]);
			pthread_mutex_unlock(&host_requirement_lock);
			kth++;
		}
		last_seconds = seconds;
		gettimeofday(&tv,NULL);
		seconds = tv.tv_sec;
	}
	cout<<"Finish collect requiements every 5  minutes."<<endl<<endl;
	output_error_log<<"Finish collect requirements every 5 minutes."<<endl;
	

}

void Hieron_local::collect_requirement_every10seconds(int count)
{
	cout<<endl;
	cout<<"Start collect requirements every 10 seconds...."<<endl;
	int inter_require = 0;
	for(int i =0; i < count; i++)
	{
		double remain_amount = Pareto(pareto_alpha,pareto_xm);
		double band_require = remain_amount/flow_amount_devide_band;
		int delay;
		int src_index,dst_index,src_sid,dst_sid;
		src_index = rand()%switchID.size();
		src_sid = switchID[src_index];
		dst_sid = src_sid;
		while(dst_sid == src_sid)
		{
			dst_index = rand()%allSwitchID.size();
			dst_sid = allSwitchID[dst_index];
		}
		/********get the domain id of the dst sid*******/
		int dst_domain;
		while(true)
		{
			bool find = false;
			vector<int>::iterator iter_dst = switchID1.begin();
			while(iter_dst != switchID1.end())
			{
				if(*iter_dst == dst_sid)
				{
					find = true;
					dst_domain = 1;
					break;
				}
				iter_dst++;
			}
			if(find)break;
			iter_dst = switchID2.begin();
			while(iter_dst != switchID2.end())
			{
				if(*iter_dst == dst_sid)
				{
					find = true;
					dst_domain = 2;
				//	inter_require++;
					break;
				}
				iter_dst++;
			}
			if(find)break;
			iter_dst = switchID3.begin();
			while(iter_dst != switchID3.end())
			{
				if(*iter_dst == dst_sid)
				{
					find = true;
					dst_domain = 3;
				//	inter_require++;
					break;
				}
				iter_dst++;
			}
			if(find)break;
			iter_dst = switchID4.begin();
			while(iter_dst != switchID4.end())
			{
				if(*iter_dst == dst_sid)
				{
					find = true;
					dst_domain = 4;
				//	inter_require++;
					break;
				}
				iter_dst++;
			}
			if(find)break;
			iter_dst = switchID5.begin();
			while(iter_dst != switchID5.end())
			{
				if(*iter_dst == dst_sid)
				{
					find = true;
					dst_domain = 5;
					//inter_require++;
					break;
				}
				iter_dst++;
			}
			if(find)break;
		}
		if(dst_domain != domainID)
			inter_require++;
		if(dst_domain != domainID && inter_require > count/3)
		{
			//output_error_log<<"dst_domain is "<<dst_domain<<" dst sid is "<<dst_sid<<" i is "<<i<<endl;
			i--;
			continue;
		}
		bool intra_require = false;
		vector<int>::iterator iter = switchID.begin();
		while(iter != switchID.end())
		{
			if(*iter == dst_sid)
			{
				intra_require = true;
				break;
			}
			iter++;
		}
		/*****set the delay as whether they are inter require or not*****/
		if(intra_require)
		{
			delay = rand() % 50 + 20;//if the requirement is intra the domain ,it is between 20 and 70

		}
		else//as the requirement is inter requirement
		{
			delay = rand() % 50 + 70;//if the requirement is inter domain,it is between 70 and 120;
		}
		int priority = rand()%3 + 1;//priority is between 1 and 3
		host_require_actuality hra;
		hra.src_sid = src_sid;
		hra.dst_sid = dst_sid;
		hra.src_domain = domainID;
		hra.dst_domain = dst_domain;
		hra.priority = priority;
		hra.remain_amount = remain_amount;
		hra.band_require = band_require;
		hra.delay_require = delay;

		all_require_to_centralized.push_back(hra);

		if(dst_domain == domainID)//it is the intra requirement
		{
			hra.src_sid = switch_index[hra.src_sid];
			hra.dst_sid = switch_index[hra.dst_sid];
		}
		else//it is the inter requirement
		{
			host_require_actuality hra1;
			hra1.src_sid = src_sid;
			hra1.dst_sid = dst_sid;
			hra1.src_domain = domainID;
			hra1.dst_domain = dst_domain;
			hra1.priority = priority;
			hra1.remain_amount = remain_amount/2;
			hra1.band_require = band_require/2;
			hra1.delay_require = delay;
			hra1.src_marginal_sid = marginal_nodes[1];//add the marginal node sid of the src domain
			host_require_actuality_list.push_back(hra1);
			hra.band_require = band_require/2;
			hra.remain_amount = remain_amount/2;
			hra.src_marginal_sid = marginal_nodes[0];//add the marginal node sid of the src domain
		}
		host_require_actuality_list.push_back(hra);

	}
	/****calculate the usage******/
//	calculateUsage();

	//Here,go through the host_require_actualith_list to change the list, minus the remain_amount every 10 seconds
	/*vector<host_require_actuality>::iterator iter_list = host_require_actuality_list.begin();
	bool erase_host_requirement = false;
	while(iter_list != host_require_actuality_list.end())
	{
		if(iter_list->band_now == 0)
		{
			iter_list++;
			continue;
		}
		iter_list->remain_amount -= iter_list->band_now*10;
		if(iter_list->remain_amount <= 0.000000000000001)
		{
			printf("in erase the requirement!\n");
			vector<path_band_delay>::iterator iter_path = iter_list->path_band_delays.begin();
			while(iter_path != iter_list->path_band_delays.end())
			{
				for(int i = 0; i < iter_path->path_length - 1; i++)
				{
					if(!conn_array[iter_path->path[i]][iter_path->path[i+1]])
					{
						conn_array[iter_path->path[i]][iter_path->path[i+1]] = conn_array[iter_path->path[i + 1]][iter_path->path[i]]  = true;
					}
					band_array[iter_path->path[i]][iter_path->path[i+1]] += iter_path->band;
					band_array[iter_path->path[i + 1]][iter_path->path[i]]  += iter_path->band;
				}
				iter_path++;
			}
			iter_list =	host_require_actuality_list.erase(iter_list);
			printf("After erase, now there are %lu requirements\n",host_require_actuality_list.size());
			continue;
			
		}
		iter_list++;
	}
	*/
	cout<<"Finish collect requirements every 10 seconds."<<endl;
}
void Hieron_local::calculateUsage()
{
	cout<<"Start calculating the usage...."<<endl;
	int random_index = rand()%host_require_actuality_list.size();
	host_require_actuality hra = host_require_actuality_list[random_index];
	int count = 0;
	while(hra.band_now < 0.00000001 && count < 20)
	{
		count++;
		random_index = rand()%host_require_actuality_list.size();
		hra = host_require_actuality_list[random_index];
	//	display_host_requirement(host_require_actuality_list.begin() + random_index);
	}
	/*if(hra.band_now < 0.00000001)
		cout<<"the band now is 0"<<endl;
	else printf("the band is > 0 and the band is %f\n",hra.band_now);*/
	printf("The band of the path choosed is %f\n",hra.band_now);
	int * path;
	int start,end;
	if(hra.path_band_delays.size()>0)
	{
		path = (hra.path_band_delays[hra.path_band_delays.size()-1]).path;
		int count = 0;
		double count_use = 0;
		count = (hra.path_band_delays[hra.path_band_delays.size()-1]).path_length;
		for(int i =0; i < count - 1; i++)
		{
			count_use += (band_array_copy[path[i]][path[i+1]] - band_array[path[i]][path[i+1]])/band_array_copy[path[i]][path[i+1]];
		}
		if(count != 0)
		{
			count_use /= count;
			usage.push_back(count_use);
		}
	}
	cout<<"Finish calculating the usage."<<endl;
	
}

void Hieron_local::mergeDelay_band()
{
	vector<host_require_actuality>::iterator iter_hra = host_require_actuality_list.begin();
	int p1,p2,p3;
	vector<host_require_actuality>d11,d12,d13,d14,d15,d21,d22,d23,d24,d25,d31,d32,d33,d34,d35;
	vector<host_require_actuality>e11,e12,e13,e14,e15,e21,e22,e23,e24,e25,e31,e32,e33,e34,e35;
	p1 = 0;
	p2 = 0;
	p3 = 0;
	while(iter_hra != host_require_actuality_list.end())
	{
		if(iter_hra->src_domain == iter_hra->dst_domain)
		{
		//	if(iter_hra->band_now < 0.00000000001 )//never allocate by TE
			{

			//	host_require_actuality hra(iter_hra->src_sid,iter_hra->dst_sid,iter_hra->src_domain,iter_hra->dst_domain,iter_hra->priority,iter_hra->delay_require,iter_hra->remain_amount,iter_hra->band_now,iter_hra->band_require,iter_hra->last_start_time,iter_hra->finish_time_predict);
				intra_require_list.push_back(*iter_hra);
			}
			iter_hra++;
			continue;
		}
	/*	else if(iter_hra->band_now > 0.000000000001)//is allocated before,then we neglect it
		{
			iter_hra++;
			continue;
		}*/
		else
		{
			inter_require_list.push_back(*iter_hra);
		}
		if(iter_hra->priority == 1 && iter_hra->src_marginal_sid == marginal_nodes[0])
		{
			p1++;
			switch(iter_hra->dst_domain)
			{
				case 1:{
						   vector<host_require_actuality>::iterator iter_t = d11.begin();
						   while(iter_t != d11.end() && iter_t->delay_require < iter_hra->delay_require)
							   iter_t++;
						   if(iter_t != d11.end() && iter_t->delay_require == iter_hra->delay_require)
							   iter_t->band_now += iter_hra->band_now;
						   else d11.insert(iter_t,*iter_hra);
						   break;
					   }
				case 2:{
						   vector<host_require_actuality>::iterator iter_t = d12.begin();
						   while(iter_t != d12.end() && iter_t->delay_require < iter_hra->delay_require)
							   iter_t++;
						   if(iter_t != d12.end() && iter_t->delay_require == iter_hra->delay_require)
							   iter_t->band_now += iter_hra->band_now;
						   else d12.insert(iter_t,*iter_hra);
						   break;
					   }
				case 3:{
						   vector<host_require_actuality>::iterator iter_t = d13.begin();
						   while(iter_t != d13.end() && iter_t->delay_require < iter_hra->delay_require)
							   iter_t++;
						   if(iter_t != d13.end() && iter_t->delay_require == iter_hra->delay_require)
							   iter_t->band_now += iter_hra->band_now;
						   else d13.insert(iter_t,*iter_hra);
						   break;
					   }
				case 4:{
						   vector<host_require_actuality>::iterator iter_t = d14.begin();
						   while(iter_t != d14.end() && iter_t->delay_require < iter_hra->delay_require)
							   iter_t++;
						   if(iter_t != d14.end() && iter_t->delay_require == iter_hra->delay_require)
							   iter_t->band_now += iter_hra->band_now;
						   else d14.insert(iter_t,*iter_hra);
						   break;
					   }

				case 5:{
						   vector<host_require_actuality>::iterator iter_t = d15.begin();
						   while(iter_t != d15.end() && iter_t->delay_require < iter_hra->delay_require)
							   iter_t++;
						   if(iter_t != d15.end() && iter_t->delay_require == iter_hra->delay_require)
							   iter_t->band_now += iter_hra->band_now;
						   else d15.insert(iter_t,*iter_hra);
						   break;
					   }

			}
		}
		if(iter_hra->priority == 1 && iter_hra->src_marginal_sid == marginal_nodes[1])
		{
			p1++;
			switch(iter_hra->dst_domain)
			{
				case 1:{
						   vector<host_require_actuality>::iterator iter_t = e11.begin();
						   while(iter_t != e11.end() && iter_t->delay_require < iter_hra->delay_require)
							   iter_t++;
						   if(iter_t != e11.end() && iter_t->delay_require == iter_hra->delay_require)
							   iter_t->band_now += iter_hra->band_now;
						   else e11.insert(iter_t,*iter_hra);
						   break;
					   }
				case 2:{
						   vector<host_require_actuality>::iterator iter_t = e12.begin();
						   while(iter_t != e12.end() && iter_t->delay_require < iter_hra->delay_require)
							   iter_t++;
						   if(iter_t != e12.end() && iter_t->delay_require == iter_hra->delay_require)
							   iter_t->band_now += iter_hra->band_now;
						   else e12.insert(iter_t,*iter_hra);
						   break;
					   }
				case 3:{
						   vector<host_require_actuality>::iterator iter_t = e13.begin();
						   while(iter_t != e13.end() && iter_t->delay_require < iter_hra->delay_require)
							   iter_t++;
						   if(iter_t != e13.end() && iter_t->delay_require == iter_hra->delay_require)
							   iter_t->band_now += iter_hra->band_now;
						   else e13.insert(iter_t,*iter_hra);
						   break;
					   }
				case 4:{
						   vector<host_require_actuality>::iterator iter_t = e14.begin();
						   while(iter_t != e14.end() && iter_t->delay_require < iter_hra->delay_require)
							   iter_t++;
						   if(iter_t != e14.end() && iter_t->delay_require == iter_hra->delay_require)
							   iter_t->band_now += iter_hra->band_now;
						   else e14.insert(iter_t,*iter_hra);
						   break;
					   }

				case 5:{
						   vector<host_require_actuality>::iterator iter_t = e15.begin();
						   while(iter_t != e15.end() && iter_t->delay_require < iter_hra->delay_require)
							   iter_t++;
						   if(iter_t != e15.end() && iter_t->delay_require == iter_hra->delay_require)
							   iter_t->band_now += iter_hra->band_now;
						   else e15.insert(iter_t,*iter_hra);
						   break;
					   }

			}
		}
		if(iter_hra->priority == 2 && iter_hra->src_marginal_sid == marginal_nodes[0])
		{
			p2++;
			switch(iter_hra->dst_domain)
			{
				case 1:{
						   vector<host_require_actuality>::iterator iter_t = d21.begin();
						   while(iter_t != d21.end() && iter_t->delay_require < iter_hra->delay_require)
							   iter_t++;
						   if(iter_t != d21.end() && iter_t->delay_require == iter_hra->delay_require)
							   iter_t->band_now += iter_hra->band_now;
						   else d21.insert(iter_t,*iter_hra);
						   break;
					   }
				case 2:{
						   vector<host_require_actuality>::iterator iter_t = d22.begin();
						   while(iter_t != d22.end() && iter_t->delay_require < iter_hra->delay_require)
							   iter_t++;
						   if(iter_t != d22.end() && iter_t->delay_require == iter_hra->delay_require)
							   iter_t->band_now += iter_hra->band_now;
						   else d22.insert(iter_t,*iter_hra);
						   break;
					   }
				case 3:{
						   vector<host_require_actuality>::iterator iter_t = d23.begin();
						   while(iter_t != d23.end() && iter_t->delay_require < iter_hra->delay_require)
							   iter_t++;
						   if(iter_t != d23.end() && iter_t->delay_require == iter_hra->delay_require)
							   iter_t->band_now += iter_hra->band_now;
						   else d23.insert(iter_t,*iter_hra);
						   break;
					   }
				case 4:{
						   vector<host_require_actuality>::iterator iter_t = d24.begin();
						   while(iter_t != d24.end() && iter_t->delay_require < iter_hra->delay_require)
							   iter_t++;
						   if(iter_t != d24.end() && iter_t->delay_require == iter_hra->delay_require)
							   iter_t->band_now += iter_hra->band_now;
						   else d24.insert(iter_t,*iter_hra);
						   break;
					   }

				case 5:{
						   vector<host_require_actuality>::iterator iter_t = d25.begin();
						   while(iter_t != d25.end() && iter_t->delay_require < iter_hra->delay_require)
							   iter_t++;
						   if(iter_t != d25.end() && iter_t->delay_require == iter_hra->delay_require)
							   iter_t->band_now += iter_hra->band_now;
						   else d25.insert(iter_t,*iter_hra);
						   break;
					   }

			}
		}
		if(iter_hra->priority == 2 && iter_hra->src_marginal_sid == marginal_nodes[1])
		{
			p2++;
			switch(iter_hra->dst_domain)
			{
				case 1:{
						   vector<host_require_actuality>::iterator iter_t = e21.begin();
						   while(iter_t != e21.end() && iter_t->delay_require < iter_hra->delay_require)
							   iter_t++;
						   if(iter_t != e21.end() && iter_t->delay_require == iter_hra->delay_require)
							   iter_t->band_now += iter_hra->band_now;
						   else e21.insert(iter_t,*iter_hra);
						   break;
					   }
				case 2:{
						   vector<host_require_actuality>::iterator iter_t = e22.begin();
						   while(iter_t != e22.end() && iter_t->delay_require < iter_hra->delay_require)
							   iter_t++;
						   if(iter_t != e22.end() && iter_t->delay_require == iter_hra->delay_require)
							   iter_t->band_now += iter_hra->band_now;
						   else e22.insert(iter_t,*iter_hra);
						   break;
					   }
				case 3:{
						   vector<host_require_actuality>::iterator iter_t = e23.begin();
						   while(iter_t != e23.end() && iter_t->delay_require < iter_hra->delay_require)
							   iter_t++;
						   if(iter_t != e23.end() && iter_t->delay_require == iter_hra->delay_require)
							   iter_t->band_now += iter_hra->band_now;
						   else e23.insert(iter_t,*iter_hra);
						   break;
					   }
				case 4:{
						   vector<host_require_actuality>::iterator iter_t = e24.begin();
						   while(iter_t != e24.end() && iter_t->delay_require < iter_hra->delay_require)
							   iter_t++;
						   if(iter_t != e24.end() && iter_t->delay_require == iter_hra->delay_require)
							   iter_t->band_now += iter_hra->band_now;
						   else e24.insert(iter_t,*iter_hra);
						   break;
					   }

				case 5:{
						   vector<host_require_actuality>::iterator iter_t = e25.begin();
						   while(iter_t != e25.end() && iter_t->delay_require < iter_hra->delay_require)
							   iter_t++;
						   if(iter_t != e25.end() && iter_t->delay_require == iter_hra->delay_require)
							   iter_t->band_now += iter_hra->band_now;
						   else e25.insert(iter_t,*iter_hra);
						   break;
					   }

			}
		}
		if(iter_hra->priority == 3 && iter_hra->src_marginal_sid == marginal_nodes[0])
		{
			p3++;
			switch(iter_hra->dst_domain)
			{
				case 1:{
						   vector<host_require_actuality>::iterator iter_t = d31.begin();
						   while(iter_t != d31.end() && iter_t->delay_require < iter_hra->delay_require)
							   iter_t++;
						   if(iter_t != d31.end() && iter_t->delay_require == iter_hra->delay_require)
							   iter_t->band_now += iter_hra->band_now;
						   else d31.insert(iter_t,*iter_hra);
						   break;
					   }
				case 2:{
						   vector<host_require_actuality>::iterator iter_t = d32.begin();
						   while(iter_t != d32.end() && iter_t->delay_require < iter_hra->delay_require)
							   iter_t++;
						   if(iter_t != d32.end() && iter_t->delay_require == iter_hra->delay_require)
							   iter_t->band_now += iter_hra->band_now;
						   else d32.insert(iter_t,*iter_hra);
						   break;
					   }
				case 3:{
						   vector<host_require_actuality>::iterator iter_t = d33.begin();
						   while(iter_t != d33.end() && iter_t->delay_require < iter_hra->delay_require)
							   iter_t++;
						   if(iter_t != d33.end() && iter_t->delay_require == iter_hra->delay_require)
							   iter_t->band_now += iter_hra->band_now;
						   else d33.insert(iter_t,*iter_hra);
						   break;
					   }
				case 4:{
						   vector<host_require_actuality>::iterator iter_t = d34.begin();
						   while(iter_t != d34.end() && iter_t->delay_require < iter_hra->delay_require)
							   iter_t++;
						   if(iter_t != d34.end() && iter_t->delay_require == iter_hra->delay_require)
							   iter_t->band_now += iter_hra->band_now;
						   else d34.insert(iter_t,*iter_hra);
						   break;
					   }

				case 5:{
						   vector<host_require_actuality>::iterator iter_t = d35.begin();
						   while(iter_t != d35.end() && iter_t->delay_require < iter_hra->delay_require)
							   iter_t++;
						   if(iter_t != d35.end() && iter_t->delay_require == iter_hra->delay_require)
							   iter_t->band_now += iter_hra->band_now;
						   else d35.insert(iter_t,*iter_hra);
						   break;
					   }

			}
		}
		if(iter_hra->priority == 3 && iter_hra->src_marginal_sid == marginal_nodes[1])
		{
			p3++;
			switch(iter_hra->dst_domain)
			{
				case 1:{
						   vector<host_require_actuality>::iterator iter_t = e31.begin();
						   while(iter_t != e31.end() && iter_t->delay_require < iter_hra->delay_require)
							   iter_t++;
						   if(iter_t != e31.end() && iter_t->delay_require == iter_hra->delay_require)
							   iter_t->band_now += iter_hra->band_now;
						   else e31.insert(iter_t,*iter_hra);
						   break;
					   }
				case 2:{
						   vector<host_require_actuality>::iterator iter_t = e32.begin();
						   while(iter_t != e32.end() && iter_t->delay_require < iter_hra->delay_require)
							   iter_t++;
						   if(iter_t != e32.end() && iter_t->delay_require == iter_hra->delay_require)
							   iter_t->band_now += iter_hra->band_now;
						   else e32.insert(iter_t,*iter_hra);
						   break;
					   }
				case 3:{
						   vector<host_require_actuality>::iterator iter_t = e33.begin();
						   while(iter_t != e33.end() && iter_t->delay_require < iter_hra->delay_require)
							   iter_t++;
						   if(iter_t != e33.end() && iter_t->delay_require == iter_hra->delay_require)
							   iter_t->band_now += iter_hra->band_now;
						   else e33.insert(iter_t,*iter_hra);
						   break;
					   }
				case 4:{
						   vector<host_require_actuality>::iterator iter_t = e34.begin();
						   while(iter_t != e34.end() && iter_t->delay_require < iter_hra->delay_require)
							   iter_t++;
						   if(iter_t != e34.end() && iter_t->delay_require == iter_hra->delay_require)
							   iter_t->band_now += iter_hra->band_now;
						   else e34.insert(iter_t,*iter_hra);
						   break;
					   }

				case 5:{
						   vector<host_require_actuality>::iterator iter_t = e35.begin();
						   while(iter_t != e35.end() && iter_t->delay_require < iter_hra->delay_require)
							   iter_t++;
						   if(iter_t != e35.end() && iter_t->delay_require == iter_hra->delay_require)
							   iter_t->band_now += iter_hra->band_now;
						   else e35.insert(iter_t,*iter_hra);
						   break;
					   }

			}
		}
		iter_hra++;
	}
	//every time clear the merged_require;
	merged_require.clear();

	mergeRequire(d11);
	mergeRequire(d12);
	mergeRequire(d13);
	mergeRequire(d14);
	mergeRequire(d15);

	mergeRequire(d21);
	mergeRequire(d22);
	mergeRequire(d23);
	mergeRequire(d24);
	mergeRequire(d25);

	mergeRequire(d31);
	mergeRequire(d32);
	mergeRequire(d33);
	mergeRequire(d34);
	mergeRequire(d35);

	mergeRequire(e11);
	mergeRequire(e12);
	mergeRequire(e13);
	mergeRequire(e14);
	mergeRequire(e15);

	mergeRequire(e21);
	mergeRequire(e22);
	mergeRequire(e23);
	mergeRequire(e24);
	mergeRequire(e25);

	mergeRequire(e31);
	mergeRequire(e32);
	mergeRequire(e33);
	mergeRequire(e34);
	mergeRequire(e35);
	 
//	display_domain_requirement_transmission();

}
	
 

void Hieron_local::mergeRequire(vector<host_require_actuality>& di)
{
	//cout<<"in mergerRequire"<<endl;
	int count = 0;
	domain_requirement_transmission drt;	
	vector<host_require_actuality>::iterator iter = di.begin();
	if(iter == di.end())return;
	drt.src_domain = iter->src_domain;
	drt.dst_domain = iter->dst_domain;
	while(iter != di.end())
	{
		count++;
		if(count % mergeTimes == 1)
		{
			drt.src_marginal_sid = iter->src_marginal_sid;
			drt.priority = iter->priority;
			drt.delay_floor = iter->delay_require;
			drt.band_now = 0;
		}
		drt.band_now += iter->band_now;
		if(count % mergeTimes == 0)
		{
			drt.delay_ceiling = iter->delay_require;
			domain_requirement_transmission drt_copy(drt);
			merged_require.push_back(drt_copy);
		}
		iter++;
		if(iter == di.end() && (count % mergeTimes != 0))
		{
			iter--;
			drt.delay_ceiling = iter->delay_require;
			domain_requirement_transmission drt_copy(drt);
			merged_require.push_back(drt_copy);
			iter++;
		}
	}
}
void Hieron_local::display_domain_requirement_transmission()
{
	vector<domain_requirement_transmission>::iterator iter = merged_require.begin();
	while(iter != merged_require.end())
	{
		cout<<"*******************************"<<endl;
		cout<<"src domain is  "<<iter->src_domain<<endl;
		cout<<"dst domain is  "<<iter->dst_domain<<endl;
		cout<<"priority is "<<iter->priority<<endl;
		cout<<"delay floor is "<<iter->delay_floor<<endl;
		cout<<"delay ceiling is "<<iter->delay_ceiling<<endl;
		cout<<"band count is  "<<iter->band_require<<endl;
		cout<<"********************************"<<endl<<endl;
		iter++;
	}
}
void* Hieron_local::connect_to_root_and_centralized(void *args)
{
	cout<<"The local is connectting to the root...."<<endl;
	int temp = -1;
	long sleep_time = 10000;
	while(temp < 0)
	{
		temp = connect_to_server(root_ip,root_port);
		usleep(sleep_time);
	}
	root_socket = temp;
	temp = -1;
	while(temp < 0)
	{
		temp = connect_to_server(centralized_ip,centralized_port);
		usleep(sleep_time);
	}
	centralized_socket = temp;
	printf("Local %d has connected to the root and the centralized!\n",domainID);
}
int Hieron_local::connect_to_server(string ip,int port)
{
	int sock_cli = socket(AF_INET,SOCK_STREAM, 0);
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = inet_addr(ip.c_str()); 
	timeval time = {0,0};
	time.tv_sec = 0;
	time.tv_usec =1000;
	socklen_t len = sizeof(time);
	setsockopt(sock_cli,SOL_SOCKET,SO_SNDTIMEO,&time,len);

	fd_set rset,wset;
	int error = 0;
	unsigned long ul = 1;
	ioctl(sock_cli,FIONBIO,&ul);

	int result = connect(sock_cli,(struct sockaddr *)&servaddr,sizeof(servaddr));
	//cout<<"sock_Cli is "<<sock_cli<<endl;
	//return sock_cli;
	if(result<0&&errno!=EINPROGRESS)
	{
		cout<<"errno != EINPROCES"<<endl;
		return -1;
	}
	else if(result < 0)
	{
		int n;
		FD_ZERO(&rset);
		FD_SET(sock_cli,&rset);
		wset = rset;
		struct timeval tval;
		tval.tv_sec = 0;
		tval.tv_usec = 1000;
		if((n=select(sock_cli+1,&rset,&wset,NULL,&tval))==0)
		{
			perror("connect");
	    		cout<<"connect to "<<root_ip<<" of port:"<<root_port <<" failed "<<endl;
			close(sock_cli);
			errno = ETIMEDOUT;
			return -1;
		}
		socklen_t len2 = sizeof(error);
		getsockopt(sock_cli,SOL_SOCKET,SO_ERROR,&error,&len2);
		if(error == 0)
		{
			return sock_cli;
		}
		else
		{
			close(sock_cli);
			return -1;
		}

	    	perror("connect");
	    	cout<<"connect to "<<root_ip<<" of port:"<<root_port <<" failed "<<endl;
		return -1;
	}
	else if(result == 0)
	{
		cout<<"result is 0"<<endl;
	}
	else 
	    	cout<<"connect to "<<root_ip<<" of port:"<<root_port <<" success "<<endl;
	ul = 0;
	ioctl(sock_cli,FIONBIO,&ul);
	/*pthread_t tid[100];
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);
	int ret;
	int index = -1;
	for(int i =0; i<100;i++)
	{
		if(!socketID[i])
		{
			index = i;
			socketID[i] =true;
			break;
		}
	}
	if(index == -1)
	{
		cout<<"can not find a useful socketID"<<endl;
		exit(1);
	}*/
	return sock_cli;
}

void Hieron_local::send_packet_to_other(int sock_cli,char sendbuf[],int send_bytes) 
{
	timeval time = {0,0};
	time.tv_sec = 1;
	time.tv_usec =0;
	socklen_t len = sizeof(time);
	setsockopt(sock_cli,SOL_SOCKET,SO_SNDTIMEO,&time,len);
//	cout<<sizeof(sendbuf)<<endl;
//	packet_struct* ps = (packet_struct *)sendbuf;
//	cout<<"in send_packet_to_other packet data is : "<<ps->data<<endl;
//	cout<<"in send packet to other packet length is : "<<strlen(sendbuf)<<endl;
	int length = send(sock_cli, sendbuf,send_bytes,0); 
	int total_length = 0;
	while(true)
	{
		if(length == -1)
		{
			length = send(sock_cli, sendbuf+total_length,send_bytes-total_length,0); 
			continue;
		}
		else
		{
			total_length += length;
			if(total_length >=send_bytes)
			{
				break;
			}
			length = send(sock_cli, sendbuf+total_length,send_bytes-total_length,0); 
		}
	}
			
}

char *Hieron_local:: recv_packet_from_other(int sock_cli,int BUFFER_SIZE)
{
	char* recvbuf = new char[BUFFER_SIZE];
	recv(sock_cli,recvbuf,sizeof(recvbuf),0);
	return recvbuf;
}
void Hieron_local::core_function()
{
	stringstream ss_name;
	ss_name<<"./errorlog/"<<TE_time_local; 
	string filename = ss_name.str();
	output_error_log.open(filename.c_str(),ofstream::app);
	//set the seed of rand function
	unsigned int seed;
	seed = (unsigned)time(NULL)*domainID;
	srand(seed);
	pthread_mutex_init(&mergelock,NULL);
	pthread_mutex_init(&host_requirement_lock,NULL);

	//The first step is connecting the root server
	//connect_to_root();
	condition = 1;
	count_TE = 0;
	getVirtualLink();
	minus_band_allocated_2_virtual_link();
	count_inter_sent_to_root = 0;
	
//	cout<<"after virtual"<<endl;
	//pthread_mutex_init(&queuelock,NULL);//need to start a lock or not
	pthread_t tids[3];
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);
	//pthread_mutex_init(&queue_packet_mutex,NULL);
	int ret;
	ret = pthread_create(&tids[0],&attr,server_function,NULL);
	if(ret!=0)
	{
		cout<<"pthread_create error:error_code=" <<ret<<endl;
	}
	ret = pthread_create(&tids[1],&attr,main_logic,NULL);
	if(ret!=0)
	{
		cout<<"pthread_create error:error_code=" <<ret<<endl;
	}

	ret = pthread_create(&tids[2],&attr,connect_to_root_and_centralized,NULL);
	if(ret!=0)
	{
		cout<<"pthread_create error:error_code=" <<ret<<endl;
	}


	pthread_attr_destroy(&attr);
	void *status;
	for(int i =0 ;i <3;i++)
	{
		int ret2 = pthread_join(tids[i],&status);
		if(ret2 != 0)
		{
			cout<<"pthread_join error: error_code="<<ret2<<endl;
		}
	}

	//pthread_mutex_destroy(&queue_packet_mutex);
}
void* Hieron_local::server_function(void * args)
{
	cout<<"~~~~~~~~~~~~~~in receive packet function!~~~~~~~~~~~~~~~"<<endl<<endl;
//	cout<<"in receive wait is "<<wait<<endl;
//	pthread_mutex_lock(&queue_packet_mutex);
//	packetQueue queue = *((packetQueue *)(args));
	
	signal(SIGCHLD, SIG_IGN);
	int listenfd = socket(AF_INET,SOCK_STREAM,0);
	//int x = fcntl(listenfd,F_GETFL,0);
	//fcntl(listenfd,F_SETFL,x|O_NONBLOCK);
	struct sockaddr_in servaddr;
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(local_port);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int on = 1;
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
		exit(EXIT_FAILURE);
	
	if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
	{
		perror("bind");
		exit(1);
	}
	
	if (listen(listenfd, SOMAXCONN) == -1)
	{
		perror("listen");
		exit(1);
	}

	struct sockaddr_in peeraddr; 
	socklen_t peerlen = sizeof(peeraddr); 
	int conn; 

	pid_t pid;

	while (1)
	{
	//	pthread_mutex_lock(&queuelock);
		//cout << "in receive packet while!" << endl;
		if ((conn = accept(listenfd, (struct sockaddr *)&peeraddr, &peerlen)) < 0) 
		{
			perror("connect error!");
			continue;
		}
		//pthread_mutex_lock(&queuelock);
		printf("recv connect ip=%s port=%d\n", inet_ntoa(peeraddr.sin_addr),
			ntohs(peeraddr.sin_port));
		//socketMapping.insert(make_pair(inet_ntoa(peeraddr.sin_addr),conn));	
		 
		pthread_t tid[100];
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);
		int ret;
		int index = -1;
		for(int i =0; i<100;i++)
		{
			if(!socketID[i])
			{
				index = i;
				socketID[i] =true;
				break;
			}
		}
		if(index == -1)
		{
			cout<<"can not find a useful socketID"<<endl;
			exit(1);
		}

		ret = pthread_create(&tid[index],&attr,do_service,(void *)(&conn));
//		do_service((void *)(&conn));
		void *status;
	//	int ret2 = pthread_join(tid[0],&status);
	//	if(ret2 != 0)
	//	{
	//		cout<<"pthread_join error: error_code="<<ret2<<endl;
	//	}

		
	}
	cout<<"in the end of receive packet function!"<<endl;
	pthread_exit(0);
	
	return 0;

}

void* Hieron_local::do_service(void* args)
{
	int conn = *((int*)(args));
	//cout<<"here conn is : "<<conn<<endl;
	char recvbuf[32];
	while (1)
	{
		//cout<<"in do service!"<<endl;
		//pthread_mutex_lock(&queuelock);
		//cout<<"in do service!"<<endl;
		
		memset(recvbuf, 0, sizeof(recvbuf));
		//cout<<"before read!"<<endl;
		int ret = read(conn, recvbuf, sizeof(recvbuf));
		if (ret == -1&&errno != EAGAIN)
		{
			perror("read error!");
			exit(1);
		}	
		while(ret != 32)
		{
			int ret1 = read(conn,recvbuf+ret,sizeof(recvbuf)-ret);
			if (ret1 == -1&&errno != EAGAIN)
			{
		 		perror("read error!");
				exit(1);
			}	
			ret += ret1;
		}

		if (ret == 0)   
		{
			printf("client close\n");
			break;
		}
		if(ret != 32)cout<<"receive packet num is : "<<ret<<endl;
		
	//	packet_transmission *packet = (packet_transmission *)recvbuf;
	//	output_error_log<<"now condition is "<<condition<<" receive packet ("<<packet->parameter1<<","<<packet->parameter2<<","<<packet->parameter3<<","<<packet->parameter4<<","<<packet->parameter5<<endl;
		if(condition == 1)
		{
			if(first_print)
			{
				first_print = false;
				cout<<"-----------------------In condition 1-----------------------"<<endl;
			}
			command * c = (command *)recvbuf;
			if(c->type == 100001 && c->domain == domainID && c->padding1 == -10000)
			{
				timeval tv;
				gettimeofday(&tv,NULL);
				all_start_time = tv.tv_sec*1000 + tv.tv_usec/1000;
				first_print = true;
				condition = 2;
				command com(100001,domainID,0,-10000,0,0);
				send_packet_to_other(root_socket,(char *)(&com),32);
				cout<<"------------------after send confirm to root-------------"<<endl;
			}
			else
			{
				cout<<"the packet receive is : "<<endl;
				printf("%d,%d,%d,%d,%f,%d\n",c->type,c->domain,c->num,c->padding1,c->padding2,c->padding3);
				cout<<"-----log in do_service,in condition 1,error-----"<<endl;
				output_error_log<<TE_time_local<<":"<<" log in do_service,in condition 1,error,receive the packet "<<c->type<<" "<<c->domain<<" "<<c->num<<" "<<c->padding1<<" "<<c->padding2<<" "<<c->padding3<<endl;	

				exit(0);
			}
		}
		else if(condition == 2)
		{
			if(first_print)
			{
				first_print = false;
				cout<<endl;
				cout<<"-----------------------In condition 2-----------------------"<<endl;
			}
			command * c = (command *)recvbuf;
			if(c->type == 100002 && c->domain == domainID && c->padding1 == -10000)
			{
				//send the virtual link to the root
				condition = 3;
				first_print = true;
				int count_virtual_edge = 0;
				for(int i = 0; i < virtual_link_list.size();i++)
					for(int j = 0; j < virtual_link_list[i].path_band_delays.size();j++)
						count_virtual_edge++;
				command com(100002,domainID,count_virtual_edge,-10000,0,0);
				send_packet_to_other(root_socket,(char*)(&com),32);
				cout<<"------In local condition 2, send the virual link------"<<endl;
				vector<host_require_actuality>::iterator iter  = virtual_link_list.begin();
				while(iter != virtual_link_list.end())
				{
					//printf("virtual link %d->%d has %lu (delay,band) pairs : \n",iter->src_sid,iter->dst_sid,iter->path_band_delays.size());
					vector<path_band_delay>::iterator iter_pbd = iter->path_band_delays.begin();
					while(iter_pbd != iter->path_band_delays.end())
					{
					//	printf("(%d,%f)\n",iter_pbd->delay,iter_pbd->band*percent_virtual_link);
						virtual_edge_transmission vet(iter->src_sid,iter->dst_sid,iter_pbd->delay,-10001,iter_pbd->band*percent_virtual_link,domainID);
						send_packet_to_other(root_socket,(char*)(&vet),32);
						usleep(5);
						iter_pbd++;
					}
					iter++;
				}
			}
			else
			{
				cout<<"-----log in do_service,in condition 2,error-----"<<endl;
				output_error_log<<TE_time_local<<":"<<"-----log in do_service,in condition 2,error-----"<<endl;	
				exit(0);
			}
		}
		else if(condition == 3)
		{
			//send the inter domain requirement to the root
			if(first_print)
			{
				first_print = false;
				cout<<endl;
				cout<<"-----------------------In condition 3-----------------------"<<endl;
			}
			command * c = (command *)recvbuf;
		//	printf("get the command : type = %d,domain = %d,padding1 = %d\n",c->type,c->domain,c->padding1);
			if(c->type == 100003 && c->domain == domainID && c->padding1 == -10000)
			{
				num_inter_domain_requirement_receive = 0;
			
				vector<host_require_actuality> all_require_list;
				vector<host_require_actuality>::iterator iter = host_require_actuality_list.begin();
				while(iter != host_require_actuality_list.end())
				{
					if(iter->src_domain == iter->dst_domain)
					{
						host_require_actuality hra(iter->src_sid,iter->dst_sid,iter->src_domain,iter->dst_domain,iter->src_marginal_sid,iter->priority,iter->delay_require,iter->remain_amount,iter->band_now,iter->band_require,iter->last_start_time,iter->finish_time_predict);
						all_require_list.push_back(hra);
					}
					else
					{
						host_require_actuality hra(switch_index[iter->src_sid],switch_index[iter->src_marginal_sid],iter->src_domain,iter->dst_domain,iter->src_marginal_sid,iter->priority,iter->delay_require,iter->remain_amount,iter->band_now,iter->band_require,iter->last_start_time,iter->finish_time_predict);
						all_require_list.push_back(hra);
					}
					iter++;
				}
				//reset the inter requirement as from src sid to the marginal sid in the src domain
				ofstream output;
				stringstream ss_name;
				ss_name<<"./data/TE_time_local/"<<TE_time_local; 
				string filename = ss_name.str();
				output.open(filename.c_str(),ofstream::app);
				timeval tv1;
				gettimeofday(&tv1,NULL);
				long time_before_TE = tv1.tv_sec*1000 + tv1.tv_usec/1000;
				pthread_mutex_lock(&host_requirement_lock);
				TE_fun(all_require_list);


				gettimeofday(&tv1,NULL);
				long time_after_TE = tv1.tv_sec*1000 + tv1.tv_usec/1000;
				output<<time_after_TE - time_before_TE<<endl;
			//	printf("TE in local run time is %ld\n",time_after_TE - time_before_TE);
				//pthread_mutex_unlock(&host_requirement_lock);

			//	cout<<"Finish the TE in local."<<endl<<endl;
				
				//set back the band now of both inter and intra requiement
				vector<host_require_actuality>::iterator iter1 = all_require_list.begin();
				iter = host_require_actuality_list.begin();
				while(iter1 != all_require_list.end())
				{
					if(iter1->src_domain == iter1->dst_domain)
					{
						if(iter1->src_sid != iter->src_sid || iter1->dst_sid != iter->dst_sid)
						{
							output_error_log<<"Error in condition 3 the two hra should be same"<<endl;
							exit(0);
						}
						else
						{
							vector<path_band_delay>::iterator iter_path = iter1->path_band_delays.begin();
							//output_error_log<<"path total band is "<<iter1->band_now<<endl;
							while(iter_path != iter1->path_band_delays.end())
							{
							//	output_error_log<<"the path band is "<<iter_path->band<<endl;
								path_band_delay pbd(iter_path->path_length,iter_path->path,iter_path->band,iter_path->delay);
								iter->path_band_delays.push_back(pbd);
								iter_path++;
							}
							iter->band_now = iter1->band_now;
						}
					}
					else
					{
						if(index_switch[iter1->src_sid] != iter->src_sid || index_switch[iter1->dst_sid] != iter->src_marginal_sid)
						{
							output_error_log<<"Error in condition 3 the two hra should be same"<<endl;
							exit(0);
						}
						else
						{
							vector<path_band_delay>::iterator iter_path = iter1->path_band_delays.begin();
							while(iter_path != iter1->path_band_delays.end())
							{
								path_band_delay pbd(iter_path->path_length,iter_path->path,iter_path->band,iter_path->delay);
							
								iter->path_band_delays.push_back(pbd);
								iter_path++;
							}
							iter->band_now = iter1->band_now;
						}
					}
					iter++;
					iter1++;
				}
				display_all_require_path();
				output_error_log<<endl<<endl;
				mergeDelay_band();//merge the inter domain requirement
				condition =  4;	
				first_print = true;
				
				vector<domain_requirement_transmission>::iterator iter_mr = merged_require.begin();
				while(iter_mr != merged_require.end())
				{
					if(iter_mr->band_now > 0)
						count_inter_sent_to_root++;
					iter_mr++;
				}

				command com(100003,domainID,count_inter_sent_to_root,-10000,0,0);
				send_packet_to_other(root_socket,(char*)(&com),32);
				iter_mr = merged_require.begin();
				cout<<"----In local condition 3, send the inter requirement------"<<endl;
				output_error_log<<"the size of merged require is "<<merged_require.size()<<endl;
				while(iter_mr != merged_require.end())
				{
				//	printf("%d->%d:(priority=%d,  delay = %d, band = %f)\n",iter_mr->src_domain,iter_mr->dst_domain,iter_mr->priority,iter_mr->delay_floor,iter_mr->band_require);
					if(iter_mr->band_now == 0)
					{
						iter_mr++;
						continue;
					}
					inter_domain_requirement_transmission idrt(iter_mr->src_domain,iter_mr->dst_domain,iter_mr->priority,iter_mr->delay_floor,iter_mr->band_now,iter_mr->src_marginal_sid);
					send_packet_to_other(root_socket,(char*)(&idrt),32);
					output_error_log<<"Send inter domain requirement : (src_domain,dst_domain,src_marginal_sid,priorit,delay,band),("<<iter_mr->src_domain<<","<<iter_mr->dst_domain<<","<<iter_mr->src_marginal_sid<<","<<iter_mr->priority<<","<<iter_mr->delay_floor<<","<<iter_mr->band_now<<")"<<endl;
					usleep(5);
					iter_mr++;
				}
				num_inter_domain_requirement_receive = 0;
			
				/*~~~~~~~~~have modify here~~~~~~~~~~~~~~~*/

			}
			else
			{
				output_error_log<<"Error in condition 3"<<endl;
				exit(0);
			}	
			last_TE_require_allocated_list.clear();//need to clear the vector storing the TE allocated value

		}
		else if(condition == 4)
		{
			//get the inter domain requiremnt after the TE function in root
			if(first_print)
			{
				first_print = false;
				cout<<endl;
				cout<<"-----------------------In condition 4-----------------------"<<endl;
			}
			inter_domain_requirement_transmission * idrt = (inter_domain_requirement_transmission *)recvbuf;
			host_require_actuality hra;
			hra.src_domain = idrt->src_domain;
			hra.dst_domain = idrt->dst_domain;
			hra.priority = idrt->priority;
			hra.delay_require = idrt->delay;
			hra.band_now = idrt->band;
			last_TE_require_allocated_list.push_back(hra); //store the require allocated in last TE both in local and root 
		//	printf("local %d get the message from root as(%d,%d,%d,%d,%f,%d)\n",domainID,idrt->src_domain,idrt->dst_domain,idrt->priority,idrt->delay,idrt->band,idrt->padding);
			if(idrt->src_domain == domainID/*&&idrt->padding == -10002*/)	
			{
			//	cout<<"---------After the TE in root,the inter requirement is-------"<<endl;
				vector<domain_requirement_transmission>::iterator iter = merged_require.begin();
				while(iter != merged_require.end())
				{
					if(iter->src_domain == idrt->src_domain && iter->dst_domain == idrt->dst_domain && idrt->delay == iter->delay_floor && iter->priority == idrt->priority && idrt->src_marginal_sid == iter->src_marginal_sid)
					{
					//	printf("%d->%d:(priority = %d, delay = %d, band = %f)\n",iter->src_domain,iter->dst_domain,iter->priority,iter->delay_floor,idrt->band);
						output_error_log<<"Get inter domain requirement : (src_domain,dst_domain,priorit,delay,band),("<<iter->src_domain<<","<<iter->dst_domain<<","<<iter->priority<<","<<iter->delay_floor<<","<<idrt->band<<")"<<endl;
						iter->band_now = idrt->band;
						num_inter_domain_requirement_receive++;
						break;
					}
					iter++;
				}
				if(iter == merged_require.end())
				{
					cout<<"--------------log:error in do service : iter is the end in condition 4---------------"<<endl;
					output_error_log<<TE_time_local<<":"<<"--------------log:error in do service : iter is the end in condition 4---------------"<<endl;
					exit(0);
				}
				//call back the band allocated to the inter requirement that TE in root just meet part
				give_back_band_allocated_to_inter(iter);
			}
			else
			{
				cout<<"--------------log:error in do service in condition 4---------------"<<endl;
				output_error_log<<" get the message as "<<idrt->src_domain<<","<<idrt->dst_domain<<","<<idrt->priority<<","<<idrt->delay<<","<<idrt->band<<")"<<endl;
				output_error_log<<TE_time_local<<":"<<"--------------log:error in do service in condition 4---------------"<<endl;
				exit(0);
			}
			if(num_inter_domain_requirement_receive == count_inter_sent_to_root)
			{
				reset_inter_host_require();
				//local run TE to allocate band of the intra requirement again
				vector<host_require_actuality>::iterator iter_host = host_require_actuality_list.begin();
				while(iter_host != host_require_actuality_list.end())
				{
					if(iter_host->dst_domain == iter_host->src_domain && iter_host->band_now < iter_host->band_require)
					{
						host_require_actuality hra(iter_host->src_sid,iter_host->dst_sid,iter_host->src_domain,iter_host->dst_domain,iter_host->src_marginal_sid,iter_host->priority,iter_host->delay_require,iter_host->remain_amount,0,iter_host->band_require-iter_host->band_now,iter_host->last_start_time,iter_host->finish_time_predict);
						intra_require_left.push_back(hra);
					}
					iter_host++;
				}
				output_error_log<<TE_time_local<<":"<<"the intra require left size is "<<intra_require_left.size()<<endl;
				//run the TE again to allocate the band to the band to the intra requirement
				ofstream output;
				stringstream ss_name;
				ss_name<<"./data/TE_time_local/"<<TE_time_local; 
				string filename = ss_name.str();
				output.open(filename.c_str(),ofstream::app);
				timeval tv1;
				gettimeofday(&tv1,NULL);
				long time_before_TE = tv1.tv_sec*1000 + tv1.tv_usec/1000;
				//pthread_mutex_lock(&host_requirement_lock);
				TE_fun(intra_require_left);
				output_error_log<<"before reset ! "<<endl;
				display_all_require_path();
				output_error_log<<endl<<endl;
				reset_intra_host_require();

				gettimeofday(&tv1,NULL);
				long time_after_TE = tv1.tv_sec*1000 + tv1.tv_usec/1000;
				output<<time_after_TE - time_before_TE<<endl;
		//		printf("TE in local run time is %ld\n",time_after_TE - time_before_TE);

				pthread_mutex_unlock(&host_requirement_lock);

		//		cout<<"Finish the TE in local."<<endl<<endl;

				output_error_log<<"after reset ! "<<endl;
				display_all_require_path();
				output_error_log<<endl<<endl;

				gettimeofday(&tv1,NULL);
				all_end_time = tv1.tv_sec*1000 + tv1.tv_usec/1000;
				ofstream output1;
				stringstream ss_name1;
				ss_name1<<"./data/total_time_local/"<<TE_time_local; 
				string filename1 = ss_name1.str();
				output1.open(filename1.c_str(),ofstream::app);
				output1<<all_end_time - all_start_time<<endl;
				printf("Total time is %ld\n",all_end_time - all_start_time);

				stringstream ss_name_fair;
				ss_name_fair<<"./data/TE_fairness/"<<TE_time_local; 
				string filename_fair;
				filename_fair = ss_name_fair.str();
				ofstream input_fair;
				input_fair.open(filename_fair.c_str());
				int count_one = 0;
				vector<host_require_actuality>::iterator iter_fair = host_require_actuality_list.begin();
				while(iter_fair != host_require_actuality_list.end())
				{
					double rate = iter_fair->band_now / iter_fair->band_require;
					int src_sid,dst_sid;
					if(iter_fair->src_domain == iter_fair->dst_domain)
					{
						src_sid = index_switch[iter_fair->src_sid];
						dst_sid = index_switch[iter_fair->dst_sid];
					}
					else{

						src_sid = iter_fair->src_sid;
						dst_sid = iter_fair->dst_sid;
					}
					if(rate == 0)count_one++;
					input_fair<<iter_fair->src_domain<<" "<<iter_fair->dst_domain<<" "<<src_sid<<" "<<dst_sid<<" "<<iter_fair->band_now<<" "<<iter_fair->band_require<<" "<<rate<<endl;
					iter_fair++;
				}
				output_error_log<<"total require has "<<host_require_actuality_list.size()<<" one require has "<<count_one<<endl;



				/*******now choose some path to calculate the usage and send the path to centralized*******/

				int count_path = 0;

				int random_index = rand()%host_require_actuality_list.size();
				host_require_actuality hra = host_require_actuality_list[random_index];
				vector<int> random_list;
				int count = 0;
				while(count < 1000 && count_path < 10)
				{
				//	output_error_log<<TE_time_local<<":"<<"random index is "<<random_index<<" band now is "<<hra.band_now<<endl;
				//	output_error_log<<TE_time_local<<":"<<"count is "<<count<<" count path is "<<count_path<<endl;
					count++;
					if(hra.band_now > 0 && hra.dst_domain == hra.src_domain)
					{
						vector<int>::iterator iter_random = random_list.begin();
						while(iter_random != random_list.end())
						{
							if(*iter_random == random_index)
								break;
							iter_random++;
						}
						if(iter_random != random_list.end())
						{
							random_index = rand()%host_require_actuality_list.size();
							hra = host_require_actuality_list[random_index];
							continue;
						}
						else 
							random_list.push_back(random_index);
						int * path;
						if(hra.path_band_delays.size()>0 && hra.src_domain == hra.dst_domain)
						{
							int max_band_index = -1;
							double max_band = 0;
							for(int i = 0; i < hra.path_band_delays.size(); i++)
							{
							//	output_error_log<<"max_band is "<<max_band<<" path "<<i<<" band is "<<hra.path_band_delays[i].band<<endl;
								if(hra.path_band_delays[i].band > max_band)
								{
									max_band = hra.path_band_delays[i].band;
									max_band_index = i;
								}
							}
							output_error_log<<"path size is "<<hra.path_band_delays.size()<<endl;
							output_error_log<<"max band index is "<<max_band_index<<endl;
							if(max_band_index == -1)
							{
								exit(0);
							}
							double path_band = (hra.path_band_delays[max_band_index]).band;
							path = (hra.path_band_delays[max_band_index]).path;
							int count_link = 0;
							double count_use = 0;
							count_link = (hra.path_band_delays[max_band_index]).path_length;
							int min_index = -1;
							int min_index_ori = -1;
							double min_band = 999;
							double min_band_ori = 999;
							for(int i =0; i < count_link - 1; i++)
							{
								if(band_array_copy[path[i]][path[i+1]] == 0)
								{
									output_error_log<<TE_time_local<<"the band between "<<index_switch[path[i]]<<"("<<path[i]<<","<<i<<")"<<" and "<<index_switch[path[i+1]]<<"("<<path[i+1]<<","<<i+1<<") should not be 0"<<endl;
								}
								else count_use += (band_array_copy[path[i]][path[i+1]] - band_array[path[i]][path[i+1]])/band_array_copy[path[i]][path[i+1]];
								if(band_array[path[i]][path[i+1]] < min_band)
								{
									min_band = band_array[path[i]][path[i+1]];
									min_index = i;
								}
								if(band_array_copy[path[i]][path[i+1]] < min_band_ori)
								{
									min_band_ori = band_array_copy[path[i]][path[i+1]];
									min_index_ori = i;
								}

							}
							if(count_link > 0)
							{
								if(min_index == -1)
								{
									output_error_log<<"min index should not be 0! "<<endl;
								}
								else{
										int src_sid,dst_sid;
										if(hra.src_domain == hra.dst_domain)
										{
											src_sid = index_switch[hra.src_sid];
											dst_sid = index_switch[hra.dst_sid];
										}
										else{
												src_sid = hra.src_sid;
												dst_sid = hra.dst_sid;
											}
									output_error_log<<src_sid<<"->"<<dst_sid<<" : path band is "<<path_band<<" min band is "<<min_band<<endl;
									//double rate = (band_array_copy[path[min_index]][path[min_index+1]] - band_array[path[min_index]][path[min_index+1]])/band_array_copy[path[min_index]][path[min_index+1]];
									double rate = (path_band)/(path_band + band_array[path[min_index]][path[min_index+1]]);
									if(rate < 0.1)
									{
										random_index = rand()%host_require_actuality_list.size();
										hra = host_require_actuality_list[random_index];
										continue;
									}
									double rate_mean = 0;
									rate_mean = (band_array_copy[path[min_index_ori]][path[min_index_ori+1]] - band_array[path[min_index_ori]][path[min_index_ori+1]])/band_array_copy[path[min_index_ori]][path[min_index_ori+1]];
									usage_mean.push_back(rate_mean);
									usage_link.push_back(count_use);

									usage.push_back(rate);
									path_send_to_centralized.push_back(path);
									path_length_send_to_centralized.push_back(count_link);
									output_error_log<<"usage is "<<rate<<endl;
									count_path++;
								}
							//	usage.push_back(count_use);
							}
						}
						else
						{
							output_error_log<<TE_time_local<<"the band now is "<<hra.band_now<<" the path size is "<<hra.path_band_delays.size()<<endl;

						}
					}

					random_index = rand()%host_require_actuality_list.size();
					hra = host_require_actuality_list[random_index];
					//	display_host_requirement(host_require_actuality_list.begin() + random_index);

				}
				output_error_log<<TE_time_local<<":"<<"count_path is "<<count_path<<endl;

				stringstream ss_name2;
				ofstream output_usage;
				ss_name2<<"./data/Usage/"<<TE_time_local; 
				filename = ss_name2.str();
				output_usage.open(filename.c_str(),ofstream::app);

				int count_p = 0;
				int count_l = 0;
				double count_usage = 0;
				double count_usage_mean = 0;
				double count_usage_link = 0;

			
				output_error_log<<TE_time_local<<"the usage size is "<<usage.size()<<endl;
				if(usage.size() <= 5)
				{
					vector<double>::iterator iter_use = usage.begin();
					vector<double>::iterator iter_use_mean = usage_mean.begin();
					vector<double>::iterator iter_use_link = usage_link.begin();
					vector<int>::iterator iter_links = path_length_send_to_centralized.begin();
					while(iter_use != usage.end())
					{
						if(*iter_links < 0)
							output_error_log<<TE_time_local<<"the < 0 links is "<<*iter_links<<endl;
					//	count_p += *iter_links;
						count_p++;
						count_l += *iter_links;
						count_usage += *iter_use;
						count_usage_mean += *iter_use_mean;
						count_usage_link += *iter_use_link;
						iter_use_mean++;
						iter_use++;
						iter_links++;
						iter_use_link++;
					}

					count_usage /= count_p;
					count_usage_mean /= count_p;
					count_usage_link /= count_l;
					output_error_log<<TE_time_local<<"count usage is "<<count_usage<<"count link is "<<count_p<<" count usage mean is "<<count_usage_mean<<" count link is "<<count_p<<"usage link is "<<count_usage_link<<" link are "<<count_l<<endl;
					usage.clear();
					output_usage<<count_usage<<endl;
					output_usage<<count_usage_mean<<" "<<count_p<<endl;
					output_usage<<count_usage_link<<" "<<count_l<<endl;
				}
				
				else if(usage.size() > 5)
				{
					vector<int> max_usage;
					for(int i = 0; i < 5; i++)
					{
						double max = 0;
						int index = -1;
						for(int j = 0; j < usage.size(); j++)
						{
							bool has = false;
							for(int k = 0; k < max_usage.size(); k++)
							{
								if(max_usage[k] == j)
								{
									has = true;
									break;
								}
							}
							if(has)continue;
							else if(usage[j] > max)
							{
								max = usage[j];
								index = j;
							}
						}
						if(index != -1)max_usage.push_back(index);
						else output_error_log<<"it is not right,the index is -1!"<<endl;
					}
					for(int i =0; i < max_usage.size(); i++){
						count_usage += usage[max_usage[i]];
						count_usage_mean += usage_mean[max_usage[i]];
						count_usage_link += usage_link[max_usage[i]];
						count_l += path_length_send_to_centralized[max_usage[i]];
						count_p++;
					}
					
					/*
					vector<double>::iterator iter_use = usage.begin();
					vector<int>::iterator iter_links = path_length_send_to_centralized.begin();
					while(iter_use != usage.end())
					{
						if(*iter_links < 0)
							output_error_log<<TE_time_local<<"the < 0 links is "<<*iter_links<<endl;
					//	count_p += *iter_links;
						count_p++;
						count_usage += *iter_use;
						iter_use++;
						iter_links++;
					}		
					*/
				
					count_usage /= count_p;
					count_usage_mean /= count_p;
					count_usage_link /= count_l;
					output_error_log<<TE_time_local<<"count usage is "<<count_usage<<"count link is "<<count_p<<" count usage mean is "<<count_usage_mean<<" count link is "<<count_p<<"usage link is "<<count_usage_link<<" link are "<<count_l<<endl;
					usage.clear();
					output_usage<<count_usage<<endl;
					output_usage<<count_usage_mean<<" "<<count_p<<endl;
					output_usage<<count_usage_link<<" "<<count_l<<endl;
				}


				/****now send all host require to centralized***/
				int count_send_to_centralized = 0;
				vector<host_require_actuality>::iterator iter_centralized = all_require_to_centralized.begin();
				while(iter_centralized != all_require_to_centralized.end())
				{
					if(iter_centralized->band_now < 0.0000000000001)
					{
						count_send_to_centralized++;
					}
					iter_centralized++;
				}
				condition = 5;
				first_print = true;
				/******send the number of host require to centralized********/
				command com(100004,domainID,count_send_to_centralized,-10000,count_usage*count_p,count_p);
				send_packet_to_other(centralized_socket,(char*)(&com),32);
				output_error_log<<TE_time_local<<"after first sending to comparision! "<<count_p<<endl;
				output_error_log<<TE_time_local<<"the require size is : "<<all_require_to_centralized.size()<<"count_send to_centralized is "<<count_send_to_centralized<<endl;

				/************now send all the reuqirement to the centralized serveras 117**********/
				iter_centralized = all_require_to_centralized.begin();
				int count_send = 0;
				while(iter_centralized != all_require_to_centralized.end())
				{
					if(iter_centralized->band_now < 0.0000000000001)
					{
						host_require_transmission hrt(iter_centralized->src_sid,iter_centralized->dst_sid,iter_centralized->priority,iter_centralized->delay_require,iter_centralized->band_require,domainID);
						send_packet_to_other(centralized_socket,(char *)(&hrt),32);
						count_send++;
					}
					iter_centralized++;
				}
				output_error_log<<TE_time_local<<"after sending the requirements to comparision! And total send "<<count_send<<endl;

				
				//exit(0);
			}
		}
		
		else if(condition == 5)
		{
			if(first_print)
			{
				first_print = false;
				cout<<endl;
				cout<<"-----------------------In condition 5-----------------------"<<endl;
				output_error_log<<TE_time_local<<":"<<"-----in condition 5-----"<<endl;
			}
			command * c = (command *)recvbuf;
		//	printf("get the command : type = %d,domain = %d,padding1 = %d\n",c->type,c->domain,c->padding1);
			if(c->type == 100004 && c->domain == domainID && c->padding1 == -10000)
			{
				vector<int>::iterator iter_length = path_length_send_to_centralized.begin();
				int links = 0;
				while(iter_length != path_length_send_to_centralized.end())
				{
					links += *iter_length - 1;
					iter_length++;
				}
			/*	if(links == 0)
				{
					cout<<"-------log:error the links send to the centralized is 0-------"<<endl;
					output_error_log<<TE_time_local<<":"<<"-------log:error the links send to the centralized is 0!-------"<<endl;
					exit(0);
				}
				else*/
				{
					int count_band = 0;
					double total_usage = 1;
					for(int m =0; m < switchCount; m++)
						for(int n = m + 1; n < switchCount; n++)
						{
							if(band_array_copy[m][n] > 0)
							{
								count_band++;
								total_usage += (band_array_copy[m][n] - band_array[m][n])/band_array_copy[m][n];
							}
						}
					if(count_band == 0)
					{
						cout<<"Error in count the total usage!"<<endl;
						output_error_log<<TE_time_local<<":"<<"Error in count the total usage!"<<endl;
						exit(0);
					}
					ofstream output2;
					stringstream ss_name2;
					ss_name2<<"./data/TE_usage_total/"<<TE_time_local; 
					string filename2 = ss_name2.str();
					output2.open(filename2.c_str(),ofstream::app);
					output2<<total_usage/count_band<<endl;

					command com(100005,domainID,links,-10000,total_usage/count_band,0);
					send_packet_to_other(centralized_socket,(char*)(&com),32);
				}
				iter_length = path_length_send_to_centralized.begin();
				int *path;
				vector<int *>::iterator iter_path = path_send_to_centralized.begin();
				while(iter_path != path_send_to_centralized.end())
				{
				//	output_error_log<<TE_time_local<<":"<<"----length is -----"<<*iter_length<<endl;
					path = *iter_path;
					int pair = (*iter_length - 1)/2;
					int left = (*iter_length - 1)%2;
				/*	if(pair == 0)
					{
						output_error_log<<TE_time_local<<":"<<"-----pair should not be 0!-----"<<endl;
						exit(0);
					}*/
					if(left == 0)
					{
						for(int i =0; i < pair; i++)
						{
							command com(domainID,index_switch[path[2*i]],index_switch[path[2*i+1]],index_switch[path[2*i+1]],-10000,index_switch[path[2*i+2]]);
							send_packet_to_other(centralized_socket,(char*)(&com),32);
						}

					}
					else
					{
						for(int i = 0; i < pair; i++)
						{
							command com(domainID,index_switch[path[2*i]],index_switch[path[2*i+1]],index_switch[path[2*i+1]],-10000,index_switch[path[2*i+2]]);
							send_packet_to_other(centralized_socket,(char*)(&com),32);
						}
						
						command com(domainID,index_switch[path[2*pair]],index_switch[path[2*pair+1]],-1,-10000,-1);
						send_packet_to_other(centralized_socket,(char*)(&com),32);

					}
					iter_length++;
					iter_path++;
				}
				output_error_log<<TE_time_local<<":"<<"-----Finish sending links-----"<<endl;
				int t1,t2,t3;

				ifstream input_local;
				stringstream ss_name;
				ss_name<<"./data/TE_time_local/"<<TE_time_local; 
				string filename = ss_name.str();
				input_local.open(filename.c_str());
				int count = 1;
				if(input_local.is_open())
				{
					while(input_local.good()&& !input_local.eof())
					{
						char buffer[1024];
						input_local.getline(buffer,1024);
						string message;
						message = buffer;
					//	cout<<"message is "<<message<<endl;
						istringstream ss(message);
						switch(count)
						{
							case 1 : ss>>t1;break;
							case 2 : ss>>t2;break;
						}
						count++;
					}
				}

				stringstream ss_name1;
				ss_name1<<"./data/total_time_local/"<<TE_time_local; 
				filename = ss_name1.str();
				ifstream input_total;
				input_total.open(filename.c_str());
				count = 1;
				if(input_total.is_open())
				{
					while(input_total.good()&& !input_total.eof())
					{
						char buffer[1024];
						input_total.getline(buffer,1024);
						string message;
						message = buffer;
					//	cout<<"message is "<<message<<endl;
						istringstream ss(message);
						switch(count)
						{
							case 1 : ss>>t3;break;
						}
						count++;
					}
				}
				output_error_log<<TE_time_local<<": time are "<<t1<<" "<<t2<<" "<<t3<<endl;
				command com(100006,t1,t2,t3,-10000,domainID);
				send_packet_to_other(centralized_socket,(char*)(&com),32);
				output_error_log<<TE_time_local<<":"<<"-----Finish sending the times-----"<<endl;
			}

			condition = 1;
			first_print = true;
			output_error_log<<TE_time_local<<":"<<"-----Finish this run time-----"<<endl;
			exit(0);
		}

		//pthread_mutex_unlock(&queuelock);
	}
	//cout<<"close the conn : "<<conn<<endl;
	close(conn);

}

void * Hieron_local::main_logic(void *args)
{
	collect_requirements();
}

void Hieron_local::give_back_band_allocated_to_inter(vector<domain_requirement_transmission>::iterator iter)
{
	cout<<endl<<endl;
/*	for(int i =0; i < switchCount; i++)
		for(int j =0; j<switchCount; j++)
			if(band_array[i][j] < 0)printf("%d->%d has the band %f\n",i,j,band_array[i][j]);*/
	cout<<"Now allocate the inter domain requirement in local...."<<endl;
	//vector<domain_requirement_transmission>::iterator iter = merged_require.begin();
	//while(iter != merged_require.end())
	{
		/*int *path = new[switchCount];
		int delay_path;
		dijkstra(iter,int end,path,iter->delay_require,delay_path);*/
		double band_allocated_to_inter_in_local = 0;
		//the first traversal time is to count the total band require delay between floor and ceiling
		vector<host_require_actuality>::iterator iter_inter = inter_require_list.begin();
		while(iter_inter != inter_require_list.end())
		{
			if(iter_inter->dst_domain == iter->dst_domain && iter_inter->src_marginal_sid == iter->src_marginal_sid && iter_inter->priority == iter->priority &&  iter_inter->delay_require <= iter->delay_ceiling && iter_inter->delay_require >= iter->delay_floor)
			{
				band_allocated_to_inter_in_local += iter_inter->band_now;
			}
			iter_inter++;
		}
		//printf("the band require count between delay (%d and %d) is %f\n",iter->delay_floor,iter->delay_ceiling,band_allocated_to_inter_in_local);
		double rate;
		if(band_allocated_to_inter_in_local == 0)
		{
			cout<<"-------log:error in allocate_inter_domain_requirement_in_local-------"<<endl;
			output_error_log<<TE_time_local<<":"<<"-------log:error in allocate_inter_domain_requirement_in_local-------"<<endl;
			exit(0);
		}
		else rate = iter->band_now / band_allocated_to_inter_in_local;
		iter_inter = inter_require_list.begin();
		while(iter_inter != inter_require_list.end())
		{	
			if(iter_inter->dst_domain == iter->dst_domain && iter_inter->src_marginal_sid == iter->src_marginal_sid && iter_inter->priority == iter->priority && iter_inter->delay_require <= iter->delay_ceiling && iter_inter->delay_require >= iter->delay_floor)
			{
				//give back the band of each path 
				iter_inter->band_now *= rate;
				vector<path_band_delay>::iterator iter_path = iter_inter->path_band_delays.begin();
				while(iter_path != iter_inter->path_band_delays.end())
				{
					double band_give_back = (1 - rate) * iter_path->band;
					for(int i = 0; i <  iter_path->path_length - 1; i++)
					{
						if(conn_array[iter_path->path[i]][iter_path->path[i+1]] == false)
						{
							conn_array[iter_path->path[i]][iter_path->path[i + 1]] == true;
							conn_array[iter_path->path[i + 1]][iter_path->path[i]] == true;
						}
						output_error_log<<iter_path->path[i]<<"->"<<iter_path->path[i+1]<<"("<<band_array[iter_path->path[i]][iter_path->path[i+1]]<<")"<<" + "<<band_give_back<<endl;
						band_array[iter_path->path[i]][iter_path->path[i + 1]] += band_give_back;
						band_array[iter_path->path[i + 1]][iter_path->path[i]] += band_give_back;
					}
					iter_path->band *= rate;
					iter_path++;
				}
				iter_inter->finish_time_predict = -1;
			}

			iter_inter++;
		}
	}
	cout<<"Finish give back the band allocated to the inter domain requirement in local."<<endl<<endl;
}
void Hieron_local::reset_inter_host_require()
{
	vector<host_require_actuality>::iterator iter_host = host_require_actuality_list.begin();
	vector<host_require_actuality>::iterator iter_inter = inter_require_list.begin();
	while(iter_inter != inter_require_list.end())
	{
		if(iter_inter->finish_time_predict != -1)
		{
			iter_inter++;
			continue;
		}
		if(iter_host->dst_sid == iter_inter->dst_sid && iter_host->src_sid == iter_inter->src_sid && iter_host->delay_require == iter_inter->delay_require && iter_host->priority == iter_inter->priority && iter_host->src_marginal_sid == iter_inter->src_marginal_sid)
		{
			iter_host->path_band_delays.clear();
			iter_host->band_now = iter_inter->band_now;
			vector<path_band_delay>::iterator iter_path = iter_inter->path_band_delays.begin();
			while(iter_path != iter_inter->path_band_delays.end())
			{
			/*	bool same  = true;
				vector<path_band_delay>::iterator iter_host_path = iter_host->path_band_delays.begin();
				while(iter_host_path != iter_host->path_band_delays.end())
				{
					same  = true;
					if(iter_host_path->path_length == iter_path->path_length)
					{
						for(int i =0; i < iter_path->path_length; i++)
						{
							if(iter_host_path->path[i] != iter_path->path[i])
							{
								same =false;
								break;
							}
						}
						if(same)iter_host_path->band += iter_path->band;
					}
					if(same)break;
					iter_host_path++;
				}
				if(!same)*/{
					path_band_delay pbd(iter_path->path_length,iter_path->path,iter_path->band,iter_path->delay);
					iter_host->path_band_delays.push_back(pbd);
				}
				iter_path++;
			}
			iter_inter++;
		}
		iter_host++;
	}
}
void Hieron_local::reset_intra_host_require()
{
	vector<host_require_actuality>::iterator iter_host = host_require_actuality_list.begin();
	vector<host_require_actuality>::iterator iter_intra = intra_require_left.begin();
	while(iter_intra != intra_require_left.end())
	{
		if(iter_host->dst_sid == iter_intra->dst_sid && iter_host->src_sid == iter_intra->src_sid && iter_host->delay_require == iter_intra->delay_require && iter_host->priority == iter_intra->priority)
		{
			iter_host->band_now += iter_intra->band_now;
			vector<path_band_delay>::iterator iter_path = iter_intra->path_band_delays.begin();
			while(iter_path != iter_intra->path_band_delays.end())
			{
				bool same  = true;
				vector<path_band_delay>::iterator iter_host_path = iter_host->path_band_delays.begin();
				while(iter_host_path != iter_host->path_band_delays.end())
				{
					same  = true;
					if(iter_host_path->path_length == iter_path->path_length)
					{
						for(int i =0; i < iter_path->path_length; i++)
						{
							if(iter_host_path->path[i] != iter_path->path[i])
							{
								same =false;
								break;
							}
						}
						if(same)iter_host_path->band += iter_path->band;
					}
					if(same)break;
					iter_host_path++;
				}
				if(!same){
					path_band_delay pbd(iter_path->path_length,iter_path->path,iter_path->band,iter_path->delay);
					iter_host->path_band_delays.push_back(pbd);
				}
				iter_path++;
			}
			iter_intra++;
		}
		iter_host++;
	}

}
void Hieron_local::set_rate_inter_domain_requirement(double rate)
{
	rate_inter_domain_requirement = rate;
}
int Hieron_local::get_switch_count()
{
	return switchCount;
}
void Hieron_local::set_TE_time_local(string s)
{
	TE_time_local = s;
}

void Hieron_local::display_all_require_path()
{
	vector<host_require_actuality>::iterator iter = host_require_actuality_list.begin();
	while(iter != host_require_actuality_list.end())
	{
		int src_sid,dst_sid;
		if(iter->src_domain == iter->dst_domain)
		{
			src_sid = index_switch[iter->src_sid];
			dst_sid = index_switch[iter->dst_sid];
		}
		else{
			src_sid = iter->src_sid;
			dst_sid = iter->dst_sid;
		}
		output_error_log<<src_sid<<"->"<<dst_sid<<"("<<iter->src_domain<<"->"<<iter->dst_domain<<":"<<iter->src_marginal_sid<<")"<<"band is "<<iter->band_now<<" has paths : "<<endl;
		vector<path_band_delay>::iterator iter_path = iter->path_band_delays.begin();
		while(iter_path != iter->path_band_delays.end())
		{
			output_error_log<<iter_path->band<<" : ";
			for(int i = 0; i < iter_path->path_length; i++)
				output_error_log<<iter_path->path[i]<<" ";
			output_error_log<<endl;

			iter_path++;
		}
		output_error_log<<endl;
		iter++;
	}
}
			
