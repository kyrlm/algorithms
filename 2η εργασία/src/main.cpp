#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/graph_utility.hpp>
#include <iostream>
#include <vector>
#include <LEDA/graph/graph.h>
#include <LEDA/graph/edge_array.h>
#include <LEDA/graph/node_array.h>
#include <LEDA/graph/graph_misc.h>
#include <LEDA/core/random_source.h>
#include "LEDA/system/timer.h"
#include <boost/heap/fibonacci_heap.hpp>


using namespace std;
using namespace boost;
using namespace heap;
using leda::node_array;
using leda::edge_array;


//δομη αποθήκευσης των πληροφοριών των κόμβων
struct NodeInfo{
    int distance;
    int predecessor;
};

//δομή αποθήκευσης των πληροφοριών των ακμών
struct EdgeInfo{
    int weight;
};


typedef leda::graph LGraph;


typedef boost::adjacency_list<boost::vecS,boost::vecS,boost::bidirectionalS,NodeInfo,EdgeInfo> BGraph;
typedef boost::graph_traits<BGraph>::vertex_descriptor Vertex;
typedef boost::graph_traits<BGraph>::edge_descriptor arc;
typedef boost::graph_traits<BGraph>::vertex_iterator vertex_it;
typedef boost::graph_traits<BGraph>::edge_iterator arc_it;
typedef boost::graph_traits<BGraph>::out_edge_iterator out_arc_it;
typedef boost::graph_traits<BGraph>::in_edge_iterator in_arc_it;

typedef boost::property_map<BGraph, int EdgeInfo::*>::type WeightMap;
typedef boost::property_map<BGraph, int NodeInfo::*>::type DistanceMap;
typedef boost::property_map<BGraph, int NodeInfo::*>::type PredecessorMap;



vector<int> weightVec; //vector για την αποθήκευση των βαρών των ακμών

//naming
DistanceMap b_dist;
WeightMap b_weight;
PredecessorMap b_pred;

arc_it akmh, akmh2; //Iterators ακμών.
vertex_it koryfh, koryfh2; //Iterators κορυφών.
out_arc_it e3erxomenh,e3erxomenh2; //Iterators εξερχόμενων ακμών.
in_arc_it eiserxomenh, eiserxomenh2; //Iterators εισερχόμενων ακμών.



struct HeapCompare { //Σύγκριση των κορυφών που θα εισάγονται στο heap
    bool operator()(Vertex a, Vertex b) const {
        return b_dist[a] > b_dist[b];
    };
};

//συνάρτηση για αντιγραφή γραφήματος
void copy_graph(const LGraph &LG, BGraph &BG, edge_array<int> &l_weight, node_array<NodeInfo>&nodeInfo){


    leda::node_array<Vertex> copy_in_BG(LG);
    arc a;
    leda::edge e;
    leda::node v;


    forall_nodes(v,LG){
        copy_in_BG[v]=add_vertex(BG);
        b_dist[copy_in_BG[v]]=nodeInfo[v].distance;
    }
    bool isAdded;


    forall_edges(e,LG){
        tie(a,isAdded)=add_edge(copy_in_BG[source(e)], copy_in_BG[target(e)], BG);
        b_weight[a]=l_weight[e];
    }
}

//συνάρτηση για random δημιουργία γραφήματος και μετατροπή του σε γράφημα leda
void randomGraph(BGraph &BG, int a, int b){
  
   //δημιουργία γραφήματος boost
    LGraph LG;
    edge_array<int> l_weight(LG);

    WeightMap b_weight = get(&EdgeInfo::weight, BG);
    DistanceMap b_dist = get(&NodeInfo::distance, BG);
    PredecessorMap b_pred =get(&NodeInfo::predecessor, BG);

    random_graph(LG, a, b, true, false, true); //Δημιουργία τυχαίου γραφήματος.
    Make_Connected(LG); //σύνδεση των κορυφών με ακμές

    leda::node_array<NodeInfo> nodeInfoo(LG);
    l_weight.init(LG);

    //αρχικοποίηση των κόστων των ακμών
    leda::random_source S;
    leda::edge e;
    forall_edges(e,LG){
        l_weight[e]=S()%100;
    }

    //αρχικοποίηση της απόστασης των κορυφών σε άπειρο (ενα μεγάλο νούμερο)
    leda::node v;
    forall_nodes(v,LG){
        nodeInfoo[v].distance= 1000000;
    }


    //αντιγραφή γραφήματος απο Leda σε boost με την συνάρτηση που δημιουργήθηκε παραπάνω
    copy_graph(LG, BG, l_weight, nodeInfoo);

    //αποθηκευση των βαρων
     Vertex p;
     for(tie(eiserxomenh, eiserxomenh2) = in_edges(p,BG); eiserxomenh != eiserxomenh2; eiserxomenh++){
        weightVec.push_back(b_weight[*eiserxomenh]);
    }

}

