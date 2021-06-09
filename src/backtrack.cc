/**
 * @file backtrack.cc
 *
 */

#include "backtrack.h"
using namespace std;

Backtrack::Backtrack() {}
Backtrack::~Backtrack() {}

// function prototypes
Vertex getRoot(const Graph &query, const Graph &data);
pair<Vertex, vector<vector<Vertex>>> buildDAG(const Graph &query, const Vertex &rootVertex);
map<Vertex, set<Vertex>> buildInverseDAG(const Graph &query, const vector<vector<Vertex>> &adj_list);
map<size_t, bool> initializeCSVisited(const Graph &query, const CandidateSet &cs);
map<size_t, bool> initializeUVisited(const Graph &query);
void backtrack(
  const Graph &data,
  const Graph &query,
  const pair<Vertex, vector<vector<Vertex>>> &rootedDAG,
  const map<Vertex, set<Vertex>> &inverseRootedDAG,
  const CandidateSet &cs,
  map<size_t, bool> csVisited,
  map<size_t, bool> uVisited,
  vector<pair<Vertex, Vertex>> m
);
set<Vertex> getExtendableVertices(
  const Graph &query,
  const map<Vertex, set<Vertex>> &inverseRootedDAG,
  map<size_t, bool> uVisited
);
void printEmbedding(vector<pair<Vertex, Vertex>> &embedding);
void printFullEmbedding(vector<pair<Vertex, Vertex>> &embedding);

// Implementation
void Backtrack::PrintAllMatches(const Graph &data, const Graph &query, const CandidateSet &cs) {
  cout << "t " << query.GetNumVertices() << "\n";
  // Part 1 -> Build DAG from q and G
  const Vertex rootVertex = getRoot(query, data);
  const pair<Vertex, vector<vector<Vertex>>> rootedDAG = buildDAG(query, rootVertex);
  const map<Vertex, set<Vertex>> inverseRootedDAG = buildInverseDAG(query, rootedDAG.second);
  // Part 2 -> Build refined CS (given)
  // Part 3 -> Initialize empty partial embedding
  vector<pair<Vertex, Vertex>> partialEmbedding;
  // Part 4 -> Backtrack to find embedding
  map<size_t, bool> csVisited = initializeCSVisited(query, cs);
  map<size_t, bool> uVisited = initializeUVisited(query);
  backtrack(data, query, rootedDAG, inverseRootedDAG, cs, csVisited, uVisited, partialEmbedding);
}

// Function to get root of query graph
Vertex getRoot(const Graph &query, const Graph &data) {
  const int numVertices = query.GetNumVertices();
  Vertex currentRootVertex = -1;
  double currentMin = -1;
  for (Vertex currentVertex = 0; currentVertex < numVertices; currentVertex++) {
    const Label queryLebel = query.GetLabel(currentVertex);
    const size_t degree = query.GetDegree(currentVertex);
    size_t initialCandidateSet = 0;
    for (int i = 0; i < data.GetNumVertices(); i++) {
      if (data.GetLabel(i) == queryLebel && data.GetDegree(i) >= degree) {
        initialCandidateSet++;
      }
    }
    const double currentResult = initialCandidateSet / (double) degree;
    if (currentMin == -1 || currentResult < currentMin) {
      currentRootVertex = currentVertex;
      currentMin = currentResult;
    }
  }
  return currentRootVertex;
}

