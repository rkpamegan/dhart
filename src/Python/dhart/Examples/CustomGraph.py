"""

"""
import random
import math
from typing import Tuple
from ctypes import *
from dhart.spatialstructures import *
from dhart.raytracer import EmbreeBVH
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

def dist(a : Tuple[float, float, float], b : Tuple[float, float, float]) -> float:
    """ Calculate the squared distance between two points in 3D space """
    return math.sqrt((a[0] - b[0]) ** 2 + (a[1] - b[1]) ** 2 + (a[2] - b[2]) ** 2)

g = Graph()
for (a,b) in edges:
    g.AddEdgeToGraph(node_list[a], node_list[b], 1, "")

g.CompressToCSR()
print(g.getNodes().array)
node1_id = g.GetNodeID(node_list[0])
node2_id = g.GetNodeID(node_list[11])

print(DijkstraShortestPath(g, node1_id.value, node2_id.value))

# CalculateCrossSlope(g)
# CalculateEnergyExpenditure(g)
# g.CompressToCSR()
# print(DijkstraShortestPath(g, node1_id.value, node2_id.value, cost_type=CostAlgorithmKeys.CROSS_SLOPE))