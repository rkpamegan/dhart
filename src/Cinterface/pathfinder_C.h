#include <cinterface_utils.h>

#define C_INTERFACE extern "C" __declspec(dllexport) int

namespace HF {
	namespace SpatialStructures { class Graph; class Path; class PathMember; }
	namespace Pathfinding { class BoostGraph; }
}

/**
* @defgroup Pathfinding
Find paths between differnt points in a graph.
* @{
*/

/// <summary>
/// Create a path from start to end in C++. If no path exists, the NO_PATH error code will be
/// returned, and both pointers will be left as they were before the function was called.
/// </summary>
/// <param name="g"> The graph to conduct the search on. </param>
/// <param name="start">
/// The id of the node for the starting point of the path. The ID must belong to a node already
/// within the graph!
/// </param>
/// <param name="end">
/// The id of the node for the end point of the path. The ID must belong to a node already within
/// the graph!
/// </param>
/// <param name="out_path"> Return parameter for the path object. </param>
/// <param name="out_data"> Return parameter for the path's members. </param>
/// <returns>
/// HF_STATUS::NO_PATH if a path could not be generated. HF_STATUS::OK if a path was generated.
/// </returns>
/// <remarks> Uses the Boost Graph Library to perform Dijkstra's shortest path algorithm. For multiple paths, use <see cref="CreatePaths"/> for multiple paths at once. </remarks>
/*!
	\code
		// Requires #include "pathfinder_C.h", #include "graph.h", #include "path.h", #include "path_finder.h"

		// Create a Graph g, and compress it.
		HF::SpatialStructures::Graph g;
		g.addEdge(0, 1, 1);
		g.addEdge(0, 2, 2);
		g.addEdge(1, 3, 3);
		g.addEdge(2, 4, 1);
		g.addEdge(3, 4, 5);
		g.Compress();

		// Create a boostGraph from g
		auto boostGraph = HF::Pathfinding::CreateBoostGraph(g);

		// Prepare parameters for CreatePath
		HF::SpatialStructures::Path* out_path = nullptr;
		HF::SpatialStructures::PathMember* out_path_member = nullptr;
		int out_size = -1;

		CreatePath(&g, 0, 4, &out_size, &out_path, &out_path_member);

		// Use out_path, out_path_member

		// Remember to free resources when finished
		DestroyPath(out_path);

		// At this point, out_path_member has also been destroyed, so we set this to nullptr
		out_path_member = nullptr;
	\endcode
*/
C_INTERFACE CreatePath(
	const HF::SpatialStructures::Graph* g,
	int start,
	int end,
	int* out_size,
	HF::SpatialStructures::Path** out_path,
	HF::SpatialStructures::PathMember** out_data
);

/// <summary>
/// Create multiple paths from start to end in C++. both out_ members return as arrays of pointers,
/// with one for each element. Sizes of zero represent non-existant paths, and will be set to null pointers
/// </summary>
/// <param name="g"> The graph to conduct the search on. </param>
/// <param name="start">
/// An array of ids for starting nodes. Length must match that of end and all the IDS must belong to
/// nodes that already exist within the graph.
/// </param>
/// <param name="end">
/// An array of ids for ending nodes. Length must match that of start and all the IDS must belong to
/// nodes that already exist within the graph.
/// </param>
/// <param name="out_path_ptr_holder"> Return parameter for path objects. </param>
/// <param name="out_data"> Return parameter for pointers to the path objects' underlying data. </param>
/// <param name="out_sizes">
/// Output array of integers representing the length of of each path in out_data's arrays. Sizes of
/// 0 indicate that no path could be generated.
/// </param>
/// <param name="num_paths">Size of start and end arrays</param>
/// <returns> HF::OK on completion. </returns>

