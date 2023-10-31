// Compile with c++ a2ece650.cpp -std=c++11 -o a2ece650
#include <vector> 
#include <list> 
#include <string> 
#include <sstream> 
#include <iostream>
#include <climits>
#include <unordered_set>
#include <queue>
#include <set>
#include <algorithm>

using namespace std; 

const int N=10001; 

class graph { 
    int V;   //Vertices representing total number of vertices in the graph
    vector<vector<int>>adj; //Create a Adjacency vector
    int source, destination;  //Variable to store source and destination

//Defining access modifiers and getters setters
public: 
    graph(int v) : V(v) {}; 
    //Function that adds an undirected edge between two vertices in a graph 
    //represented using an adjacency vector data structure
    void add_edge(int V, int source, int destination) { 
        adj[source].push_back(destination); 
        adj[destination].push_back(source); 
    } 

    //Function that changes the number of vertices in a graph 
    //represented using an adjacency list data structure
    void changeVertexRange(int v) { 
        V = v; 
        adj.resize(v); 
    } 

    //Function to fetch the vertex values
    int vertexRange() { 
        return V; 
    } 

    //Function to create a Adjacency Vector containing integers of vectors
    vector<vector<int>> AdjacentVector() { 
        return adj; 
    } 

    //Clear the Adjacency Vector
    void emptyAdjacentVector() { 
        adj.clear(); 
    } 

    //Get the source vertex value
    int getSourceVertex() { 
        return source; 
    } 

    //Get the destination vertex value
    int getDestVertex() { 
        return destination; 
    } 

    //Set the source value
    void setSourceVertex(int s) { 
        source = s; 
    } 

    //Set the Destination Value
    void setDestVertex(int d) { 
        destination = d; 
    } 
}; 

//Function for splitting en Edge line 
vector<string> split(const string &input, const string &delimiter) { 
    vector<string> final; 
    stringstream ss(input);
    string token;

    while (getline(ss, token, delimiter[0])){
        final.push_back(token);
    }  
    return final; 
    
} 

//Function for extracting source and destination from the line
void extractvertexSrcDst(string &line, int &src, int &dst) { 
    line.erase(0, 2); 
    stringstream s(line); 
    s >> src >> dst; 
} 

//take a reference to line and remove the first 2 index i.e. V and <space>
//then extract the integer and assign it to s. assign s to v.
int extractvertexRange(string &line) { 
    line.erase(0, 2); 
    int v = 0; 
    stringstream s(line); 
    s >> v; 
    return v; 
} 

//Function to implement BFS Algorithm and print shortest distance
void printShortestDistance(vector<vector<int>>& adjList, int src, int dest) {
    int V = adjList.size();

    vector<int> dist(V, INT_MAX); //Maximum distance set to Infinity
    vector<int> pred(V, -1);

    queue<int> q;
    q.push(src);
    dist[src] = 0;

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        for (int v : adjList[u]) {
            if (dist[v] == INT_MAX) {
                dist[v] = dist[u] + 1;
                pred[v] = u;
                q.push(v);
            }
        }
    }

    if (dist[dest] == INT_MAX) {
        cout << "Error: Given nodes are not connected!";
        return;
    }

    vector<int> path;
    int traverse = dest;
    while (traverse != -1) {
        path.push_back(traverse);
        traverse = pred[traverse];
    }

    for (int i = path.size() - 1; i >= 0; i--) {
        if (i < path.size() - 1) {
            cout << "-";
        }
        cout << path[i];
    }
}

