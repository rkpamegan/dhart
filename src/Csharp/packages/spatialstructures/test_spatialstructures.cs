using Microsoft.VisualStudio.TestTools.UnitTesting;
using HumanFactors.SpatialStructures;
using System.Linq;
using System.Runtime.CompilerServices;
using HumanFactors.Exceptions;
using System.Collections.Generic;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System;
using System.Security.Cryptography;

namespace HumanFactors.Tests.SpatialStructures
{
    [TestClass]
    public class TestGraph
    {
        [TestMethod]
        public void CreateEmptyGraph()
        {
            Graph G = new Graph();
        }
        [TestMethod]
        public void AddEdgeFromID()
        {
            Graph G = new Graph();
            G.AddEdge(0, 1, 39);
            G.CompressToCSR();
            Assert.AreEqual(39, G.GetCost(0, 1),
                "Edge failed to be added through ids"
            );
        }
        [TestMethod]
        public void AddEdgeFromV3()
        {
            Graph G = new Graph();
            G.AddEdge(new Vector3D(0, 0, 2), new Vector3D(0, 0, 1), 39);

            G.CompressToCSR();
            Assert.AreEqual(39, G.GetCost(0, 1),
                "Edge failed to be added through vectors"
            );
        }

        [TestMethod]
        public void AddEdgeFromV3WithCosts()
        {

            // Create nodes
            Vector3D node0 = new Vector3D(0, 0, 1);
            Vector3D node1 = new Vector3D(0, 0, 2);

            // Create graph, compress, add edge
            Graph G = new Graph();
            G.AddEdge(node0, node1, 39);
            G.CompressToCSR();

            // Add an edge between the alternate costs
            G.AddEdge(node0, node1, 54, "AltCost");

            // get the ids for both nodes
            int id_0 = G.GetNodeID(node0);
            int id_1 = G.GetNodeID(node1);

            Assert.AreEqual(54, G.GetCost(id_0, id_1, "AltCost"),
                "Adding an edge with vector3Ds causes the alt cost to not save"
            );
        }

        [TestMethod]
        public void GetCSRPointers()
        {
            Graph G = new Graph();
            G.AddEdge(new Vector3D(0, 0, 2), new Vector3D(0, 0, 1), 39);
            var csr = G.CompressToCSR();

            // Assert that all pointers aren't null, and that the CSR
            // has the correct ammount of non-zeros, rows, and columns in it
            Assert.AreNotEqual(IntPtr.Zero, csr.inner_indices);
            Assert.AreNotEqual(IntPtr.Zero, csr.outer_indices);
            Assert.AreNotEqual(IntPtr.Zero, csr.data);
            Assert.AreEqual(1, csr.nnz);  // Should be 1 nnz since there is 1 edge
            Assert.AreEqual(3, csr.cols); // Should be 3 cols since there are 2 nodes an an empty column
            Assert.AreEqual(3, csr.rows); // Should be 3 rows isnce there are 2 nodes and an empty column
        }

        [TestMethod]
        public void GetCSRPointersForAltCost()
        {
            Graph G = new Graph();

            G.AddEdge(1, 2, 39);
            var default_ptrs = G.CompressToCSR();

            G.AddEdge(1, 2, 54, "ALT");
            var alt_ptrs = G.CompressToCSR("ALT");

            // Assert that all pointers aren't null, and that the CSR
            // has the correct ammount of non-zeros, rows, and columns in it
            Assert.AreEqual(default_ptrs.inner_indices, alt_ptrs.inner_indices);
            Assert.AreEqual(default_ptrs.outer_indices, alt_ptrs.outer_indices);
            Assert.AreNotEqual(default_ptrs.data, alt_ptrs.data, "Same value array returned for default and alt CSRs");
            Assert.AreEqual(1, alt_ptrs.nnz);  // Should be 1 nnz since there is 1 edge
            Assert.AreEqual(3, alt_ptrs.cols); // Should be 3 cols since there are 2 nodes an an empty column
            Assert.AreEqual(3, alt_ptrs.rows); // Should be 3 rows isnce there are 2 nodes and an empty column


            // Assert that NO_COST is thrown when csrptrs with an invalid cost
            try { var csr_bad = G.CompressToCSR("NotACalidCost"); }
            catch(KeyNotFoundException) { }
        }

        [TestMethod]
        public void GetNodes()
        {
            Graph G = new Graph();
            G.AddEdge(new Vector3D(0, 0, 2), new Vector3D(0, 0, 1), 39);

            var nodes = G.getNodes();
            Assert.IsTrue(nodes.size == 2);
        }
        [TestMethod]
        public void GetNodesCopy()
        {
            Graph G = new Graph();
            G.AddEdge(new Vector3D(0, 0, 2), new Vector3D(0, 0, 1), 39);

            var nodes = G.getNodes();
            var arr = nodes.CopyArray();
            Assert.IsTrue(arr.Count() == 2);
        }
        [TestMethod]
        public void AggregateNodes()
        {
            Graph G = new Graph();
            G.AddEdge(new Vector3D(0, 0, 2), new Vector3D(0, 0, 1), 39);

            var scores = G.AggregateEdgeCosts(GraphEdgeAggregation.SUM);
            var score_arr = scores.array;
            Assert.AreEqual(39, score_arr[0]);
            Assert.AreEqual(0, score_arr[1]);
        }

