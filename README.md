# Router Max-Flow Simulator

A small C program that generates a random network of routers and computes the **maximum bandwidth** that can flow from the first router to the last one, using the **Edmonds-Karp** algorithm (Ford-Fulkerson with BFS).

## How the network is modeled

- **Routers.** You choose how many routers (N) to create. Each one gets a random `downspeed` between 1 and 100.
- **Links.** Every ordered pair of routers has a 50% chance of a one-way link, so the network is a **directed graph** stored as an N×N adjacency matrix.
- **In/out tables.** For each router, the program prints which routers link *into* it (`in` table) and which routers it links *out* to (`out` table).
- **Link capacity.** A link's capacity is the downspeed of the **slower** of its two routers, because a link is only as fast as its weakest end.
- **Max flow.** The program computes the maximum flow from router 1 (source) to router N (sink). If no path connects them, it says so.

## The algorithm

Edmonds-Karp repeatedly:

1. Uses **BFS** on the residual graph to find the shortest path from source to sink that still has spare capacity.
2. Pushes as much flow along that path as its bottleneck edge allows.
3. Updates the residual capacities, including reverse edges, so earlier routing choices can be undone.

It stops when no augmenting path remains. The total flow pushed is the maximum flow. Running time is O(V·E²).

## Build and run

Requires a C99 compiler (for variable-length arrays).

```bash
gcc -std=c99 -Wall -Wextra -o router_maxflow router_maxflow.c
./router_maxflow
```

The program asks for the number of routers (at least 2).

## Example

A run with 4 routers:

```text
give number of routers: 4

0 1 1 0
1 0 1 1
1 1 0 1
1 0 1 0

router 1 in table: 0 1 1 1
router 2 in table: 1 0 1 0
router 3 in table: 1 1 0 1
router 4 in table: 0 1 1 0

router 1 out table: 0 1 1 0
router 2 out table: 1 0 1 1
router 3 out table: 1 1 0 1
router 4 out table: 1 0 1 0

 routing table after weights are included:
0 9 71 0
9 0 9 9
71 9 0 71
73 0 71 0

router 1 downspeed: 80
router 2 downspeed: 9
router 3 downspeed: 71
router 4 downspeed: 73

max flow: 80
```

Router 1 can send at most 9 + 71 = 80 through its two outgoing links, and router 4 can receive exactly 9 + 71 = 80 through its incoming links, so the max flow is 80.

Since the network is random, every run gives different output.

## Possible extensions

- Use each router's `maxload` and `load` fields (already in the `Router` struct) to simulate congestion.
- Let the user choose the source and sink routers.
- Add a seed argument so runs can be reproduced.

## Credits

The `bfs()` and `fordFulkerson()` functions are adapted from the GeeksforGeeks article *"Ford-Fulkerson Algorithm for Maximum Flow Problem"*. The network model, router simulation and in/out tables are my own work.
