import math

from humanfactorspy.geometry import LoadOBJ, CommonRotations
from humanfactorspy.raytracer import EmbreeBVH
from humanfactorspy.graphgenerator.graph_generator import GenerateGraph
import humanfactorspy

def test_GetNodes():
    mesh_path = humanfactorspy.get_sample_model("plane.obj")
    obj = LoadOBJ(mesh_path, rotation=CommonRotations.Yup_to_Zup)
    bvh = EmbreeBVH(obj)
    g = GenerateGraph(bvh, (0, 0, 0.1), (0.5, 0.5, 0.5), 1000)
    node_list = g.getNodes()
    assert len(node_list.array) > 0
    print(g.CompressToCSR())


def test_UnsuccessfulGraphCreationDoesntThrow():
    mesh_path = humanfactorspy.get_sample_model("plane.obj")
    obj = LoadOBJ(mesh_path, rotation=CommonRotations.Yup_to_Zup)
    bvh = EmbreeBVH(obj)
    g = GenerateGraph(bvh, (200000, 0, 0.1), (0.5, 0.5, 0.5), 1000)
    assert g is None


def test_EnsureParallelGraphIsEquivalentToStandardGraph():
    mesh_path = humanfactorspy.get_sample_model("plane.obj")
    obj = LoadOBJ(mesh_path, rotation=CommonRotations.Yup_to_Zup)
    bvh = EmbreeBVH(obj)

    start_point = (0, 0, 1)
    spacing = (1, 1, 1)
    max_nodes = 500

    parallel_graph = GenerateGraph(bvh, start_point, spacing, max_nodes, cores=-1)
    standard_graph = GenerateGraph(bvh, start_point, spacing, max_nodes, cores=0)

    parallel_nodes = parallel_graph.getNodes()
    standard_nodes = standard_graph.getNodes()
    parallel_array = parallel_nodes.array
    standard_array = standard_nodes.array

    # Note that the parallel graph can potentially have more nodes since it
    # performs calculations in batches instead of one by one
    assert len(parallel_array) >= len(standard_array)
    for i in range(0, len(standard_array)):
        std_node = standard_array[i]
        par_node = parallel_array[i]

        distance = math.sqrt(
                            pow(std_node[0] - par_node[0], 2)
                            + pow(std_node[1] - par_node[1], 2)
                            + pow(std_node[2] - par_node[2], 2))

        assert distance < 0.001