#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <numeric>
#include <list>


using namespace std;

// class graph{

//     int vertexRange;
//     int src;
//     int dst;
//     vector<list<int>> adj;

// public:

//     void add_edge( int v1, int v2);
// };

void add_edge(vector<list<int>> &adj,int src, int dest) 
{ 
    adj[src].push_back(dest); 
    adj[dest].push_back(src); 
} 

int extractvertexRange(string str) 
{ 
    stringstream ss;    
  
    /* Storing the whole string into string stream */
    ss << str; 
    /* Running loop till the end of the stream */
    string temp; 
    int foundVertexSize; 
    while (!ss.eof()) { 

        /* extracting word by word from stream */
        ss >> temp; 
  
        /* To check if a positive integer is captured */
        if ((stringstream(temp) >> foundVertexSize) && (foundVertexSize >= 0))
        {
            //cout << foundVertexSize; /* DEBUG_POINT:: Print the captured number*/
            break;
        }
        /* To save from space at the end of string */
        temp = ""; 
    } 
    return (foundVertexSize);
} 

void extractvertexSrcDst(string str, int &sourceVertex, int &destVertex) 
{ 
    stringstream ss;    
  
    /* Storing the whole string into string stream */
    ss << str; 
    /* Running loop till the end of the stream */
    string temp; 
    int flag=0; 
    while (!ss.eof()) { 

        /* extracting word by word from stream */
        ss >> temp; 
  
        /* To check if a positive integer is captured */
        if ((flag==0) && (stringstream(temp) >> sourceVertex) && (sourceVertex >=0))
        {
            flag+=1;
            //cout<<sourceVertex<<endl; //DEBUG_POINT
           continue;
        }
        else if((flag!=0) && (stringstream(temp) >> destVertex) && (destVertex >=0)){
            //cout<<destVertex<<endl; //DEBUG_POINT
            break;

        }
        else{
            continue;
        }
        /* To save from space at the end of string */
        temp = ""; 

    } 
} 


