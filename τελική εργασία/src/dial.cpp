

//********************** Dijkstra Dial's Algorithm *************************



#include "LEDA/system/timer.h"
#include <bits/stdc++.h>


// Αναπαράσταση του απείρου με τον μεγάλο αριθμό 1000000
#define INF 1000000

using namespace std;
using namespace leda;



// Συνάρτηση για την δημιουργία ακμών με βάρη στο γράφημα
void addEdge(vector <pair<int, int> > adj[], int u, int v, int wt)
{
    //Ένωση κορυφών με βάρη και συσχέτιση με ακμές
    adj[u].push_back(make_pair(v, wt));
    adj[v].push_back(make_pair(u, wt));
}



//Συνάρτηση Dial
void Dial(int src, int W, int V,vector <pair<int, int> > adj[] ){

    //Δημιουργία vector με τις αποστάσεις
    vector<pair<int, list<int>::iterator> > dist(V);

    // Αρχίκοποίηση των αποστάσεων στο άπερο (αναπαράσταση με έναν πολύ μεγάλο αριθμό)
    for (int i = 0; i < V; i++)
        dist[i].first = INF;

    // Δημιουργία των Buckets
    list<int> B[W * V + 1];

    // Εισαγωγή του source στο πρώτο bucket και αρχικοποίηση της απόστασης σε 0
    B[0].push_back(src);
    dist[src].first = 0;


    int idx = 0;
    while (1)
    {

        //Ψάχνουμε για κάποιο bucket το οποίο δεν θα είναι άδειο
        while (B[idx].size() == 0 && idx < W*V)
            idx++;

        // Αν όλα τα buckets είναι άδεια τότε ο αλγόριθμος έχει τελειώσει
        if (idx == W * V)
            break;

        // Εφόσον το bucket που έχουμε βρει δεν είναι άδειο τότε βγάζουμε το πρώτο στοιχείο του
        int u = B[idx].front();
        B[idx].pop_front();

        // Φτιάχνουμε τις αλλαγες που έγιναν και διορθώνουμε τις αποστάσεις μετά την αφαίρεση ένος κόμβου
        for (auto i = adj[u].begin(); i != adj[u].end(); ++i)
        {
            int v = (*i).first;
            int weight = (*i).second;

            int du = dist[u].first;
            int dv = dist[v].first;

            // Αν υπάρχει μικρότερο μονοπάτι απο την κορυφή πηγή στην κορυφή προορισμού
            if (dv > du + weight)
            {
                //Διαγράφουμε την κορυφή με την μεγαλύτερη διαδρομή απο το bucket που βρίσκεται (αν η διαδρομή αυτή δεν είναι άπειρη)
                if (dv != INF)
                    B[dv].erase(dist[v].second);

                // Ανανέωση των αποστάσεων
                dist[v].first = du + weight;
                dv = dist[v].first;

                // Τοποθέτηση της κορυφής σε bucket
                B[dv].push_front(v);

                dist[v].second = B[dv].begin();
            }
        }
    }
}




int main()
{

    //Καθορισμός του άριθμου των κορυφών που θα έχει το γράφημα
    int V = 1000;
    vector<pair<int, int> > adj[V];


    //Δημιουργία του γραφήματος με τυχαίες τιμές
    int array_w[]={0};
    for (int i=0; i <1000 ; i++)
    {
        int a,b,c;
        a=rand()%100;
        b=rand()%100;
        c=rand()%100 + 10;
        array_w[i]=c;
        array_w[c];
        addEdge(adj,a,b,c);
    }


    //Εύρεση του μεγαλύτερου βάρους
    int max_w = array_w[0];
    for (int i=0; i<1000; i++)
    {
        if (array_w[i] > max_w)
                {max_w = array_w[i];}

    }
    cout<< "Max weight is: "<<max_w<< endl;


    //Μέτρηση χρόνου συνάρτησης Dial
    timer tim;
    tim.start();
    for(int i=0;i <10000; i++){
         Dial(0,max_w,V, adj);
    }
    tim.stop();
    cout<<"my_Dial: "<<tim.elapsed_time()/10000 <<" seconds"<<endl;




    //Δημιουργία πινάκων που θα χρειαστούν για τον DIjkstra
    //edge_array<int>  cost(G);
    //node_array<int>  dist0(G);
    //node_array<edge> pred(G);

    /*μέτρηση χρόνου συνάρτησης Dijkstra της LEDA
    timer tim2;
    tim2.start();
    for(int i=0;i <10000; i++){
         DIJKSTRA(adj,S,cost,dist,pred);
    }
    tim.stop();
    cout<<"DIjkstra Leda : "<<tim.elapsed_time()/10000 <<" seconds"<<endl; */

    return 0;
}

