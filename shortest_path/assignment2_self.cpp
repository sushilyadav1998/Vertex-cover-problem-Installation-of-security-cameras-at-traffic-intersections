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
    int V; 
    vector<vector<int>>adj; 
    int source, destination; 

public: 
    graph(int v) : V(v)  {adj.resize(V) ;}; 
    void add_edge(int source, int destination) { 
        adj[source].push_back(destination); 
        adj[destination].push_back(source); 
    } 

    void changeVertexRange(int v) { 
        // V = v; 
        this->V = v; 
        adj.clear();
        adj.resize(v); 
    } 

    int vertexRange() { 
        return V; 
    } 

    vector<vector<int>> AdjacentList() { 
        return adj; 
    } 

    void emptyAdjacentList() { 
        adj.clear(); 
    } 

    int getSourceVertex() { 
        return source; 
    } 

    int getDestVertex() { 
        return destination; 
    } 

    void setSourceVertex(int s) { 
        source = s; 
    } 

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

void printShortestDistance(vector<vector<int>>& adjList, int src, int dest) {
    int V = adjList.size();

    vector<int> dist(V, INT_MAX);
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
    vector<vector<int>>adjacentList=g.AdjacentList(); //prepare a adjacency list
    bool v_seen  = false;
    while(!cin.eof()){

        string line;

        getline(cin,line);
        //cout << "Line received" << line[0] << endl;
        //check the condition for Vertex
        if( line[0] == 'V' || line[0] == 'v'){
            g.emptyAdjacentList();  // empty any previous values that is assigned to V
            g.changeVertexRange(extractvertexRange(line)); 
            //cout <<"The graph vertex range is: " <<g.getVertexRange()<< "\n"; 
            v_seen = true;
            continue;
        }
        //check the condition for Edges
        else if (line[0] == 'E' || line[0] == 'e') {
            if (!v_seen) { // Check if V line has been seen
            cerr << "Error: Missing V line!" << endl;
            break;
             }
            adjacentList.resize(g.vertexRange());
            if (g.vertexRange() != 0) {
                int flag = 0;
                vector<string> v = split(line, ">");
                string split1;
                //The resulting value of split1 will be the string "1,2,2,4,5,4,5,6".
                for (auto const& s : v) {
                    split1 += s;
                }

                v = split(split1, "<");
                string split2;
                for (auto const& s : v) {
                    split2 += s;
                }

                v = split(split2, ",");
                // for (int i = 0 ; i<v.size(); i+=1){
                //     cout << "vertex " << i << ":" << v[i] << endl;
                // }
                
                // Remove the "E" from the first string in the vector
                v[0].erase(0, 2);

                // Remove the "{" from the first string in the vector
                v[0].erase(std::remove(v[0].begin(), v[0].end(), '{'), v[0].end());

                // Remove the "}" from the last string in the vector
                v.back().erase(std::remove(v.back().begin(), v.back().end(), '}'), v.back().end());

                // Print the resulting vector
                // for (const auto& s : v) {
                //     std::cout << s << " ";
                // }

                // for (int i = 0 ; i<v.size(); i+=1){
                //     cout << "vertex " << i << ":" << v[i] << endl;
                // }
                vector<int> edgeVertexIndex;
                try {
                    int x;

                    for (auto& a : v) {
                    int x = 0;
                    if (isdigit(a[0])) { // Check if the first character is a digit
                        x = stoi(a);
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
                            g.add_edge(edgeVertexIndex[i], edgeVertexIndex[i + 1]);
                        }
                        adjacentList = g.AdjacentList();
                    }
                    else {
                        throw invalid_argument("Invalid Vertex Values!!!");
                    }
                }
                catch (const exception& e) {
                    g.changeVertexRange(0);
                    adjacentList.clear();
                    cerr << "Error: " << e.what() << endl;
                }
            }
            else {
                cerr << "Error: Vertex range not available!!!" << endl;
            }
            if (v_seen && (line.empty() || line[0] != 'E' && line[0] != 'e')) {
                 cerr << "Error: Missing E line!" << endl;
                }
        }
        //check the input condition for Shortest Distance
        else if( line[0] == 's' || line[0] == 'S'){ 
                try {
                if (g.vertexRange() == 0) {
                    throw runtime_error("Error: No vertex range available!");
                }
                if (adjacentList.empty()) {
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
                // for (int i = 0; i < adjacentList.size(); i++) {
                // cout << i << ": ";
                // for (int j = 0; j < adjacentList[i].size(); j++) {
                //     cout << adjacentList[i][j] << " ";
                // }
                // cout << endl;
                // }
                printShortestDistance(adjacentList, source, dest); 
                // printShortestDistance(adjList, source, dest, g.getVertexRange());
                cout << "\n" << endl;
                } 
                catch (const exception& e) {
                g.changeVertexRange(0);
                adjacentList.clear();
                cerr << e.what() << endl;
                 }
    }
    }
    return 0;
}