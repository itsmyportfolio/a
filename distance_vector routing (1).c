
// Distance Vector  -- 	To find best paths

#include <stdio.h>

#define INFINITY 9999  //INFINITY (9999) means there is NO direct connection
#define MAX 5

struct Route {
    int distance[MAX];
    int next_hop[MAX];
} table[MAX];

void distanceVectorRouting(int cost[MAX][MAX], int nodes) {
    for (int i = 0; i < nodes; i++) {
        for (int j = 0; j < nodes; j++) {
            table[i].distance[j] = cost[i][j];
            table[i].next_hop[j] = j;
        }
    }

    int changed;
    do {
        changed = 0;
        for (int i = 0; i < nodes; i++) {
            for (int j = 0; j < nodes; j++) {
                for (int k = 0; k < nodes; k++) {
                    if (table[i].distance[j] > table[i].distance[k] + table[k].distance[j]) {
                        table[i].distance[j] = table[i].distance[k] + table[k].distance[j];
                        table[i].next_hop[j] = k;
                        changed = 1;
                    }
                }
            }
        }
    } while (changed);

    printf("\nRouting Table:\n");              // To store best next hops and distances
    for (int i = 0; i < nodes; i++) {
        printf("Router %d:\n", i);
        for (int j = 0; j < nodes; j++) {
            printf("To %d via %d Distance: %d\n", j, table[i].next_hop[j], table[i].distance[j]);
        }
        printf("\n");
    }
}

int main() {
    int cost[MAX][MAX] = {
        {0, 2, INFINITY, 1, INFINITY},
        {2, 0, 3, INFINITY, INFINITY},
        {INFINITY, 3, 0, 1, 5},
        {1, INFINITY, 1, 0, 2},
        {INFINITY, INFINITY, 5, 2, 0}
    };
    
    int nodes = 5;
    distanceVectorRouting(cost, nodes);
    
    return 0;
}