//συνάρτηση για τον αλγόριθμο Dijkstra-SP
void DijkstraSP(BGraph &BG, Vertex s, Vertex t){

    if(t!=nil){ //αν υπάρχει η κορυφή t , τότε μπορούμε να συνεχίσουμε κανονικά

    int koryfhT=0;

    //δημιουργία heap για την αποθήκευση των κορυφών και την διαχείρηση τους
    boost::heap::fibonacci_heap<Vertex, boost::heap::compare<HeapCompare>> heap; 
    boost::heap::fibonacci_heap<Vertex, boost::heap::compare<HeapCompare>>::handle_type handles[num_vertices(BG)];



    b_dist[s]=0; //Αρχικοποίηση με 0 την απόσταση από την κορυφή που ξεκινάμε
    handles[s]= heap.push(s); //εισαγωγή της αρχικής κορυφής στην ουρά

    //όσο η ουρά δεν είναι άδεια και η κορυφή t δεν έχει βρεθεί ακόμα
    while(!heap.empty() && koryfhT==0){
        Vertex vert  =heap.top(); //βρίσκουμε την κορυφή με την μικρότερη απόστασση
        heap.pop(); //και την αφαιρούμε από την ουρά
        int i=0;

        if(vert == t){ //αν η κοντινότερη κορυφή είναι η t 
            koryfhT=1; //ενημερώνουμε την μετασβλητή
        }
        else{
            for (tie(e3erxomenh, e3erxomenh2) = out_edges(vert, BG);e3erxomenh != e3erxomenh2; e3erxomenh++, i++) { //Για κάθε εξερχόμενη ακμή

            int distanceTarget = b_dist[target(*e3erxomenh, BG)];
            int distanceCurrent = b_dist[vert];
            int edgeWeight = b_weight[*e3erxomenh];

            if (distanceTarget > distanceCurrent + edgeWeight) { //ελέγχουμε αν ισχύει η τριγωνική ανισότητα
                if (distanceTarget == 1000000) { //Αν βλέπουμε την κορυφή για πρώτη φορά.
                    handles[target(*e3erxomenh, BG)] = heap.push(target(*e3erxomenh, BG)); //την εισάγουμε στο heap
                }
                b_dist[target(*e3erxomenh, BG)] = b_dist[vert] + b_weight[*e3erxomenh]; 
                heap.update(handles[target(*e3erxomenh, BG)]); 
                b_pred[target(*e3erxomenh, BG)] = vert; 
            }
        }
    }
 }

}
    //αλλιώς αν δεν υπάρχει κορυφή t, τότε εμφανίζεται το κατάλληλο μήνυμα
    else{
        cout<<"No node t"<<"\n";
    }
}


