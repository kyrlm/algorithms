
//****************** Dijkstra Radix-Heap ********************


#include <LEDA/graph/graph_alg.h>
#include <LEDA/core/_p_queue.h>
#include <LEDA/core/impl/bin_heap.h>
#include <LEDA/core/impl/r_heap.h>
#include <LEDA/core/impl/list_pq.h>
#include "LEDA/graph/graph.h"
#include "LEDA/core/random_source.h"
#include <iostream>
#include "LEDA/system/timer.h"

using namespace std;
using namespace leda;


// Δημιουργία συνάρτησης Dijkstra με την χρήση Radix Heap
template<class r_impl>
void dijkstra(graph& G, node s, edge_array<int>&  cost, node_array<int>&  dist, node_array<edge>& pred, p_queue<int,node,r_impl>& R)
{
  typedef typename p_queue<int,node,r_impl>::item r_item;
  
  node_array<r_item> I(G);
  node v;
  edge e;
  r_item it;

  // Αρχικοποίηση αποστάσεων στο άπειρο και προγόνους στο μηδέν
  forall_nodes(v,G)
  {
    pred[v] = nil;
    dist[v] = MAXINT;
   }

  //Αρχικοποίηση απόστασης πηγής στο μηδέν και εισαγωγή κορυφής πηγής στο heap
  dist[s] = 0;
  I[s] = R.insert(0,s);

  //Όσο το heap δεν είναι άδειο, ψάχνουμε το στοιχείο με την μικρότερη απόσταση
  while (! R.empty())
  {
    it = R.find_min();
    node u = R.inf(it);
    int du = dist[u];

    //Για όλες τις ακμές του γραφήματος βρίσκουμε αν υπάρχει μικρότερη απόσταση και μειώνουμε το key
    // Αν η απόσταση είναι άπειρη του στόχου είναι άπειρη, τότε του θέτουμε την νέα απόσταση που βρίκαμε και το εισάγουμε στο heap
    forall_adj_edges(e,u)
    {
        v = G.target(e);
        int c = du + cost[e];
        if (c < dist[v])
        {

            //Εισαγωγή στο heap
            if (dist[v] == MAXINT)
               I[v] = R.insert(c,v);
            else

               // Μείωση του key
               R.decrease_p(I[v],c);
            dist[v] = c;
            pred[v] = e;
       }
    }

    //Διαγραφή στοιχείου με την μικρότερη απόσταση από το heap
    R.del_item(it);

   }
}



int main(){

    //Δημιουργία γραφήματος
    GRAPH<int,int> G;
    int n = 5000; //κορυφές
    int m = 10000; //ακμές


    //Τυχαίο γράφημα
    random_graph(G,n,m);

    //Δημιουργία πινάκων που θα χρειαστούν για τον Dijkstra της LEDA
    edge_array<int>  cost(G);
    node_array<int>  dist0(G);
    node_array<int>  dist(G);
    node_array<edge> pred(G);

    //Εισαγωγή κόστων στις ακμές (απο 0 μέχρι 100)
    edge e;
    forall_edges(e,G)
      G[e] = cost[e] = rand_int(0,100);




   // Εύρεση μέγιστου κόστους (Segmentation Fault)?????
   /* int max_w = cost[0];

   for (int j=0; j< sizeof(cost); j++){

       if(max_w < cost[j]){
                max_w = cost[j];

       }

    }*/



    // Επιλογή τυχαίας κορυφής πηγής
    node S = G.choose_node();



   //Μέτρηση χρόνου για dijkstra της LEDA
    timer tim2;
    tim2.start();
    for(int i=0;i <10000; i++){
        DIJKSTRA (G,S,cost,dist0,pred);
    }
    tim2.stop();
    cout<<"Dijkstra LEDA: "<<tim2.elapsed_time()/10000 <<" seconds"<<endl;


    // Θεωρώ τυχαίο μέγιστο κόστος 99 , αφού αλλιώς έχω Segmentation Fault
    int max_w =90;
    p_queue<int,node,r_heap> R(max_w);


   //Μέτρηση χρόνου για dijkstra με radix-heap
    timer tim;
    tim.start();
    for(int i=0;i <10000; i++){
        dijkstra(G, S, cost, dist, pred, R);
    }
    tim.stop();
    cout<<"Dijkstra-RHeap: "<<tim.elapsed_time()/10000 <<" seconds"<<endl;


return 0;
}