/*!
	\code
		// Requires #include "pathfinder_C.h", #include "graph.h", #include "path.h", #include "path_finder.h"

		HF::SpatialStructures::Graph g;
		g.addEdge(0, 1, 1);
		g.addEdge(0, 2, 2);
		g.addEdge(1, 3, 3);
		g.addEdge(2, 4, 1);
		g.addEdge(3, 4, 5);
		g.Compress();

		// Maximum amount of paths to search
		const int MAX_SIZE = 2;

		// Create a Graph g, and compress it
		auto boostGraph = HF::Pathfinding::CreateBoostGraph(g);

		// We want to find the shortest paths from 0 to 3, and 0 to 4.
		int start_nodes[] = { 0, 0 };
		int end_nodes[] = { 3, 4 };

		// Create dynamically-allocated arrays of pointers to Path
		// Create dynamically-allocated arrays of pointers to PathMember
		HF::SpatialStructures::Path** out_path = new HF::SpatialStructures::Path * [MAX_SIZE];
		HF::SpatialStructures::PathMember** out_path_member = new HF::SpatialStructures::PathMember * [MAX_SIZE];

		// Sizes of paths generated by CreatePaths. Sizes of 0 mean that a path was unable to be generated.
		int* out_sizes = new int[MAX_SIZE];

		// Use CreatePaths
		CreatePaths(&g, start_nodes, end_nodes, out_path, out_path_member, out_sizes, MAX_SIZE);

		///
		/// Resource cleanup
		///

		for (int i = 0; i < MAX_SIZE; i++) {
			if (out_path[i]) {
				// Release memory for all pointers in out_path
				DestroyPath(out_path[i]);
				out_path[i] = nullptr;
			}
		}

		if (out_path) {
			// Release memory for pointer to out_path buffer
			delete[MAX_SIZE] out_path;
			out_path = nullptr;
		}

		if (out_path_member) {
			// Release memory for pointers to out_path_member buffer
			delete[MAX_SIZE] out_path_member;
			out_path_member = nullptr;
		}

		if (out_sizes) {
			// Release memory for pointer to out_sizes buffer
			delete[MAX_SIZE] out_sizes;
			out_sizes = nullptr;
		}
	\endcode
*/
C_INTERFACE CreatePaths(
	const HF::SpatialStructures::Graph* g,
	const int* start,
	const int* end,
	HF::SpatialStructures::Path** out_path_ptr_holder,
	HF::SpatialStructures::PathMember** out_path_member_ptr_holder,
	int* out_sizes,
	int num_paths
);

/*!

*/
C_INTERFACE CreatePathCostType(
	const HF::SpatialStructures::Graph* g,
	int start,
	int end,
	int* out_size,
	HF::SpatialStructures::Path** out_path,
	HF::SpatialStructures::PathMember** out_data,
	const char *cost_name
);

/*!
	
*/
C_INTERFACE CreatePathsCostType(
	const HF::SpatialStructures::Graph* g,
	const int* start,
	const int* end,
	HF::SpatialStructures::Path** out_path_ptr_holder,
	HF::SpatialStructures::PathMember** out_path_member_ptr_holder,
	int* out_sizes,
	int num_paths,
	const char* cost_name
);

/// <summary> Get the size of a path and a pointer to its path members. </summary>
/// <param name="p"> Pointer to the path to get information from. This can handle null values. </param>
/// <param name="out_member_ptr"> Pointer to the path to get information from. Should not be null. </param>
/// <param name="out_size"> The number of path members in the path. </param>
/// <returns> HF_STATUS::NO_PATH if the path is not valid. HF_OK otherwise. </returns>

/*!
	\code
		// Requires #include "pathfinder_C.h", #include "path.h"

		// Create a Graph g, and compress it.
		HF::SpatialStructures::Graph g;
		g.addEdge(0, 1, 1);
		g.addEdge(0, 2, 2);
		g.addEdge(1, 3, 3);
		g.addEdge(2, 4, 1);
		g.addEdge(3, 4, 5);
		g.Compress();

		// Create a boostGraph from g
		auto boostGraph = HF::Pathfinding::CreateBoostGraph(g);

		HF::SpatialStructures::Path* out_path = nullptr;
		HF::SpatialStructures::PathMember* out_path_member = nullptr;
		int out_size = -1;

		CreatePath(&g, 0, 4, &out_size, &out_path, &out_path_member);

		// Get p's info, store results in out_path_member and out_size
		GetPathInfo(p, &out_path_member, &out_size);

		// Remember to free resources when finished
		DestroyPath(out_path);

		// At this point, out_path_member has also been destroyed, so we set this to nullptr
		out_path_member = nullptr;
	\endcode
*/
C_INTERFACE GetPathInfo(
	HF::SpatialStructures::Path* p,
	HF::SpatialStructures::PathMember** out_member_ptr,
	int* out_size
);

/// <summary> Delete a path. </summary>
/// <param name="path_to_destroy"> Pointer to the path to delete. </param>

