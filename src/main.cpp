#include "LEDA/graph/graph.h"
#include "LEDA/core/random_source.h"
#include "LEDA/core/queue.h"
#include "LEDA/system/timer.h"
#include <iostream>


using namespace leda;
using namespace std;



list<node> my_BFS(const graph &G, node s, node_array<int> &dist, node_array<edge> &pred){

    queue<node> Q; //δημιουργία ουράς για τις κορυφές του γραφήματος
    node v;
    forall_nodes(v,G){    //ορισμός του επιπέδου όλων των κορυφών σε -1 για να ξέρουμε οτι δεν έχουν βρεθεί ακόμα
        dist[v] = -1;
    }

    dist[s] = 0; //ορισμός του επιπέδου του κόμβου s σε 0, αρχικός κόμβος
    Q.append(s); // εισαγωγή του κόμβου s στην ουρά

    list<node> my_list; //δημιουργία λίστας η οποία θα περιλαμάβει τις κορυφές που έχει ανακαλύψει ο BFS

    edge e;

    while ( !Q.empty() ){ //Όσο η ουρά δεν είναι άδεια
        v = Q.pop(); //αφαιρούμε την πρώτη κορυφή από την ουρά
        my_list.append(v); // προσθέτουμε την κορυφή στην λίστα

        forall_out_edges(e,v){ //για όλες τις ακμές που εξέρχονται απο την κορυφή v
            node targ= G.target(e); //κορυφή προορισμού
            if (dist[targ] < 0){ // αν το επίπεδο της κορυφής είναι -1, δηλαδή δεν έχει βρεθεί ακόμα
                Q.append(targ); //προσθέτουμε την κορυφή στην ουρά
                dist[targ] = dist[v]+1; //αυξάνουμε το επίπεδο της απόστασης
                pred[targ]=e;//ορίζουμε τον προκάτοχο της ακμής
            }
        }
    }
    return my_list; //επιστρέφεται η λίστα με τις κορυφές που έχουν ανακαλυφθεί
}



