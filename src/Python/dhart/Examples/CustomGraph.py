"""

In this example we create a graph, then run a graph function on its custom nodes, edges, and costs. 

.. testcode::

    # Import required packages 
    from typing import Tuple
    from ctypes import *
    from dhart.spatialstructures import *
    from dhart.pathfinding import DijkstraShortestPath

    # Define the nodes we want to add to the graph, and how they will be connected.
    node_list = [(10, 2, 18), (9, 8, 10),
            (5, 1, 17), (15, 3, 17),
            (1, 17, 3), (11, 12, 4),
            (11, 20, 0), (0, 12, 8),
            (7, 11, 6), (1, 4, 15),
            (16, 5, 8), (17, 12, 2)]

    edges = [(0, 1), (0, 2), (0, 3), 
        (4, 5), (4, 6), (4, 7),
        (4, 8), (1, 9), (1, 2),
        (1, 10), (1, 3), (1, 7),
        (1, 8), (9, 2), (9, 7),
        (5, 10), (5, 6), (5, 8),
        (5, 11), (10, 3), (10, 8),
        (10, 11), (6, 11), (7, 8)]

    # Define a custom cost function
    def cost(a : Tuple[float, float, float], b : Tuple[float, float, float]) -> float:
        return abs(a[0] * b[0] + a[1] * b[1] + a[2] * b[2])

    # Create a graph and add default edges to make an unweighted base graph.
    g = Graph()
    for (a,b) in edges:
        g.AddEdgeToGraph(node_list[a], node_list[b], 1,"")
        g.AddEdgeToGraph(node_list[b], node_list[a], 1,"")

    # Compress the graph to a CSR format before adding custom costs.
    g.CompressToCSR()

    # Add the edges to the graph with the cost function
    cost_type = "cost"
    for (a,b) in edges:
        a_id = g.GetNodeID(node_list[a])
        b_id = g.GetNodeID(node_list[b])
        g.AddEdgeToGraph(a_id.value, b_id.value, cost(node_list[a], node_list[b]), cost_type)
        g.AddEdgeToGraph(b_id.value, a_id.value, cost(node_list[a], node_list[b]), cost_type)

    # Get the nodes at indices 1 and 5 of the graph's node array 
    node1_idx = 1
    node1 =  g.getNodes().array[node1_idx]
    print(node1)

    node2_idx = 5
    node2 = g.getNodes().array[node2_idx]
    print(node2)

    # Convert to NodeStruct
    x1, y1, z1, type1, id1 = node1
    node1_struct = NodeStruct(x1, y1, z1, type1, id1)
    x2, y2, z2, type2, id2 = node2
    node2_struct = NodeStruct(x2, y2, z2, type2, id2)

    # See the outgoing edges from node2
    print(g.GetEdgesForNode(node2_struct))

    # Check the edge cost for the first edge added to the graph with the custom cost
    print(g.GetEdgeCost(edges[0][0], edges[0][1], cost_type))

    # Get the shortest paths for the default cost and our custom cost
    print(DijkstraShortestPath(g, id1, id2))
    print(DijkstraShortestPath(g, id1, id2, cost_type))

.. testoutput:: 
    :options: +NORMALIZE_WHITESPACE

    (9.0, 8.0, 10.0, 0, 1)
    (11.0, 12.0, 4.0, 0, 5)
    [(( 1., 17., 3., 0,  4), 1, 1.) ((11., 20., 0., 0,  6), 1, 1.)
    (( 7., 11., 6., 0,  8), 1, 1.) ((16.,  5., 8., 0, 10), 1, 1.)
    ((17., 12., 2., 0, 11), 1, 1.)]
    286.0
    [(1.,  1) (1., 10) (0.,  5)]
    [(211., 1) (233., 8) (  0., 5)]

"""

from typing import Tuple
from ctypes import *
from dhart.spatialstructures import *
from dhart.pathfinding import DijkstraShortestPath


node_list = [(10, 2, 18),
            (9, 8, 10),
            (5, 1, 17),
            (15, 3, 17),
            (1, 17, 3),
            (11, 12, 4),
            (11, 20, 0),
            (0, 12, 8),
            (7, 11, 6),
            (1, 4, 15),
            (16, 5, 8),
            (17, 12, 2)]

edges = [(0, 1), (0, 2), (0, 3), 
         (4, 5), (4, 6), (4, 7),
         (4, 8), (1, 9), (1, 2),
         (1, 10), (1, 3), (1, 7),
         (1, 8), (9, 2), (9, 7),
         (5, 10), (5, 6), (5, 8),
         (5, 11), (10, 3), (10, 8),
         (10, 11), (6, 11), (7, 8)]

def cost(a : Tuple[float, float, float], b : Tuple[float, float, float]) -> float:
    return abs(a[0] * b[0] + a[1] * b[1] + a[2] * b[2])

g = Graph()

for (a,b) in edges:
    g.AddEdgeToGraph(node_list[a], node_list[b], 1,"")
    g.AddEdgeToGraph(node_list[b], node_list[a], 1,"")

g.CompressToCSR()

cost_type = "cost"
for (a,b) in edges:
    a_id = g.GetNodeID(node_list[a])
    b_id = g.GetNodeID(node_list[b])
    g.AddEdgeToGraph(a_id.value, b_id.value, cost(node_list[a], node_list[b]), cost_type)
    g.AddEdgeToGraph(b_id.value, a_id.value, cost(node_list[a], node_list[b]), cost_type)

node1_idx = 1
node1 =  g.getNodes().array[node1_idx]
print(node1)

node2_idx = 5
node2 = g.getNodes().array[node2_idx]
print(node2)

x1, y1, z1, type1, id1 = node1
node1_struct = NodeStruct(x1, y1, z1, type1, id1)
x2, y2, z2, type2, id2 = node2
node2_struct = NodeStruct(x2, y2, z2, type2, id2)

print(g.GetEdgesForNode(node2_struct))

print(g.GetEdgeCost(edges[0][0], edges[0][1], cost_type))

print(DijkstraShortestPath(g, id1, id2))
print(DijkstraShortestPath(g, id1, id2, cost_type))