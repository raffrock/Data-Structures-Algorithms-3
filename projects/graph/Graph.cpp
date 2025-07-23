#include "Graph.hpp"

/**
 * Prints graph for debugging purposes.
 */
void VertexCover::displayGraphHidden(Graph g) {
    
    for (auto n : g) {
        std::cout << "\n" << n.first << ": {";
        for (Vertex v : n.second) {
            std::cout << " " << v;
        }
        std::cout << " }";
    }
    std::cout << "\n";
}


/**
 * @brief Helper function to return three letter airport code from segemented string.
 */
std::string find_location(std::string input) {
    auto start = input.find("- ");
    input = input.substr(start+2);
    auto end = input.find(" ");
    if (end != std::string::npos) {
        input = input.substr(0,end);
    }
    return input;
}

/**
* @brief Reads the contents of a flight table
* as specified by the filename, into an undirected
* Graph.
*
* @param filename (a const. string reference) The filename of the file to be read
* @return (Graph) The resultant Graph object described by the file's contents.
*
* @throws (std::runtime_error) If the file cannot be opened for some reason (eg. using fin.fai
*/
Graph VertexCover::readFromFile(const std::string& filename) {
    // stores all edges
    Graph unsorted_graph;

    // read in file
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file.");
    }

    // goes through file, line by line
    // ex. Departure (15: 43) - SRB | Destination  (03: 36) - WIE
    std::string line;
    while (std::getline(file, line)) {
        int seperater = line.find("|");

        if (seperater == std::string::npos) {
            break;
        }

        Vertex departure = find_location(line.substr(0,seperater)); 
        Vertex destination = find_location(line.substr(seperater+1));
        
        // check if in graph then add
        if (unsorted_graph.find(departure) != unsorted_graph.end()) {
            unsorted_graph.at(departure).insert(destination);
        } else {
            unsorted_graph.insert({departure,{destination}});
        }

        std::swap(departure,destination);

        if (unsorted_graph.find(departure) != unsorted_graph.end()) {
            unsorted_graph.at(departure).insert(destination);
        } else {
            unsorted_graph.insert({departure,{destination}});
        }

    }
    file.close();

    return unsorted_graph;
}

/**
 * @brief Generates a sub-optimal minimumum vertex cover
 * by repeatedly choosing the largest degree vertex & 
 * adding it to the cover set.
 *
 * @param g (Graph) The graph object 
 * for which to generate a vertex cover. 
 * NOTE: This is NOT a const. reference
 *
 * EXAMPLE: with all adjacent edges, 
 * from the original graph), and continue until we've generated a
 * vertex cover for the Graph.
 * That's it!
    {
    "JFK" : { "LGA", "KIX" },
    "LGA" : { "JFK" },
    "KIX" : { "JFK" }
    }
 * 
 * @return (std::unordered_set<Vertex>) The set of vertices 
 * that forms a vertex cover of the graph.
 */
 std::unordered_set<Vertex> VertexCover::cover_graph(Graph g) {
    Neighbors cover_set;

    while ((g.size() > 1)) {
        // find pair with largest number of nieghbors
        auto most_edges = std::max_element(g.begin(),g.end(),[] (auto v1, auto v2) { return v1.second.size() < v2.second.size(); } );
        // if nothing returned
        if (most_edges == g.end()) {
            break;
        }

        Vertex next_vertex = (*most_edges).first;
        Neighbors next_neighbors = (*most_edges).second;

        // add edge
        cover_set.insert(next_vertex);
        // erase edge
        g.erase(next_vertex);

        // erase all adj edges
        for (Vertex n : next_neighbors) {
            // if found
            auto itr = g.find(n);
            if (itr != g.end()) {
                // update Neighbors
                Neighbors& n_set = (*itr).second;
                n_set.erase(next_vertex);
                // remove if empty, update if not
                if (n_set.size() == 0) {
                    g.erase(n);
                }
            }
        }
    }

    return cover_set;
}