bool my_bipar_checker(const graph &G, list<node> &V1, list<node> &V2) {

    node_array<int> dist(G); // πίνακας επιπέδων κορυφών
    node_array<edge> pred(G); // πίνακας προκατόχου ακμών

    node S = G.choose_node(); //διαλέγουμε τυχαία μια κορυφή για να ξεκινήσουμε

    my_BFS(G,S,dist,pred); //καλούμε την συνάρτηση my_BFS

    node v;
    edge e;

    node_array<int> color(G); //δημιουγία του πίνακα χρωμάτων για τις κορυφές

    forall_nodes(v, G) { //για κάθε κορυφή στο γράφημα G.
        if (dist[v] % 2 == 0) { //αν η απόσταση είναι άρτιος αριθμός.
            color[v] = 1; //η κορυφή γίνεται πράσινη
        } 
        else { //αν η απόσταση είναι περιττός αριθμός.
            color[v] = 0; //η κορυφή γίνεται μπλε
        }
    }

    forall_edges(e,G){ //για κάθε ακμή του γραφήματος G
            node source = G.source(e); //ορίζεται κορυφή προέλευσης
            node target = G.target(e); //ορίζεται κορυφή προορισμού

            if(color[source]==color[target]){ // αν η κορυφή προέλευσης και η κορυφή προορισμού έχουν το ίδιο χρώμα
                    list<node> kyklos; //δημηιουργία λίστας για τις κορυφές που αποτελόυν τον κύκλο
                    int flag=0;
                    kyklos.append(source);//προσθέτουμε κορυφές στον κύκλο
                    V1.append(source);

                    //Βρίσκουμε εισερχόμενες και εξερχόμενες ακμές

                    edge akmh;
                    edge akmh2;
                    akmh =pred[source]; //Βρίσκουμε την ακμή προέλευσης του κόμβου 
                    akmh2 =pred[target]; //Βρίσκουμε την ακμή προορισμού του κόμβου


                    while(akmh != nil){ //Αν υπάρχει η ακμή προέλευσης
                        source = G.source(akmh); //παίρνουμε τον κόμβο προέλευσης
                        kyklos.append(source); //εισάγουμε τον κόμβο στον κύκλο
                        akmh =pred[source]; //Βρίσκουμε την ακμή προέλευσης του κόμβου
                    }

                    while(akmh2 !=nil && flag==0){ //Αν υπάρχει η ακμή προορισμού
                        V1.append(target);//Εισαγωγή κόμβου στην λίστα του κύκλου
                        target=G.source(akmh2); //παίρνουμε την κορυφή προέλευσης
                        akmh2=pred[target];//Βρίσκουμε την ακμή προορισμού του κόμβου
                        if(kyklos.search(target) != nil){ //Αν βρεθεί ο κοινός κόμβος
                            flag =1;
                        }

                    }

                    node test;

                    do{ test =kyklos.pop(); } //Αφαιρούμε κορυφές από την λίστα μέχρι να βρούμε το target, δηλαδή τον κοινό πρόγονο.
                    while(test != target);

                    V1.append(test); //Εισαγωγή του κοινού κόμβου στην λίστα του κύκλου

                    return false; //επιστροφή false γιατί το γράφημα δεν είναι διμερές
            }
    }

                 //αν τα χρώματα των κορυφών δεν ταυτίζονται χωρίζουμε τις κορυφές στους δύο πίνακες ανάλογα με το χρώμα τους
                    forall_nodes(v,G){
                        if(color[v]==1){
                            V1.append(v);
                        }
                        else{
                            V2.append(v);
                        }
                        return true; //επιστροφή true γιατί το γράφημα είναι διμερές
                    }

}   


       
void Embbeded_Square_Graph(graph &G,list<node> &my_list,int koryfes, int counter){

    //Δημιουργία κορυφών τετραγώνου
    node a= G.new_node();
    node b= G.new_node();
    node c= G.new_node();
    node d= G.new_node();

    //Δημιουργία ακμών τετραγώνου
    G.new_edge(a,b);
    G.new_edge(b,c);
    G.new_edge(c,d);
    G.new_edge(d,a);

    //Δημιουργία αντίθετων ακμών για να γίνει το γράφημα μη κατευθυνόμενο
    G.new_edge(b,a);
    G.new_edge(c,b);
    G.new_edge(d,c);
    G.new_edge(a,d);

    if(!my_list.empty()){ //Αν η λίστα δεν είναι άδεια τότε σημαίνει οτι υπάρχει και άλλο τετράγωνο

        //Κόμβοι του προηγούμενου τετραγώνου
        node a2= my_list.pop();
        node b2= my_list.pop();
        node c2= my_list.pop();
        node d2= my_list.pop();

        //Ενώνουμε τις ακμές με το παλιό τετράγωνο
        G.new_edge(a,a2);
        G.new_edge(b,b2);
        G.new_edge(c,c2);
        G.new_edge(d,d2);

        //Δημιουργία αντίθετων ακμών για να γίνει το γράφημα μη κατευθυνόμενο
        G.new_edge(a2,a);
        G.new_edge(b2,b);
        G.new_edge(c2,c);
        G.new_edge(d2,d);


        counter = counter +4; //μετράμε τις κορυφές που προσθέσαμε
    }
    
    else{counter=4;}
        //Εισαγωγή των κορυφών στην λίστα
        my_list.append(a);
        my_list.append(b);
        my_list.append(c);
        my_list.append(d);


    if(counter<koryfes){ //αν οι κορυφές που έχουμε είναι λιγότερες από αυτές που θέλουμε, τότε
        Embbeded_Square_Graph(G,my_list,koryfes,counter); //κλήση ξανά της συνάρτησης για δημιουργία νέου τετραγώνου.

    }
}


void Ring_Graph(graph &G, int koryfes){

    //Δημιουργία των 2 πρώτων κορυφών
    node a=G.new_node();
    node b=G.new_node();

    //Δημιουργία των 2 πρώτων ακμών
    G.new_edge(a,b);
    G.new_edge(b,a);

    //Όσο έχουμε ακόμα κορυφές, δημιουργούμε κορυφές, τις ενώνουμε με ακμές και ενημερώνουμε την τελευταία κορυφή
    for(int i=0;i<koryfes-2;i++){
        node c=G.new_node();
        G.new_edge(c,b);
        G.new_edge(b,c);
        b=c;
    }

    //Ένωση της πρωτης με την τελευταία κορυφή
    G.new_edge(a,b);
    G.new_edge(b,a);



}
int main(){
	graph G;
    list<node> my_list;

    Embbeded_Square_Graph(G,my_list,50000,0); //Κλήση συνάρτησης δημιουργίας γραφήματος
    //Ring_Graph(G,50001); //Κλήση της συνάρτησης γραφήματος δακτυλίου (αριθμός κορυφών περιττός)

    list<node> V1;
    list<node> V2;

    //μέτρηση χρόνου για την συνάρτηση μου χρησιμοποιώντας συναρτήσεις της βιβλιοθήκης LEDA

    timer tim;
    tim.start();
    for(int i=0;i <10; i++){
        bool my= my_bipar_checker(G,V1,V2);
    }
    tim.stop();
    cout<<"my_bipar_checker: "<<tim.elapsed_time()/10 <<" seconds"<<endl;


    //μέτρηση χρόνου για την συνάρτηση της LEDA

    timer tim2;
    tim2.start();
    for(int i=0;i <10; i++){
        bool leda=Is_Bipartite(G,V1,V2);
    }
    tim2.stop();
    cout<<"Is_Bipartite: "<<tim2.elapsed_time()/10 << " seconds"<<endl;

    return 0;
}
