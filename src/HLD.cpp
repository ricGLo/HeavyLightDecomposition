#include <bits/stdc++.h>
using namespace std;
#define MAX 100000

//Segment tree
long long ST[4*MAX]; 

//Adjacent List of the tree
vector<vector<int>> AdjList; 

//Stores the value (weight) of the node
vector<long long> values; 

//Stores the parent Id of each node
vector<int> parent; 

//Stores the heaviest child of the node. This is, the child with mode descendants.
vector<int> heaviest; 

// Stores the depth of each node in the tree  
vector<int> depth; 

// It stores nodes Id's separated by paths. For instance, if the first path consists on k nodes, the first k places in the vector contain those k nodes Id's in the order corresponding to the path beginning with the root of the path to the leaf node of the path. Then the vector stores another path in the same way and so on.
vector<int> HLD; 

//The j-th entry of this vector stores the index where node j is in the HLD vector.
vector<int> map_to_HLD; 

// Stores the root of the path to which the node belongs. Recall that each of the nodes only belongs to one path.
vector<int> roots; 

/////////////////////////////////////////  SEGMENT TREE IMPLEMENTATION  //////////////////////////////////////
/* 
[i_l, i_r] : Currently index interval
q_idx : index to update
val : new value of q_idx  */

void updateUtil(int index, int i_l, int i_r, int q_idx, long long val){
    if (i_l == i_r){
        ST[index] = values[HLD[i_l]] = val;
        return;
    }

    int m = (i_l + i_r)/2;
    if(q_idx <= m)
        updateUtil(2*index + 1, i_l, m, q_idx, val);
    else
        updateUtil(2*index + 2, m+1, i_r, q_idx, val);

    ST[index] = ST[index*2+1] + ST[index*2+2];
}

// Function to update a value in the segment tree
void update(int n, int q_idx, int val){
   updateUtil(0, 0, n-1, q_idx, (long long) val); // cast val to long long
}

long long queryST_util(int index, int i_l, int i_r, int q_l, int q_r){
    // out of range
    if (i_l > i_r || i_l>q_r || i_r<q_l)
        return 0;

    // Current segment is fully in range
    if (q_l <= i_l && i_r <= q_r)
        return ST[index];

    int m = (i_l + i_r)/2;
    return queryST_util(2*index + 1, i_l, m, q_l, q_r) + queryST_util(2*index + 2, m+1, i_r, q_l, q_r);

}

long long queryST(int n, int q_l, int q_r){
    return queryST_util(0, 0, n-1, q_l, q_r);
}

// A recursive function that constructs Segment Tree for
void constructSTUtil(int l, int r, int index){

    //Leaf node
    if (r == l){
        ST[index] = values[HLD[r]];
        return;
    }
    // If there are more than one element, then recur for left and right subtrees
    int mid = (l + r)/2;
    constructSTUtil(l, mid, index*2+1);
    constructSTUtil(mid+1, r, index*2+2);

    ST[index] = ST[index*2 + 1] + ST[index*2 + 2];
}
  
// Function to construct segment tree from given array.
void constructST(int n){
    constructSTUtil(0, n-1, 0);
}
////////////////////////////////   END OF SEGMENT TREE IMPLEMENTATION ///////////////////////////////////////////////////////


// Depth First Search
int DFS(int v, int dad, int d){
    int size = 1; // size of the subtree with root v
    int max_size = -INT_MAX; //size of bigger subtree whose root is some child of v, it is initialized at -infty
    depth[v] = d; 
    parent[v] = dad; //assign parent of v
    for(int i = 0; i < AdjList[v].size(); i++){
        int u = AdjList[v][i];
        int subsize = DFS(u, v, d+1);
        if(subsize > max_size){ //update which is the heaviest subtree
            heaviest[v] = u;
            max_size = subsize;
        }
        size += subsize;
    }
    return size;
}

void make_HLD(int N){
    int current_idx = 0; // currently index in the vector HLD
    for(int i = 0; i < N; i++){
        // If i is the root of the entire tree, or if i is not the heaviest subtree among its siblings, then i is the root of a path
        if(parent[i] == -1 || heaviest[parent[i]] != i){
            int j = i; //node in the travel of the path
            while(j != -1){ // travel the path until reaching a leaf node
                HLD[current_idx] = j;
                roots[j] = i;
                map_to_HLD[j] = current_idx;
                current_idx++;
                j = heaviest[j]; //next node in the path is the heaviest child of j
            }       
        }
    }
}

long long sumPath(int a, int b, int n){
    long long sum = 0;
    while(roots[a] != roots[b]){ //While a and b are in distinct paths
        if(depth[roots[a]] > depth[roots[b]]) // b is needed to be deeper than a
            swap(a, b);
        
        sum += queryST(n, map_to_HLD[roots[b]], map_to_HLD[b]);
        b = parent[roots[b]];
    }
    //Now a and b are in the same path

    if(depth[a] > depth[b]) // b is needed to be deeper than a
        swap(a, b); 

    sum += queryST(n, map_to_HLD[a], map_to_HLD[b]); // Add up desire part od the path
    return sum;
}

int main(){
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0); 

    //variables to read the INPUT
    int N, Q, dad, a, b;
    long long num;
    char op;
    cin >> N >> Q;

    // Initialize vectors
    AdjList.assign(N, vector<int>());
    values.assign(N, 0);
    parent.assign(N, -1);
    heaviest.assign(N, -1);
    depth.assign(N, 0);
    HLD.assign(N, 0);
    roots.assign(N, 0);
    map_to_HLD.assign(N, 0);

    // Get values of each node
    for(int i = 0; i < N; i++)
        cin >> values[i];

    // Create adjacency list of the tree
    for(int i = 1; i < N; i++){
        cin >> dad;
        AdjList[dad].push_back(i);
    }

    // Construct Data Structures
    DFS(0, -1, 0); // Travel the tree via DFS
    make_HLD(N);   // Make Heavy Light Decomposition
    constructST(N); // Construct Segment Tree

    //Queries
    for(int i = 0; i < Q; i++){
        cin >> op >> a >> b;
        if(op == 'U') //Update
            update(N, map_to_HLD[a], b);
        else //SumPath
            cout << sumPath(a, b, N) << "\n";
    }
    return 0;
}