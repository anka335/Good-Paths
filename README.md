# "Good Paths" - combinatorial optimization project by Joanna Zubik and Jędrzej Kolski
## short version - https://optil.io/optilion/problem/3225#tab-4 (anka3352)
## long version - https://optil.io/optilion/problem/3227#tab-4 (anka3352)

## short version file - Joanna_Zubik_Jedrzej_Kolski.cpp
## long version file - Joanna_Zubik_Jedrzej_Kolski_long.cpp
## documentation - Good_Paths.pdf

project was created in pair with Jędrzej Kolski (https://github.com/Jeziorop?tab=repositories)


# Good Paths Short
https://optil.io/optilion/problem/3225

## Main Idea

At the beginning we check for a graph type and compress graph. Then depending on which one is it, we run backtrack or heuristic-based search and then the local search on the best path found.

## More in-depth description of used techniques

### Data analysis and graph types

We searched for percentage of bridges, number of components and average degree in different types. By running test programs that showed solutions corresponding to different cases we distinguished 7 different graph types (you can see their description on figures below)

  
<img width="722" height="253" alt="image" src="https://github.com/user-attachments/assets/45991eb8-1bc1-400d-a4ad-600ddad18d3f" />
<br>
<img width="668" height="164" alt="image" src="https://github.com/user-attachments/assets/bccbe5d1-67b0-4701-a772-fab14370381d" />
<br>
<img width="408" height="159" alt="image" src="https://github.com/user-attachments/assets/69dc0ba2-e225-42d9-b516-9492da783904" />
<br>
<img width="602" height="163" alt="image" src="https://github.com/user-attachments/assets/23e3a7ef-9679-4840-9ee6-824a45db6552" />
<br>
<img width="487" height="164" alt="image" src="https://github.com/user-attachments/assets/00fc5524-31c7-431f-8d5f-4cee21c5cfb9" />
<br>
<img width="294" height="349" alt="image" src="https://github.com/user-attachments/assets/79ebf0f0-31bc-478c-aa79-944f3be7e5a7" />



### Solution for each graph type

Each type was compressed in the same way because we found it easier to manage later, even if it does not do much

- Type 1:
  - Graph sorting based on weights
  - Heuristic-based search to 3.5 second of program run
  - Local search to 19.3 second of program run
    - Alternative tail not random and not using backtrack
    - Alternative tail not random and using backtrack
    - Detour random, reversed and with max depth equal to 25000
    - Extension
- Type 2:
  - Graph sorting based on weights
  - Heuristic-based search to 3.5 second of program run
  - Local search to 19.0 second of program run
    - Alternative tail not random and not using backtrack
    - Alternative tail not random and using backtrack
    - Detour random, reversed and with max depth equal to 10000
    - Extension
- Type 3:
  - Default graph sorting
  - Backtrack to 6.5 second of program run
  - Local search to 19.3 second of program run
    - Alternative tail not random and not using backtrack
    - Alternative tail random and not using backtrack
    - Detour random, reversed and with max depth equal to 9000
    - Extension
- Type 4:
  - Graph sorting based on weights
  - Backtrack to 15.5 second of program run
  - Local search to 19.4 second of program run
    - Alternative tail not random and not using backtrack
    - Alternative tail random and not using backtrack
    - Detour not random, reversed and with max depth equal to 10000
    - Extension
- Type 5:
  - Graph sorting based on weights
  - Backtrack to 16.5 second of program run
  - Local search to 19.4 second of program run
    - Alternative tail not random and using backtrack
    - Alternative tail random and not using backtrack
    - Detour random, reversed and with max depth equal to 1000
    - Extension
- Type 6:
  - Heuristic-based search to 3.5 second of program run
  - Local search to 19.1 second of program run
    - Alternative tail not random and not using backtrack
    - Alternative tail not random and using backtrack
    - Detour random, reversed and with max depth equal to 100
    - Extension
- Type 7:
  - Heuristic-based search to 3.5 second of program run
  - Local search to 19.1 second of program run
    - Alternative tail not random and not using backtrack
    - Alternative tail random and not using backtrack
    - Detour not random, reversed and with max depth equal to 20000
    - Extension

### Graph compression

We compress graph based on „branches" which are nodes that have 2 at most 2 neighbors. To maintain properties of the original graph we compress only branches which all neighbors are also branches. Based on them we create one new node (called compressed node or comp node) that has a weight - number of nodes that it absorbed and took place of those compressed branches. Other remain nodes get weight 1.


<img width="344" height="356" alt="image" src="https://github.com/user-attachments/assets/d9134a98-701d-42ba-91f7-f74af6dad635" />


### Backtrack

We start it at a random node and run it for some particular time. Its purpose is to find a first best path that will be then changed by local search.

### Heuristic-based search (start search)

Heuristic approach assumes, that the best extension is the one which sees as little unseen nodes (nodes that haven't been seen by nodes from path) as possible (at least one).

### Local search (main search) - the idea

In local search we run different operations for as long as we can based on graph types. We check for alternative tail - a new part of the path that goes from our best path to some unseen node, detour - a new part of the path that goes from some node on our best path to the another and extension - extending this path as much as possible.

### Local search (main search) - alternative tail

For each node from the path we start search for a new path (tail), it can't cross any previous part of the path. When extension of tail isn't further possible we check if it's beneficial to exchange it with left or right tail (nothing fancy, just by lengths of these tails).

### Local search (main search) - detour

For each node from the path we look for a detour. We extend such detour and after each extension we check if it's possible to connect it back to the main path, then we check if such exchange is beneficial by looking up prefix values.

### Local search (main search) - extension

Extends ends of current path as far as possible using heuristic and random approaches.

### Decompression

Our last important part of the code is decompression. It is not always necessary as some compressed graphs (comp graphs) have no added nodes. We use data structures created in the compression step and recreate the final path

### Important data structures

vector<vector<_int_\>> graph - original graph

vector<vector<_int_\>> comp_graph - compressed graph with new nodes (comp nodes)

vector<_int_\> which_component - which node has which graph component

vector<_int_\> which_comp - which node belongs to which compressed node

vector<_int_\> weights - weight of each node (number of nodes that it absorbed during compression)

list<_int_\> final_path - we used list for final path, because it was the fastest data structure for this kind of problem (we could add new nodes wherever we wanted in O(1))

vector&lt;CompNode&gt; comp_nodes - vector that keeps newly added nodes (comp nodes) and information about them

vector&lt;int&gt; seen - a lookup table that stores for each node number of neighbours that belong to the path

vector&lt;bool&gt; vis - a lookup table that's true if given node is in current path and false otherwise

# Good paths long
https://optil.io/optilion/problem/3227

## What was changed

We added new time limits for each type that were about 9 times larger than those before in short version.

# Responsibilities

Most of ideas we created in cooperation, so these responsibilities are not that straightforward.

## Joanna Zubik

Was responsible for testcase analysis, graph compression, decompression and backtracking (it sums up to about half of the code) and most of this documentation except of heuristic-based search and local search explanation.

## Jędrzej Kolski

 Was responsible for heuristic-based search, local search, set time limits and helped with backtracking and documentation.