int main(){

    graph g(0); // Initialize the graph with V values 0
    vector<vector<int>>adjacentVector=g.AdjacentVector(); //prepare a adjacency list
    // bool v_seen  = false;
    while(!cin.eof()){
        
        string line;

        getline(cin,line);
        // cout << "Line received" << line << endl;
        //cout << line << endl;
        //check the condition for Vertex
        if( line[0] == 'V' || line[0] == 'v'){
            cout << line << endl;
            g.emptyAdjacentVector();  // empty any previous values that is assigned to V
            g.changeVertexRange(extractvertexRange(line)); 
            //cout <<"The graph vertex range is: " <<g.getVertexRange()<< "\n"; 
            // v_seen = true; //Flag to check if E and V are given together
            continue;
        }
        //check the condition for Edges
        else if (line[0] == 'E' || line[0] == 'e') {
            cout << line << endl;
            // if (!v_seen) { // Check if V line has been seen
            // cerr << "Error: Missing V line!" << endl;
            // continue;
            //  }
            adjacentVector.resize(g.vertexRange());
            if (g.vertexRange() != 0) {
                int flag = 0;
                vector<string> v = split(line, ">");
                string split1;
                for (auto const& s : v) {
                    split1 += s;
                }

                v = split(split1, "<");
                string split2;
                for (auto const& s : v) {
                    split2 += s;
                }

                v = split(split2, ",");

                // Remove the "E" from the first string in the vector
                v[0].erase(0, 2);

                // Remove the "{" from the first string in the vector
                v[0].erase(std::remove(v[0].begin(), v[0].end(), '{'), v[0].end());

                // Remove the "}" from the last string in the vector
                v.back().erase(std::remove(v.back().begin(), v.back().end(), '}'), v.back().end());

                // for (int i = 0 ; i<v.size(); i+=1){
                //     cout << "vertex " << i << ":" << v[i] << endl;
                // }
                vector<int> edgeVertexIndex;
                try {
                    for (auto& s : v) {
                    int x = 0;
                    if (isdigit(s[0])) { // Check if the first character is a digit
                        x = stoi(s);
                        if (x >= g.vertexRange()) {
                            g.changeVertexRange(0);
                            ++flag;
                            break;
                        }
                        //cout << x << endl;
                        edgeVertexIndex.push_back(x);
                    }
                   }
                   // for (unsigned int i = 0; i < edgeVertexIndex.size(); i = i + 2){
                   //     cout << edgeVertexIndex[i] << edgeVertexIndex[i+1] << endl;
                   // }
                    if (flag != 1) {
                        for (unsigned int i = 0; i < edgeVertexIndex.size(); i = i + 2) {
                            g.add_edge(g.vertexRange(), edgeVertexIndex[i], edgeVertexIndex[i + 1]);
                        }
                        adjacentVector = g.AdjacentVector();
                    }
                    else {
                        throw invalid_argument("Error: Invalid Vertex Values!!!");
                    }
                }
                catch (const exception& e) {
                    g.changeVertexRange(0);
                    adjacentVector.clear();
                    cerr << "Error: " << e.what() << endl;
                }
            }
            else {
                cerr << "Error: Vertex range not available!!!" << endl;
            }
            if ((line.empty() || line[0] != 'E' && line[0] != 'e')) {
                 cerr << "Error: Missing E line!" << endl;
                }
        }
        //check the input condition for Shortest Distance
        else if( line[0] == 's' || line[0] == 'S'){ 
            // std::cout << "S" << line << std::endl;
                try {
                if (g.vertexRange() == 0) {
                    throw runtime_error("Error: No vertex range available!");
                }
                if (adjacentVector.empty()) {
                    throw runtime_error("Error: No edges available!");
                }
                int source = g.getSourceVertex();
                int dest = g.getDestVertex();
                extractvertexSrcDst(line, source, dest);
                if (source < 0 || source >= g.vertexRange()) {
                    throw runtime_error("Error: Invalid source vertex index!");
                }
                if (dest < 0 || dest >= g.vertexRange()) {
                    throw runtime_error("Error: Invalid destination vertex index!");
                }
                // for (int i = 0; i < adjacentVector.size(); i++) {
                // cout << i << ": ";
                // for (int j = 0; j < adjacentVector[i].size(); j++) {
                //     cout << adjacentVector[i][j] << " ";
                // }
                // cout << endl;
                // }
                printShortestDistance(adjacentVector, source, dest); 
                // printShortestDistance(adjList, source, dest, g.getVertexRange());
                cout << "\n";
                } 
                catch (const exception& e) {
                g.changeVertexRange(0);
                adjacentVector.clear();
                cerr << e.what() << endl;
                 }
    }
    }
    return 0;
}