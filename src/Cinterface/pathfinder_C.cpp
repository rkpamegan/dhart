#include <pathfinder_C.h>

#include <memory>
#include <vector>

#include <HFExceptions.h>
#include <graph.h>
#include <path_finder.h>
#include <path.h>

using std::unique_ptr;
using std::make_unique;
using std::vector;

using HF::SpatialStructures::Graph;
using HF::SpatialStructures::Path;
using HF::SpatialStructures::PathMember;
using namespace HF::Pathfinding;

C_INTERFACE CreatePath(
	const Graph* g,
	int start,
	int end,
	int* out_size,
	Path** out_path,
	PathMember** out_data
) {
	// Get a boost graph from this graph
	auto bg = CreateBoostGraph(*g);
	Path* P = new Path();

	*P = FindPath(bg.get(), start, end);

	if (!P->empty()) {
		*out_path = P;
		*out_data = P->GetPMPointer();
		*out_size = P->size();
		return HF::Exceptions::HF_STATUS::OK;
	}
	else {
		delete P;
		return HF::Exceptions::HF_STATUS::NO_PATH;
	}
}

C_INTERFACE CreatePaths(
	const Graph* g,
	const int* start,
	const int* end,
	Path** out_path_ptr_holder,
	PathMember** out_path_member_ptr_holder,
	int* out_sizes,
	int num_paths
) {
	vector<int> starts(start, start + num_paths);
	vector<int> ends(end, end + num_paths);

	auto bg = CreateBoostGraph(*g);

	InsertPathsIntoArray(
		bg.get(),
		starts,
		ends,
		out_path_ptr_holder,
		out_path_member_ptr_holder,
		out_sizes
	);

	return HF::Exceptions::HF_STATUS::OK;
}

C_INTERFACE CreatePathCostType(
	const HF::SpatialStructures::Graph* g,
	int start,
	int end,
	int* out_size,
	HF::SpatialStructures::Path** out_path,
	HF::SpatialStructures::PathMember** out_data,
	const char* cost_name
) {
	// Get a boost graph from *g
	std::unique_ptr<BoostGraph, BoostGraphDeleter> bg;
	
	try {
		// If cost_name is not a valid cost type in *g,
		// std::out_of_range is thrown by Graph::GetEdges
		bg = CreateBoostGraph(*g, std::string(cost_name));
	}
	catch (std::out_of_range) {
		// should really be HF::Exceptions::HF_STATUS::NO_COST
		// but we do not have that yet. Will be left to issue #24.
		return HF::Exceptions::HF_STATUS::GENERIC_ERROR;
	}

	Path* P = new Path();

	*P = FindPath(bg.get(), start, end);

	if (!P->empty()) {
		*out_path = P;
		*out_data = P->GetPMPointer();
		*out_size = P->size();
		return HF::Exceptions::HF_STATUS::OK;
	}
	else {
		delete P;
		return HF::Exceptions::HF_STATUS::NO_PATH;
	}
}

C_INTERFACE CreatePathsCostType(
	const HF::SpatialStructures::Graph* g,
	const int* start,
	const int* end,
	HF::SpatialStructures::Path** out_path_ptr_holder,
	HF::SpatialStructures::PathMember** out_path_member_ptr_holder,
	int* out_sizes,
	int num_paths,
	const char* cost_name) {

	vector<int> starts(start, start + num_paths);
	vector<int> ends(end, end + num_paths);

	// Get a boost graph from *g
	std::unique_ptr<BoostGraph, BoostGraphDeleter> bg;
	
	try {
		// If cost_name is not a valid cost type in *g,
		// std::out_of_range is thrown by Graph::GetEdges
		bg = CreateBoostGraph(*g, std::string(cost_name));
	}
	catch (std::out_of_range) {
		// should really be HF::Exceptions::HF_STATUS::NO_COST
		// but we do not have that yet. Will be left to issue #24.
		return HF::Exceptions::HF_STATUS::GENERIC_ERROR;
	}

	InsertPathsIntoArray(
		bg.get(),
		starts,
		ends,
		out_path_ptr_holder,
		out_path_member_ptr_holder,
		out_sizes
	);

	return HF::Exceptions::HF_STATUS::OK;
}

/// <summary>
/// Get the size of a path
/// </summary>
C_INTERFACE GetPathInfo(
	HF::SpatialStructures::Path* p,
	PathMember** out_member_ptr,
	int* out_size
) {
	if (p) {
		*out_size = p->size();
		*out_member_ptr = p->GetPMPointer();
		return HF::Exceptions::HF_STATUS::OK;
	}
	else {
		*out_size = -1;
		*out_member_ptr = NULL;
		return HF::Exceptions::HF_STATUS::NO_PATH;
	}
}


C_INTERFACE DestroyPath(Path* path_to_destroy) {
	DeleteRawPtr(path_to_destroy);
	return HF::Exceptions::OK;
}

C_INTERFACE CreateAllToAllPaths(
	const HF::SpatialStructures::Graph* g,
	HF::SpatialStructures::Path** out_path_ptr_holder,
	HF::SpatialStructures::PathMember** out_path_member_ptr_holder,
	int* out_sizes,
	int num_paths
) {

	auto bg = CreateBoostGraph(*g);
	InsertAllToAllPathsIntoArray(bg.get(), out_path_ptr_holder, out_path_member_ptr_holder, out_sizes);

	return HF::Exceptions::OK;
}