        [TestMethod]
        public void GetIDFromNode()
        {
            Graph g = new Graph();
            Vector3D node1 = new Vector3D(0, 0, 1);
            Vector3D node2 = new Vector3D(0, 0, 2);
            Vector3D node4 = new Vector3D(0, 0, 39);
            g.AddEdge(node1, node2, 10);

            Assert.IsTrue(g.GetNodeID(node1) >= 0);
            Assert.IsTrue(g.GetNodeID(node2) >= 0);
            Assert.IsTrue(g.GetNodeID(node4) < 0);
        }

        [TestMethod]
        public void GetEdgeCost()
        {
            Graph g = new Graph();

            g.AddEdge(0, 1, 100);

            // This must be throw a logicerror if uncompressed
            // And trying to read edges
            try { g.GetCost(0, 1); }
            catch (HumanFactors.Exceptions.LogicError) { }

            // Compress to the CSR 
            g.CompressToCSR();

            Assert.AreEqual(-1, g.GetCost(1, 2),
                "Edge that does not exist doesn't return -1 as a cost"
            );
            Assert.AreEqual(100, g.GetCost(0, 1),
                "Newly created edge for default cost type doesn't appear for getedge"
            );
        }

        [TestMethod]
        public void GetEdgeCostMulti()
        {
            Graph g = new Graph();
            string cost_type = "TestCost";

            // This should throw because the user is trying
            // to add a cost type to a graph that isn't compressed
            try { g.AddEdge(0, 1, 39, cost_type); }
            catch (LogicError) { }

            g.CompressToCSR();

            // This should throw an exception since the edge
            // doesn't already exist in the graph for the default cost_type.
            try { g.AddEdge(0, 1, 39, cost_type); }
            catch (LogicError) { }

            // Add the edge for the default cost type
            g.AddEdge(0, 1, 54);

            // Assert that this edge doesn't exist for this alternate cost type
            try { g.GetCost(1, 2, cost_type); }
            catch (KeyNotFoundException) { };


            // Add the edge for the alternate cost type
            g.AddEdge(0, 1, 100, cost_type);

            Assert.AreEqual(-1, g.GetCost(0, 2, cost_type),
                "A cost that doesn't exist for this type returns something other than -1."
           );

            // Assert that the default graph is still readable
            Assert.AreEqual(54, g.GetCost(0, 1),
                "Adding another cost modified the cost in the default graph.");

            // Assert that the non-default cost is still readable
            Assert.AreEqual(100, g.GetCost(0, 1, cost_type), "The alternate cost cannot be read");
        }


        [TestMethod]
        unsafe public void CalculateAndStoreCrossSlope()
        {
            // Create the graph
            Graph g = new Graph();

            // Create 7 nodes
            Vector3D n0 = new Vector3D(2, 6, 6);
            Vector3D n1 = new Vector3D(0, 0, 0);
            Vector3D n2 = new Vector3D(-5, 5, 4);
            Vector3D n3 = new Vector3D(-1, 1, 1);
            Vector3D n4 = new Vector3D(2, 2, 2);
            Vector3D n5 = new Vector3D(5, 3, 2);
            Vector3D n6 = new Vector3D(-2, -5, 1);

            // Add 9 edges
            g.AddEdge(n0, n1, 0); // [ -2, -6, -6 ]
            g.AddEdge(n1, n2, 0); // [ -5,  5,  4 ]
            g.AddEdge(n1, n3, 0); // [ -1,  1,  1 ]
            g.AddEdge(n1, n4, 0); // [  2,  2,  2 ]
            g.AddEdge(n2, n4, 0); // [ -9, -3, -2 ]
            g.AddEdge(n3, n5, 0); // [ -6,  2,  1 ]
            g.AddEdge(n5, n6, 0); // [ -7, -8, -1 ]
            g.AddEdge(n4, n6, 0); // [ -6, -7, -1 ]

            // Compress the graph after adding edges
            // Always compress the graph after adding edges!
            var def_csr = g.CompressToCSR();
            // Calculate and store edge type in g: cross slope
            CostAlgorithms.CalculateAndStoreCrossSlope(g);
            // Assert that the graph has new edges in it.
            try
            {
                CSRInfo csr = g.CompressToCSR(CostAlgorithmNames.CROSS_SLOPE);

                Assert.AreEqual(def_csr.inner_indices, csr.inner_indices);
                Assert.AreEqual(def_csr.outer_indices, csr.outer_indices);
                Assert.AreNotEqual(def_csr.data, csr.data);

                Span<float> arr = new Span<float>(csr.data.ToPointer(), csr.nnz);

                for (int i = 0; i < arr.Length; i++)
                    Debug.Write(arr[i].ToString() + ", ");

                Debug.WriteLine("");
            }
            catch (KeyNotFoundException)
            {
                Assert.Fail("Key wasn't created in the dictionary!");
            }
        }

