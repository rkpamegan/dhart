#pragma once
#define NANORT_USE_CPP11_FEATURE
#include "nanort.h"
#include "meshinfo.h"
#include <iostream>
#include <array>
#include <meshinfo.h>


namespace HF::RayTracer{
    // Define struct of doubles to be used for ray and high precision raycasting
    struct double3 {
        double3() {}
        double3(double xx, double yy, double zz) {
            x = xx;
            y = yy;
            z = zz;
        }
        double3(const double* p) {
            x = p[0];
            y = p[1];
            z = p[2];
        }
        double3 operator*(double f) const { return double3(x * f, y * f, z * f); }
        double3 operator-(const double3& f2) const {
            return double3(x - f2.x, y - f2.y, z - f2.z);
        }
        double3 operator*(const double3& f2) const {
            return double3(x * f2.x, y * f2.y, z * f2.z);
        }
        double3 operator+(const double3& f2) const {
            return double3(x + f2.x, y + f2.y, z + f2.z);
        }
        double3& operator+=(const double3& f2) {
            x += f2.x;
            y += f2.y;
            z += f2.z;
            return (*this);
        }
        double3 operator/(const double3& f2) const {
            return double3(x / f2.x, y / f2.y, z / f2.z);
        }
        double operator[](int i) const { return (&x)[i]; }
        double& operator[](int i) { return (&x)[i]; }

        double3 neg() { return double3(-x, -y, -z); }

        double length() { return sqrt(x * x + y * y + z * z); }

        void normalize() {
            double len = length();
            if (fabs(len) > 1.0e-6) {
                double inv_len = 1.0 / len;
                x *= inv_len;
                y *= inv_len;
                z *= inv_len;
            }
        }

        double x, y, z;
        // double pad;  // for alignment
    };

} // namespace


// Derive our own class from the triangle intersector so we can store some extra data
class nanoRT_Data :
    public nanort::TriangleIntersector<double, nanort::TriangleIntersection<double> > {

public:

    // Don't let the default empty constructor work since it needs the mesh
    nanoRT_Data() = delete;
    // Add a mesh object
    HF::nanoGeom::Mesh mesh;
    // Add a ray object to be used for intersections
    nanort::Ray<double> ray;
    // Add a hit object to be referenced
    nanort::TriangleIntersection<double> hit;
    // Add a distance attribute to store intersection distance
    double dist = -1;
    double point[3] = { -1,-1,-1 };

    // Set initialization of class by passing a mesh to create a nanort::TriangleIntersector
    nanoRT_Data(HF::nanoGeom::Mesh mesh) : nanort::TriangleIntersector<double, nanort::TriangleIntersection<double> >(mesh.vertices, mesh.faces, sizeof(double) * 3)
    {
        // Set the mesh data
        this->mesh = mesh;

        // Setup a no hit (for safety)
        hit.u = -1;
        hit.v = -1;
        hit.t = -1;
        hit.prim_id = -1;

        // Setup the ray
        // Set origin of ray
        this->ray.org[0] = 0.0;
        this->ray.org[1] = 0.0;
        this->ray.org[2] = 0.0;

        this->ray.dir[0] = 0.0;
        this->ray.dir[1] = 0.0;
        this->ray.dir[2] = 0.0;

        // Set max and min location 
        this->ray.min_t = 0.0f;
        this->ray.max_t = 20000.0f;
    }

    // Destructor
    ~nanoRT_Data() {
        // Mesh data was constructed with new, delete here
        delete[] this->mesh.vertices;
        delete[] this->mesh.faces;
        std::cout << "Destroyed" << std::endl;
    }
};



namespace HF::nanoGeom {

    // Convenience method not used now but here for clarity
    bool nanoRT_RayCast(nanort::BVHAccel<double>& accel,
        nanort::TriangleIntersector<double, nanort::TriangleIntersection<double> >& triangle_intersector,
        nanort::Ray<double>& ray,
        nanort::TriangleIntersection<double>& isect);


    template <typename T>
    inline nanort::BVHAccel<T> nanoRT_BVH(unsigned int * indices, T * vertices, int num_vertices, int num_indices)
    {
        // Setup nanort tracer BVH options
        nanort::BVHBuildOptions<T> build_options; // Use default option
        build_options.cache_bbox = false;

        // Construct datatype using verts and indices for building BVH
        nanort::TriangleMesh<T> triangle_mesh(vertices, indices, sizeof(T) * 3);
        nanort::TriangleSAHPred<T> triangle_pred(vertices, indices, sizeof(T) * 3);

        // build BVH using NanoRT Method (Replace this assert with an exception)
        nanort::BVHAccel<T> accel;
        assert(accel.Build(num_indices, triangle_mesh, triangle_pred, build_options));

        // Return the BVH object
        return accel;
    }
    inline nanort::BVHAccel<double> nanoRT_BVH(Mesh mesh) {
        return nanoRT_BVH(mesh.faces, mesh.vertices, mesh.num_vertices, mesh.num_faces);
    }

    bool nanoRT_Intersect(Mesh& mesh, nanort::BVHAccel<double>& accel, nanoRT_Data& intersector);
}
namespace HF::RayTracer {

    class NanoRTRayTracer {

        using Intersection = nanort::TriangleIntersection<double>;
        using Intersector = nanort::TriangleIntersector<float, Intersection>;
        using NanoBVH = nanort::BVHAccel<float>;
        using NanoRay = nanort::Ray<float>;
    public:

        NanoRay Ray;

        // Add a hit object to be referenced
        Intersector intersector; ///< Triangle Intersector
        Intersection hit;
        NanoBVH bvh; ///< A NanoRT BVH 

        std::vector<float> vertices; //< Internal vertex array
        std::vector<unsigned int> indices; //< Internal index array

        // Add a distance attribute to store intersection distance
        float dist = -1;
        std::array<float, 3> point = { 0,0,0 };

        /*! \brief Construct a new raytracer with an instance of meshinfo*/
        inline NanoRTRayTracer(const HF::Geometry::MeshInfo& MI) : intersector(vertices.data(), indices.data(), sizeof(float) * 3)
        {
            // Get the index and vertex arrays of the meshinfo
            auto mi_indices = MI.GetIndexPointer().CopyArray();

            // Convert indices to unsigned integer because that's what nanoRT uses
            vertices = MI.GetVertexPointer().CopyArray();
            indices.resize(mi_indices.size());
            for (int i = 0; i < mi_indices.size(); i++)
                indices[i] = static_cast<unsigned int>(mi_indices[i]);

            // Build the BVH
            bvh = HF::nanoGeom::nanoRT_BVH<float>(indices.data(), vertices.data(), vertices.size()/3, indices.size()/3);
        }

        inline bool PointIntersection(
            std::array<float, 3>& origin,
            const std::array<float, 3>& dir,
            float distance = -1,
            int mesh_id = -1
        ) {
            throw std::logic_error("Not Implemented!");
        }


    };
}