/*!
	\code
		// Requires #include "pathfinder_C.h", #include "graph.h", #include "path.h", #include "path_finder.h"

		// Create a Graph g, and compress it.
		HF::SpatialStructures::Graph g;
		g.addEdge(0, 1, 1);
		g.addEdge(0, 2, 2);
		g.addEdge(1, 3, 3);
		g.addEdge(2, 4, 1);
		g.addEdge(3, 4, 5);
		g.Compress();

		// Create a boostGraph from g
		auto boostGraph = HF::Pathfinding::CreateBoostGraph(g);

		HF::SpatialStructures::Path* out_path = nullptr;
		HF::SpatialStructures::PathMember* out_path_member = nullptr;
		int out_size = -1;

		CreatePath(&g, 0, 4, &out_size, &out_path, &out_path_member);

		// Use out_path, out_path_member

		// Remember to free resources when finished
		DestroyPath(out_path);

		// At this point, out_path_member has also been destroyed, so we set this to nullptr
		out_path_member = nullptr;
	\endcode
*/
C_INTERFACE DestroyPath(HF::SpatialStructures::Path* path_to_destroy);


/// <summary>
/// Create multiple paths from start to end in C++. both out_ members return as arrays of pointers,
/// with one for each element. Sizes of zero represent non-existant paths, and will be set to null pointers
/// </summary>
/// <param name="g"> The graph to conduct the search on. </param>
/// <param name="out_path_ptr_holder"> Return parameter for path objects. </param>
/// <param name="out_path_member_ptr_holder"> Return parameter for pointers to the path objects' underlying data. </param>
/// <param name="out_sizes">
/// Output array of integers representing the length of of each path in out_data's arrays. Sizes of
/// 0 indicate that no path could be generated.
/// </param>
/// <param name="num_paths">Size of start and end arrays</param>
/// <returns> HF::OK on completion. </returns>

/*!
	\code
		HF::SpatialStructures::Graph g;

		// Add the edges
		g.addEdge(0, 1, 1);
		g.addEdge(0, 2, 2);
		g.addEdge(1, 3, 3);
		g.addEdge(1, 4, 4);
		g.addEdge(2, 4, 4);
		g.addEdge(3, 5, 5);
		g.addEdge(4, 6, 3);
		g.addEdge(5, 6, 1);

		// Always compress the graph after adding edges
		g.Compress();

		// Create a BoostGraph (std::unique_ptr)
		auto bg = CreateBoostGraph(g);

		// Total paths is node_count ^ 2
		size_t node_count = g.Nodes().size();
		size_t path_count = node_count * node_count;

		// Pointer to buffer of (Path *)
		Path** out_paths = new Path * [path_count];
		// out_paths[i...path_count - 1] will be alloc'ed by InsertPathsIntoArray

		// Pointer to buffer of (PathMember *)
		PathMember** out_path_member = new PathMember * [path_count];
		// out_path_member[i...path_count - 1] points to out_paths[i...path_count - 1]->GetPMPointer();

		// Pointer to buffer of (int)
		int* sizes = new int[path_count];

		//
		// The two loops for start_points and end_points
		// are just for the output.
		//
		int curr_id = 0;
		std::vector<int> start_points(path_count);
		// Populate the start points,
		// size will be (node_count)^2
		for (int i = 0; i < node_count; i++) {
			for (int k = 0; k < node_count; k++) {
				start_points[curr_id++] = i;
			}
		}

		curr_id = 0;

		std::vector<int> end_points(path_count);
		// Populate the end points,
		// size will be (node_count)^2
		for (int i = 0; i < node_count; i++) {
			for (int k = 0; k < node_count; k++) {
				end_points[curr_id++] = k;
			}
		}

		CreateAllToAllPaths(&g, out_paths, out_path_member, sizes, path_count);

		for (int i = 0; i < path_count; i++) {
			if (out_paths[i]) {
				// Always check if out_paths[i] is nonnull!
				int total_cost = 0;
				std::cout << "Path from " << start_points[i] << " to " << end_points[i] << std::endl;

				Path p = *out_paths[i];
				for (auto m : p.members) {
					total_cost += m.cost;
					std::cout << "node ID: " << m.node << "\tcost " << m.cost << std::endl;
				}

				std::cout << "Total cost: " << total_cost << std::endl;
				std::cout << "--------------------------" << std::endl;
			}
		}

		//
		// Resource cleanup
		//
		if (sizes) {
			delete[] sizes;
			sizes = nullptr;
		}

		if (out_path_member) {
			delete[] out_path_member;
			out_path_member = nullptr;
		}

		if (out_paths) {
			for (int i = 0; i < path_count; i++) {
				if (out_paths[i]) {
					delete out_paths[i];
					out_paths[i] = nullptr;
				}
			}
		}
	}
	\endcode
*/
C_INTERFACE CreateAllToAllPaths(
	const HF::SpatialStructures::Graph* g,
	HF::SpatialStructures::Path** out_path_ptr_holder,
	HF::SpatialStructures::PathMember** out_path_member_ptr_holder,
	int* out_sizes,
	int num_paths
);

/**@}*/
