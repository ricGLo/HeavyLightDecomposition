#include <bits/stdc++.h>
using namespace std;
#define MAX 100000

long long ST[4*MAX]; //Segment tree
vector<vector<int>> AdjList; //Lista de adyacencia del arbol.
vector<long long> values; //Alamacena el valor (peso) de cada nodo.
vector<int> parent; //Almacena el padre de cada nodo.
vector<int> heaviest; //Almacena el hijo mas pesado de cada nodo, es decir el hijo que tiene mas descendientes.
vector<int> depth; //Almacena la profundidad de cada nodo.  
vector<int> HLD; //Se guardan los nodos separados por caminos. Si el primer camino consta de k nodos, los primeros k nodos del vector contienen dichos k nodos empezando por la raiz del camino hasta el nodo hoja del camino. Luego se guarda otro camino de la mismo manera y asi sucesivamente.
vector<int> map_to_HLD; //En la posicion j del vector se guarda el indice donde se encuentra el nodo j en el vector HLD.
vector<int> roots; //Almacena la raiz del path en el que se encuentra cada nodo.

/////////////////////////////////////////  SEGMENT TREE IMPLEMENTATION  //////////////////////////////////////
/* 
[i_l, i_r] : Es el intervalo del indice actual
q_idx : indice a actualizar
val : valor nuevo de q_idx  */

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

// Function to update a range of values in segment tree
void update(int n, int q_idx, int val){
   updateUtil(0, 0, n-1, q_idx, (long long) val); //Se hace cast de int a long long
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
    int size = 1; //tamano del subarbol con raiz v
    int max_size = -INT_MAX; //tamanio del subarbol mas grande que tiene como raiz algun nodo hijo de v, se inicializa en -infty
    depth[v] = d; //profundidad
    parent[v] = dad; //Asignar padre de v
    for(int i = 0; i < AdjList[v].size(); i++){
        int u = AdjList[v][i];
        int subsize = DFS(u, v, d+1);
        if(subsize > max_size){ //actualizar quien es el subarbol mas pesado
            heaviest[v] = u;
            max_size = subsize;
        }
        size += subsize;
    }
    return size;
}

void make_HLD(int N){
    int current_idx = 0; //indice actual en el vector HLD
    for(int i = 0; i < N; i++){
        //Si i es el nodo raiz o si i no es el subarbol mas grande entre los de sus hermanos, entonces i es raiz de un path
        if(parent[i] == -1 || heaviest[parent[i]] != i){
            int j = i; //nodo en el recorrido del path
            while(j != -1){ //recorrer el path hasta llegar a un nodo hoja
                HLD[current_idx] = j;
                roots[j] = i;
                map_to_HLD[j] = current_idx;
                current_idx++;
                j = heaviest[j]; //el siguiente nodo en el path es el mas pesado de los hijos de j
            }       
        }
    }
}

long long sumPath(int a, int b, int n){
    long long sum = 0;
    while(roots[a] != roots[b]){ //Mientras que los paths de a y b sean distintos
        if(depth[roots[a]] > depth[roots[b]]) // Deseamos que b sea el nodo cuyo path tenga la raiz mas profunda
            swap(a, b);
        
        sum += queryST(n, map_to_HLD[roots[b]], map_to_HLD[b]);
        b = parent[roots[b]];
    }
    //Ahora a y b estan en el mismo path

    if(depth[a] > depth[b])//Deseamos que b sea el nodo con mayor profundidad
        swap(a, b); 

    sum += queryST(n, map_to_HLD[a], map_to_HLD[b]); //Sumar el trozo deseado del camino 
    return sum;
}

int main(){
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0); 

    //Variables para leer INPUT
    int N, Q, dad, a, b;
    long long num;
    char op;
    cin >> N >> Q;

    //Inicializar vectores
    AdjList.assign(N, vector<int>());
    values.assign(N, 0);
    parent.assign(N, -1);
    heaviest.assign(N, -1);
    depth.assign(N, 0);
    HLD.assign(N, 0);
    roots.assign(N, 0);
    map_to_HLD.assign(N, 0);

    //Obtener los valores de cada nodo
    for(int i = 0; i < N; i++)
        cin >> values[i];

    //Crear la lista de adyacencia del arbol
    for(int i = 1; i < N; i++){
        cin >> dad;
        AdjList[dad].push_back(i);
    }

    //Construir las estructuras de datos
    DFS(0, -1, 0); //Recorrido del arbol en profundiad
    make_HLD(N);   //Realizar el Heavy Light Decomposition
    constructST(N); //Construir Segment Tree

    //Peticiones
    for(int i = 0; i < Q; i++){
        cin >> op >> a >> b;
        if(op == 'U') //Update
            update(N, map_to_HLD[a], b);
        else //SumPath
            cout << sumPath(a, b, N) << "\n";
    }
    return 0;
}