        [TestMethod]
        unsafe public void  CalculateAndStoreEnergyExpenditure()
        {
            // Create the graph
            Graph g = new Graph();

            // Create 7 nodes
            Vector3D n0 = new Vector3D(0, 0, 0);
            Vector3D n1 = new Vector3D(0, 0, 1);
            Vector3D n2 = new Vector3D(5, 5, 4);
            Vector3D n3 = new Vector3D(2, 2, 2);
            Vector3D n4 = new Vector3D(5, 3, 2);
            Vector3D n5 = new Vector3D(6, 6, 7);
            Vector3D n6 = new Vector3D(2, 5, 1);

            // Add 9 edges
            g.AddEdge(n0, n1, 1);
            g.AddEdge(n1, n2, 1);
            g.AddEdge(n1, n3, 1);
            g.AddEdge(n1, n4, 1);
            g.AddEdge(n3, n5, 1);
            g.AddEdge(n4, n2, 1);
            g.AddEdge(n6, n4, 1);
            g.AddEdge(n6, n5, 1);

            // Always compress the graph after adding edges!
            var def_csr = g.CompressToCSR();

            // Calculate and store edge type in g: energy expenditure
            CostAlgorithms.CalculateAndStoreEnergyExpenditure(g);

            // Assert that the graph has new edges in it.
            try
            {
                CSRInfo csr = g.CompressToCSR(CostAlgorithmNames.ENERGY_EXPENDITURE);

                Assert.AreEqual(def_csr.inner_indices, csr.inner_indices);
                Assert.AreEqual(def_csr.outer_indices, csr.outer_indices);
                Assert.AreNotEqual(def_csr.data, csr.data);


                Span<float> arr = new Span<float>(csr.data.ToPointer(), csr.nnz);

                for (int i = 0; i < arr.Length; i++)
                    Debug.Write(arr[i].ToString() + ", ");

                Debug.WriteLine("");
            }
            catch (KeyNotFoundException)
            {
                Assert.Fail("Key wasn't created in the dictionary!");
            }
        }
        [TestMethod]
        public void AddNodeAttribute()
        {
            // Create a graph and add two edges to create nodes
            Graph g = new Graph();
            g.AddEdge(0, 1, 150);
            g.AddEdge(0, 2, 100);
            g.AddEdge(0, 3, 2);

            // Add node attributes to the graph for the nodes
            // we just created
            g.AddNodeAttribute(2, "Attr", "200");
            g.AddNodeAttribute(1, "Attr", "100");
            g.AddNodeAttribute(0, "Attr", "0");

            // Get scores for this attribute from the graph
            var attr = g.GetNodeAttributes("Attr");
            
            // Assert that the scores meet our expectations
            Assert.AreEqual(4, attr.Length);
            Assert.AreEqual(attr[0], "0");
            Assert.AreEqual(attr[1], "100");
            Assert.AreEqual(attr[2], "200");

            // And that this is the empty string
            Assert.AreEqual(attr[3], "");
        }

        [TestMethod]
        public void AddMultipleNodeAttributes()
        {

            // Create a graph and add two edges to create nodes
            Graph g = new Graph();
            g.AddEdge(0, 1, 150);
            g.AddEdge(0, 2, 100);

            // Create arrays for ids and scores
            int[] ids = { 0, 1, 2 };
            string[] scores = { "0", "100", "200" };

            // Add them to the graph
            g.AddNodeAttribute("Attr", ids, scores);

            // Get scores for this attribute from the graph
            var attr = g.GetNodeAttributes("Attr");

            // Assert that the scores meet our expectations
            Assert.AreEqual(3, attr.Length);
            Assert.AreEqual(attr[1], "100");
        }

        [TestMethod]
        public void ClearAttr()
        {

            // Create a graph and add two edges to create nodes
            Graph g = new Graph();
            g.AddEdge(0, 1, 150);
            g.AddEdge(0, 2, 100);

            // Create arrays for ids and scores
            int[] ids = { 0, 1, 2 };
            string[] scores = { "0", "100", "200" };

            // Add them to the graph
            g.AddNodeAttribute("Attr", ids, scores);

            // Get scores for this attribute from the graph
            var attr = g.GetNodeAttributes("Attr");

            // Now try to delete
            g.ClearNodeAttributes("Attr");

            // check that this is truly gone
            var node_attrs = g.GetNodeAttributes("Attr");
            foreach (var node_attr in node_attrs)
                Assert.AreEqual("", node_attr, "Clear didn't clear all node attributes");
        }

        [TestMethod]
        public void GetNodeAttributes()
        {

        }

        public void ClearNodeAttributes()
        {

        }
    }
}
 