// Function to build DAG q_d from query graph through BFS
pair<Vertex, vector<vector<Vertex>>> buildDAG(const Graph &query, const Vertex &rootVertex) {
  size_t vertexSize = query.GetNumVertices();
  bool visited [vertexSize];
  for (int i = 0; i < vertexSize; i++) {
    visited[i] = false;
  }

  vector<vector<Vertex>> adj_list;
  adj_list.resize(vertexSize);

  queue<Vertex> queue;
  queue.push(rootVertex);

  while(!queue.empty()) {
    Vertex curVertex = queue.front();
    queue.pop();
    size_t startOffset = query.GetNeighborStartOffset(curVertex);
    size_t endOffset = query.GetNeighborEndOffset(curVertex);
    for (size_t currentOffset = startOffset; currentOffset < endOffset; currentOffset++) {
      Vertex currentNeighbor = query.GetNeighbor(currentOffset);
      if (!visited[currentNeighbor] && !visited[curVertex]) {
        adj_list[curVertex].push_back(currentNeighbor);
        queue.push(currentNeighbor);
      }
    }
    visited[curVertex] = true;
  }

  return make_pair(rootVertex, adj_list);
}

// Function to build Inverse DAG q_d. This function is used to find parent of vertex
map<Vertex, set<Vertex>> buildInverseDAG(const Graph &query, const vector<vector<Vertex>> &adj_list) {
  size_t vertexSize = query.GetNumVertices();
  map<Vertex, set<Vertex>> inverse_adj_list;

  for (int i = 0; i < vertexSize; i++) {
    set<Vertex> emptySet;
    inverse_adj_list.insert({i, emptySet});
  }

  for (int i = 0; i < adj_list.size(); i++) {
    vector<Vertex> edge = adj_list[i];
    for (int j = 0; j < edge.size(); j++) {
      inverse_adj_list[edge[j]].insert(i);
    }
  }

  return inverse_adj_list;
}

// Function to initialize CSVisited. This DS will be used to store whether vertex of G has been visited.
map<size_t, bool> initializeCSVisited(const Graph &query, const CandidateSet &cs) {
  const size_t vertexSize = query.GetNumVertices();
  map<size_t, bool> csVisited; // vector of visited per query vertex
  for (int i = 0; i < vertexSize; i++) {
    const size_t currentCandidateSize = cs.GetCandidateSize(i);
    for (int j = 0; j < currentCandidateSize; j++) {
      Vertex candidate = cs.GetCandidate(i, j);
      csVisited.insert({candidate, false});
    }
  }
  return csVisited;
}

// Function to initialize CSVisited. This DS will be used to store whether vertex of q has been visited.
map<size_t, bool> initializeUVisited(const Graph &query) {
  map<size_t, bool> uVisited;
  const size_t vertexSize = query.GetNumVertices();
  for (int i = 0; i < vertexSize; i++) {
    uVisited.insert({i, false});
  }
  return uVisited;
}

