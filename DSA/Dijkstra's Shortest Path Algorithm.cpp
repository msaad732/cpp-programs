#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <limits>
#include <string>
#include <tuple>
#include <algorithm>

using namespace std;

// Define an alias for the graph structure: 
// map<string, map<string, int>> -> Node (string) maps to Neighbors (map<string, int>)
// where the inner map holds Neighbor Node (string) and Edge Weight (int).
using Graph = map<string, map<string, int>>;

// Define a type for the priority queue element: (distance, node)
// Note: We negate the distance to simulate a Min-Heap with std::priority_queue, 
// which is a Max-Heap by default.
using State = pair<int, string>; 

// Define aliases for the results
using Distances = map<string, int>;
using Predecessors = map<string, string>;

/**
 * @brief Computes the shortest path from the start_node to every other reachable node 
 * in the graph using Dijkstra's algorithm.
 * * @param graph The graph represented as an adjacency list.
 * @param start_node The node to start the path calculation from.
 * @return A tuple containing the shortest distances and predecessor maps.
 */
tuple<Distances, Predecessors> dijkstra_shortest_path(const Graph& graph, const string& start_node) {
    // Initialize distances: All nodes start with infinity distance, except the start node (0).
    Distances distances;
    for (const auto& pair : graph) {
        distances[pair.first] = numeric_limits<int>::max();
    }
    distances[start_node] = 0;

    // Predecessors dictionary: Used to reconstruct the shortest path later.
    Predecessors predecessors;

    // Priority Queue (Max-Heap by default): Stores (distance, node) pairs.
    // We use std::greater<State> to make it behave as a Min-Heap based on distance.
    priority_queue<State, vector<State>, greater<State>> pq;
    
    // Push the starting node: (distance 0, start_node)
    pq.push({0, start_node});

    // Main Loop: Continues until all reachable nodes have been processed
    while (!pq.empty()) {
        // Extract the node with the minimum distance (current_distance, current_node)
        // Since it's a Min-Heap (due to std::greater), top() gives the smallest element.
        int current_distance = pq.top().first;
        string current_node = pq.top().second;
        pq.pop();

        // Check for stale entries in the priority queue (paths found earlier but longer)
        if (current_distance > distances[current_node]) {
            continue;
        }

        // Relaxation Step: Explore neighbors
        // Check if current_node exists in the graph (safety check)
        if (graph.find(current_node) != graph.end()) {
            for (const auto& neighbor_pair : graph.at(current_node)) {
                string neighbor = neighbor_pair.first;
                int weight = neighbor_pair.second;

                // Calculate the distance to the neighbor through the current node
                int new_distance = current_distance + weight;

                // Relaxation: If this new path is shorter than the currently recorded distance
                if (new_distance < distances[neighbor]) {
                    // Update the distance
                    distances[neighbor] = new_distance;
                    // Record the predecessor (the node we came from)
                    predecessors[neighbor] = current_node;
                    // Push the updated distance to the priority queue
                    pq.push({new_distance, neighbor});
                }
            }
        }
    }

    return make_tuple(distances, predecessors);
}

/**
 * @brief Reconstructs the shortest path from the start_node to the target_node 
 * using the predecessors map.
 */
vector<string> reconstruct_path(const Predecessors& predecessors, const string& start_node, const string& target_node) {
    vector<string> path;
    string current_node = target_node;
    
    // Walk backward from the target to the start using predecessors
    while (current_node != start_node) {
        if (predecessors.find(current_node) == predecessors.end()) {
            // This happens if the target is unreachable
            return {};
        }
        path.push_back(current_node);
        current_node = predecessors.at(current_node);
    }
    
    path.push_back(start_node);
    
    // The path was built backward, so reverse it
    reverse(path.begin(), path.end());
    return path;
}

// --- Example Usage ---

int main() {
    // Define a sample weighted, directed graph (Adjacency List)
    Graph sample_graph = {
        {"A", {{"B", 7}, {"C", 9}, {"F", 14}}},
        {"B", {{"C", 10}, {"D", 15}}},
        {"C", {{"D", 11}, {"F", 2}}},
        {"D", {{"E", 6}}},
        {"E", {{"F", 8}}}, // Corrected edge for demonstration
        {"F", {{"E", 9}}}
    };
    
    string START = "A";
    string TARGET = "D";

    cout << "--- Running Dijkstra's Algorithm from Node " << START << " ---" << endl;
    
    // Run the algorithm
    auto results = dijkstra_shortest_path(sample_graph, START);
    Distances shortest_distances = get<0>(results);
    Predecessors path_predecessors = get<1>(results);

    // 1. Print all shortest distances
    cout << "\nShortest Distances from " << START << ":" << endl;
    for (const auto& pair : shortest_distances) {
        cout << "  Node " << pair.first << ": ";
        if (pair.second == numeric_limits<int>::max()) {
            cout << "Unreachable" << endl;
        } else {
            cout << pair.second << endl;
        }
    }
        
    // 2. Reconstruct and print a specific path
    vector<string> shortest_path = reconstruct_path(path_predecessors, START, TARGET);
    
    if (!shortest_path.empty()) {
        int distance_to_target = shortest_distances.at(TARGET);
        cout << "\nShortest Path to " << TARGET << " (Distance: " << distance_to_target << "):" << endl;
        
        for (size_t i = 0; i < shortest_path.size(); ++i) {
            cout << shortest_path[i];
            if (i < shortest_path.size() - 1) {
                cout << " -> ";
            }
        }
        cout << endl;
    } else {
        cout << "\nNode " << TARGET << " is unreachable from " << START << "." << endl;
    }

    return 0;
}