vector<string> split (string edge_string, string delimiter) {

    //Removing the curly braces
    string start_delim="{";
    string stop_delim="}";
    unsigned first=edge_string.find(start_delim);
    unsigned last=edge_string.find(stop_delim);
    edge_string=edge_string.substr(first+1,last-3);

    //Split string based on a string delimiter
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;

    while ((pos_end = edge_string.find (delimiter, pos_start)) != string::npos) {
        token = edge_string.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (edge_string.substr (pos_start));
    return res;
}

bool BFS_Algo(vector<list<int>> adj, int src, int dest, int v, 
                            int pred[], int dist[]) 
{ 
    // a queue to maintain queue of vertices used in BFS
    list<int> queue; 
  
    // boolean array which stores the information of vertices that are used
    bool visited[v]; 
  
    // initially all vertices are unvisited 
    // so v[i] for all i is false 
    // and as no path is yet constructed 
    // dist[i] for all i set to infinity 
    for (int i = 0; i < v; i++) { 
        visited[i] = false; 
        dist[i] = INT8_MAX; 
        pred[i] = -1; 
    } 
  
    // now source is first to be visited and 
    // distance from source to itself should be 0 
    visited[src] = true; 
    dist[src] = 0; 
    queue.push_back(src); 
  
    // standard BFS algorithm 
    while (!queue.empty()) { 
    	if( src==dest){
    		return true;
    	}
        int u = queue.front(); 
        queue.pop_front();
        list<int>::iterator it; 
        for (it = adj[u].begin(); it != adj[u].end(); ++it) { 
            if (visited[*it] == false) { 
                visited[*it] = true; 
                dist[*it] = dist[u] + 1; 
                pred[*it]= u; 
                queue.push_back(*it); 
  
                // We stop BFS when we find 
                // destination. 
                if (*it == dest) 
                   return true; 
            } 
        } 
    } 
  
    return false; 
} 

// utility function to print the shortest distance  
// between source vertex and destination vertex 
void printShortestDistance(vector<list<int>> adj, int s,  
                                    int dest, int v) 
{ 
    // predecessor[i] array stores predecessor of 
    // i and distance array stores distance of i 
    // from s 
    int pred[v], dist[v]; 

    if (BFS_Algo(adj, s, dest, v, pred, dist) == false) 
    { 
        cout << "Given source and destination"
             << " are not connected\n"; 
        return;   
    } 
  	if (s == dest){
  		cout<<s<<"-"<<dest<<"\n";
  		return;
  	}
  	else{
	    // vector path stores the shortest path 
	    vector<int> path; 
	    int crawl = dest; 
	    path.push_back(crawl); 
	    while (pred[crawl] != -1) { 
	        path.push_back(pred[crawl]); 
	        crawl = pred[crawl]; 
	    } 
	  
	    // distance from source is in distance array 
	    // cout << "Shortest path length is : "
	    //     << dist[dest]<<"\n"; 
	    // cout<<path.size()<<"\n";
	    // printing path from source to destination 
	    for (int i = path.size() - 1; i >= 0; i--){ 
	        cout << path[i]; 
	        if(i>=1){
	            cout<<"-";
	        }
	    }
	    cout<<"\n";
	    return;
	}
} 


int main(){

    int vertexRange=0; // Vertex range value decalration
    vector<list<int>> adj;
    while(!cin.eof()){

        // read a line of input until EOL and store in a string
        string line;

        getline(cin,line);

        if( line[0] == 'V'){
        	adj.clear(); //The adjacentcyList is cleared once new Vertex range is provided
            //cout<< "inside V\n"; // DEBUG_POINT 
            vertexRange=extractvertexRange(line);
                        //cout <<"The graph vertex range is: " <<vertexRange<< "\n"; //DEBUG_POINT:: print the vertex caputured.
            continue;
        }

        else if( line[0] == 'E'){ 

            adj.resize(vertexRange);
            if (vertexRange !=0){

                int flag=0; //flag to check if edge values intact
                vector<string> v=split(line,">"); //introduce first delimiter
                string firstFilter;
                for (auto const& s : v) { firstFilter += s; } //convert v into string
                //cout<<"The first filter: "<<result<<"\n"; //DEBUG_POINT

                v=split(firstFilter,"<"); //introduce second delimiter
                string secondFilter;
                for (auto const& s : v) { secondFilter += s; } //convert v into string
                //cout<<"The second filter: "<<result1<<"\n"; //DEBUG_POINT

                v=split(secondFilter,","); //introduce third delimiter

                //convert vector<string> to vector<int>
                vector<int> edgeVertexIndex ;
                for (auto &s : v) {
                    stringstream parser(s);
                    int x = 0;

                    parser >> x;
                    // Check if the vertex values are valid and if not raise the flag
                    if (x>=vertexRange){ 
                        vertexRange=0;
                        ++flag;
                        break;
                    }
                edgeVertexIndex.push_back(x);
                }

                //if the edge vertices are valid
                if (flag !=1){

                    for(int i=0;i<edgeVertexIndex.size();i=i+2){
                        add_edge(adj,edgeVertexIndex[i],edgeVertexIndex[i+1]);
                    }
                /* Following comments are for Debug Purpose:*/
                 //    for(std::size_t i = 0; i < adj.size(); i++)
                 // {
                 //     std::cout << i << ": ";
                 //     for(auto &j : adj[i])//range based for loop to iterate through adjList[i] and use the reference j to refer to each element
                 //     {
                 //         std::cout << j << ' ';
                 //     }
                 //         std::cout << '\n';
                 // }
                //for (auto i : edgeVertexIndex) cout << i << endl; //DEBUG_POINT
                //cout<<edgeVertexIndex[3]<<endl;
                //cout<< "inside E\n"; //DEBUG_POINT
                continue;
                }

                else{

                    cerr<<"Error:One or more vertex values are invalid\n";
                    continue;
                }
            }
            else{

                cerr<<"Error:No vertex range available!\n";
                continue;
            }
        }

        else if( line[0] == 's'){ //add a check for edgeVertexIndex

        	if(vertexRange!=0 && !adj.empty()){
	            int source=-1;
	            int dest=-1;
	            extractvertexSrcDst(line,source,dest);
	            cout<<source<<", "<<dest<<endl; //DEBUG_POINT
	            if((source>=0 && dest>=0)&&(source < vertexRange && dest < vertexRange))
	            {
	                printShortestDistance(adj, source, dest, vertexRange); 
	                continue;
	            }
	            else{
	            	vertexRange=0;
	            	adj.clear();
	                cout<<"Error:Invalid Vertex\n";
	            }
	            //cout<< "inside s\n"; //DEBUG_POINT
	        }
	        else{
	        	if(vertexRange==0){
	        		vertexRange=0;
	            	adj.clear();
	        		cerr<<"Error:No vertex range available!\n";
	        	}
	        	else{
	        		vertexRange=0;
	            	adj.clear();
	        		cerr<<"Error:No Edges are available!\n";
	        	}
	        }
        }

        else{

            continue;
        }
    }
    return 0;

}