//συνάρτηση για την προεπεξεργασία του αλγορίθμου A*
void ht_process(Vertex s, Vertex t, BGraph &BG){

        //L1
        leda::random_source S;
        int n = num_vertices(BG); //τυχαία επιλογή της L1
        for (tie(koryfh, koryfh2) = vertices(BG); koryfh != koryfh2; koryfh++) {
            b_dist[*koryfh] = 1000000;
        }
        int L1 = S() % n;
        while (L1 == s) {
            L1 = S() % n;
        }

        DijkstraSP(BG, L1, nil); //εύρεση αποστάσεων από L1
        std::vector<int> L1_Distances;


        for (tie(koryfh, koryfh2) = vertices(BG); koryfh != koryfh2; koryfh++) { //αποθήκευση των αποστάσεων
            L1_Distances.push_back(b_dist[*koryfh]);
        }


        int distance = 0; //εύρεση μιας πιο απομακρυσμένης κορυφής από την L1
        int L2;
        for (int i = 0; i < n; i++) {
            if (b_dist[i] > distance) {
            distance = b_dist[i];
            L2 = i;
            }
        }

         //Αρχικοποίηση αποστάσεων.
        for (tie(koryfh, koryfh2) = vertices(BG); koryfh != koryfh2; koryfh++) {
            b_dist[*koryfh] = 1000000;
        }


        DijkstraSP(BG, L1, nil); //εύρεση των αποστάσεων προς την L1
        std::vector<int> L1_1_Distances;

        for (tie(koryfh, koryfh2) = vertices(BG); koryfh != koryfh2; koryfh++) { //αποθήκευση των αποστάσεων
            L1_1_Distances.push_back(b_dist[*koryfh]);
        }

        //L2
        //Αρχικοποίηση αποστάσεων.
        for (tie(koryfh, koryfh2) = vertices(BG); koryfh != koryfh2; koryfh++) {
            b_dist[*koryfh] = 1000000;
 }


        DijkstraSP(BG, L2, nil); //εύρεση των αποστάσεων από την L2.
        std::vector<int> L2_Distances;

        for (tie(koryfh, koryfh2) = vertices(BG); koryfh != koryfh2; koryfh++) { //αποθήκευση των αποστάσεων
            L2_Distances.push_back(b_dist[*koryfh]);
        }

        //Αρχικοποίηση αποστάσεων.
        for (tie(koryfh, koryfh2) = vertices(BG); koryfh != koryfh2; koryfh++) {
            b_dist[*koryfh] = 1000000;
        }



        DijkstraSP(BG, L2, nil); //εύρεση των αποστάσεων προς την L2.
        std::vector<int> L2_1_Distances;

        for (tie(koryfh, koryfh2) = vertices(BG); koryfh != koryfh2; koryfh++) { //αποθήκευση των αποστάσεων
            L2_1_Distances.push_back(b_dist[*koryfh]);
        }
        std::vector<int> H;
        for (tie(koryfh, koryfh2) = vertices(BG); koryfh != koryfh2; koryfh++) { //υπολογισμός της ht
            int max = std::max(L1_Distances[t] - L1_1_Distances[*koryfh], L2_Distances[*koryfh] - L2_Distances[t]);

            H.push_back(max);
        }

        int i = 0;
        for (tie(eiserxomenh, eiserxomenh2) = in_edges(s,BG); eiserxomenh != eiserxomenh2; eiserxomenh++, i++) { 
            b_weight[*eiserxomenh] = weightVec[i] + H[target(*eiserxomenh, BG)] - H[source(*eiserxomenh, BG)];

        }

        //Αρχικοποίηση αποστάσεων.
        for (tie(koryfh, koryfh2) = vertices(BG); koryfh != koryfh2; koryfh++) {
           b_dist[*koryfh] = 1000000;
        }




}

void aStar(Vertex s, Vertex t, BGraph &BG){

        int i=0;
        //αρχικοποίηση αποστάσεων
        for (tie(koryfh, koryfh2) = vertices(BG); koryfh != koryfh2; koryfh++) {
            b_dist[*koryfh] = 1000000; //εισαγωγη μεγάλου αριθμού που απεικονίζει άπειρο
        }

        //αρχικοποίηση βαρών
        for (tie(eiserxomenh, eiserxomenh2) = in_edges(s,BG); eiserxomenh != eiserxomenh2; eiserxomenh++, i++) {
            b_weight[*eiserxomenh] = weightVec[i];
        }


        ht_process(s, t, BG); //κλήση συνάρτησης προεπεξεργασίας

        //αρχικοποίηση μετρητή χρόνου
        leda::timer kr;

        kr.start();
        DijkstraSP(BG,s, t); //εκτέλεση dijkstra
        kr.stop();

        float aStarTime = kr.elapsed_time();
        cout<<"Astar Time: "<< aStarTime <<"\n";

}


int main(){

        //δημιουργία γραφήματος
        BGraph BG;

        int a = 10000; //κορυφές
        int b = 20000; //ακμές

        randomGraph(BG,a,b); //κλήση συνάρτησης για την δημιουργία τυχαίου γραφήματος
        Vertex source;
        Vertex target;
        //αρχικοποίηση μετρητή χρόνου
        leda::timer kr;
        kr.start();
        DijkstraSP(BG, source, target); //εκτέλεση του Dĳkstra-SP αλγορίθμου
        kr.stop();

        float dijkstraTime =kr.elapsed_time();
        cout<< "Dijkstra Time: " << dijkstraTime << "\n";

        //κλήση της συνάρτησης aStar
        aStar(a,b,BG);


        return 0;
}