// Recursive function to backtrack to find embedding
size_t ANSWER_SIZE = 0;
void backtrack(
  const Graph &data,
  const Graph &query,
  const pair<Vertex, vector<vector<Vertex>>> &rootedDAG,
  const map<Vertex, set<Vertex>> &inverseRootedDAG,
  const CandidateSet &cs,
  map<size_t, bool> csVisited,
  map<size_t, bool> uVisited,
  vector<pair<Vertex, Vertex>> m
) {
  const size_t queryVectorSize = query.GetNumVertices();
  const size_t mSize = m.size();

  // Case when size of partial embedding equals query size
  if (mSize == queryVectorSize) {
    if (ANSWER_SIZE == 100000) {
      exit(EXIT_SUCCESS);
    }
    printEmbedding(m);
    ANSWER_SIZE++;

  // Case when size of partial embedding equals zero (Initialization)
  } else if (mSize == 0) {
    const Vertex root = rootedDAG.first;
    const size_t rootCandidateSize = cs.GetCandidateSize(root);
    for (int candidateIndex = 0; candidateIndex < rootCandidateSize; candidateIndex++) {
      const Vertex candidate = cs.GetCandidate(root, candidateIndex);
      // Add (r, v) to partial embedding m
      m.push_back(make_pair(root, candidate));
      // Mark r and v as visited
      csVisited[candidate] = true;
      uVisited[root] = true;
      // recursive backtrack with updated partial embedding
      backtrack(data, query, rootedDAG, inverseRootedDAG, cs, csVisited, uVisited, m);
      // Mark r and v as unvisited for next candidate
      m.pop_back();
      csVisited[candidate] = false;
      uVisited[root] = false;
    }

  // All other case in process of recurive backtrack
  } else {
    const vector<vector<Vertex>> adj_list = rootedDAG.second;
    set<Vertex> extendableVertexes = getExtendableVertices(query, inverseRootedDAG, uVisited);
    // Get extendable vertex using candidate size order for each vertex get possile candidate.
    // We selected extendable vertex to be vertex with minimum number of candidates.
    Vertex extendableVertex;
    size_t minimumCandidateSize = -1;
    for (Vertex currentExtendableVertex : extendableVertexes) {
      const size_t candidateSize = cs.GetCandidateSize(currentExtendableVertex);
      if (minimumCandidateSize == -1 || candidateSize < minimumCandidateSize) {
        extendableVertex = currentExtendableVertex;
        minimumCandidateSize = candidateSize;
      }
    }

    const set<Vertex> parents = inverseRootedDAG.at(extendableVertex);
    const size_t currentCandidateSize = cs.GetCandidateSize(extendableVertex);
    // Consider each candidate of extendable vertex as option
    for (int candidateIndex = 0; candidateIndex < currentCandidateSize; candidateIndex++) {
      const Vertex candidate = cs.GetCandidate(extendableVertex, candidateIndex);
      bool connectedToParent = true;
      // Check whether all parents of candidate are conencted
      for (Vertex parent : parents) {
        for (pair<Vertex, Vertex> mapping : m) {
          if (mapping.first == parent && !data.IsNeighbor(candidate, mapping.second)) {
            connectedToParent = false;
          }
        }
      }
      if (!csVisited[candidate] && connectedToParent) {
        // Add (u, v) to partial embedding m
        m.push_back(make_pair(extendableVertex, candidate));
        // Mark u and v as visited
        csVisited[candidate] = true;
        uVisited[extendableVertex] = true;
        // recursive backtrack with updated partial embedding
        backtrack(data, query, rootedDAG, inverseRootedDAG, cs, csVisited, uVisited, m);
        // Mark r and v as unvisited for next candidate
        m.pop_back();
        csVisited[candidate] = false;
        uVisited[extendableVertex] = false;
      }
    }
  }
  return;
}

// Function to get extendable verticies based on current visited
set<Vertex> getExtendableVertices(
  const Graph &query,
  const map<Vertex, set<Vertex>> &inverseRootedDAG,
  map<size_t, bool> uVisited
) {
  size_t numVertices = query.GetNumVertices();
  set<Vertex> extendableVertices;
  for (int i = 0; i < numVertices; i++) {
    if (!uVisited[i]) {
      set<Vertex> parents = inverseRootedDAG.at(i);
      bool allParentsMatched = true;
      for (Vertex parent : parents) {
        if (!uVisited[parent]) {
          allParentsMatched = false;
        }
      }
      if (allParentsMatched) {
        extendableVertices.insert(i);
      }
    }
  }
  return extendableVertices;
};

// Function used to print output
void printEmbedding(vector<pair<Vertex, Vertex>> &embedding) {
  sort(embedding.begin(), embedding.end());
  cout << "a";
  for (pair<Vertex, Vertex> vertexMapping : embedding) {
    const Vertex graphVertex = vertexMapping.second;
    cout << " " << graphVertex;
  }
  cout << "\n";
}

// Helper function used to display embedding pairs
void printFullEmbedding(vector<pair<Vertex, Vertex>> &embedding) {
  cout << "{";
  for (pair<Vertex, Vertex> vertexMapping : embedding) {
    const Vertex queryVertex = vertexMapping.first;
    const Vertex graphVertex = vertexMapping.second;
    cout << " (" << queryVertex << ", " << graphVertex << ") ";
  }
  cout << " }\n";
}
