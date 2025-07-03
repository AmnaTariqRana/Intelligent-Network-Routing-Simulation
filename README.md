# Intelligent-Network-Routing-Simulation
A C++ project that simulates intelligent message routing in a dynamic network using Dijkstra’s algorithm and splay trees. It demonstrates how graph algorithms and adaptive data structures can optimize real-world communication systems.

🚀 Overview
The project models a network as a graph, where each node acts as a router capable of sending and receiving messages. Using Dijkstra’s algorithm for shortest path calculation and splay trees for dynamic routing table updates, the system efficiently determines optimal message routes while adapting over time.

It aims to mirror real-world scenarios where routing decisions depend on efficiency, network cost, and dynamic traffic patterns.

🔑 Key Features
Graph-Based Network Architecture
Routers are represented as graph nodes with weighted edges acting as communication links that simulate bandwidth or latency costs.

Dijkstra’s Algorithm for Pathfinding
Messages are routed along the shortest available path using Dijkstra's algorithm, ensuring cost-effective communication.

Splay Trees for Routing Optimization
Frequently accessed routes are prioritized using splay trees, enabling the routing tables to self-optimize for faster lookups over time.

Realistic Message Routing Simulation
The simulation generates and transmits messages between random nodes, tracking performance metrics like hop count, total time, and route taken.

Extensive Data Logging
Every step of routing is logged for later analysis, helping examine algorithm performance and tree behavior under different scenarios.

🧰 Technologies Used
Language: C++

Data Structures: Graphs, Splay Trees, Priority Queues

Algorithms: Dijkstra's Algorithm

🎯 Learning Objectives
Understand how data structures support efficient message routing

Apply shortest path algorithms in simulated network scenarios

Implement self-adjusting trees for adaptive data handling

🔄 Potential Extensions
Supporting real-time topology changes (adding/removing routers)

Modeling network congestion or limited bandwidth

Comparing with other routing algorithms (like Bellman-Ford or A*)

Adding graphical visualization of routing paths and traffic
