#include <iostream>
#include <utility>
#include <vector>
#include <algorithm>
#include <string>
#include <iomanip>
#include <cmath>
#include <cstdlib>
#include <map>
#include <fstream>

#include "graph.h"
#include "measure.h"

const double damping = 0.85;

class pagerank {
  graph& g;
  double threshold;
  bool cur;
  size_t round = 0;

private:
  void initialize() {
    cur = false;
    round = 0;
    for (auto n = g.begin(); n < g.end(); n++) {
      g.get_out_degree(n) = g.edge_end(n) - g.edge_begin(n);
      g.get_pr(n, cur) = 1.0 / g.size_nodes();
    }
  }

  void reset_next_round() {
    auto num_nodes = g.size_nodes();
    for (auto n = g.begin(); n < g.end(); n++) {
      g.get_pr(n, !cur) = (1.0 - damping) / (double)num_nodes;
    }
  }

  bool is_converged() {
    for (auto n = g.begin(); n < g.end(); n++) {
      const auto cur_pr = g.get_pr(n, cur);
      const auto next_pr = g.get_pr(n, !cur);
      if (fabs(next_pr - cur_pr) > threshold) {
        return false;
      }
    }  
    return true;
  }

  void swap_round() {
    cur = !cur;
  }

  void scale() {
    double sum = 0.0;
    for (auto n = g.begin(); n < g.end(); n++) {
      sum += g.get_pr(n, cur);
    }
    for (auto n = g.begin(); n < g.end(); n++) {
      g.get_pr(n, cur) /= sum;
    }
  }

public:
  pagerank(graph& g, double th): g(g), threshold(th) { }

  void compute_pull() {
    initialize();
    start_measurement();

    do {
      for (auto n = g.begin(); n < g.end(); n++) {
        auto& next_pr = g.get_pr(n, !cur);
        next_pr = (1.0 - damping) / (double)g.size_nodes();

        for (auto ie = g.in_edge_begin(n); ie < g.in_edge_end(n); ie++) {
          auto in_dst = g.get_in_edge_dst(ie);
          next_pr += damping * g.get_pr(in_dst, cur) / (double)g.get_out_degree(in_dst);
        }
      }

      swap_round();
      round += 1;
    } while(!is_converged());

    stop_measurement();
    scale();
    std::cout << "Converged using " << round << " round(s)." << std::endl;
  }

  void compute_push() {
    initialize();
    start_measurement();

    do {
      reset_next_round();

      for (auto n = g.begin(); n < g.end(); n++) {
        double my_contribution = damping * g.get_pr(n, cur) / (double)g.get_out_degree(n);
        for (auto e = g.edge_begin(n); e < g.edge_end(n); e++) {
          g.get_pr(g.get_edge_dst(e), !cur) += my_contribution;
        }
      }

      swap_round();
      round += 1;
    } while(!is_converged());

    stop_measurement();
    scale();
    std::cout << "Converged using " << round << " round(s)." << std::endl;
  }

  void print() {
    for (auto n = g.begin(); n < g.end(); n++) {
      std::cout << n << " ";
      std::cout << std::fixed << std::setprecision(6);
      std::cout << g.get_pr(n, cur) << std::endl;
    }
  }
};

void out_degree_initialize(graph& g) {
  // cur = false;
  // round = 0;
  for (auto n = g.begin(); n < g.end(); n++) {
    g.get_out_degree(n) = g.edge_end(n) - g.edge_begin(n);
  }
}

void in_degree_initialize(graph& g) {
  // cur = false;
  // round = 0;
  for (auto n = g.begin(); n < g.end(); n++) {
    g.get_out_degree(n) = g.in_edge_end(n) - g.in_edge_begin(n);
  }
}

void getHistogramOutDegree(graph& g) {
  out_degree_initialize(g);
  // FOR TESTING PURPOSES: OUTPUT FILE
  std::ofstream outDegMapFile;
  outDegMapFile.open("out_degree_map.txt");

  // Map.
  std::map<int, int> outMap;

  // Loop through graph nodes.
  for (auto n = g.begin(); n < g.end(); n++) {
    // Get out degree for each node.
    int outdeg = g.get_out_degree(n);
    if (outdeg != 0) {
      // Try and find this out degree in the map.
      std::map<int, int>::iterator it = outMap.find(outdeg);
      if (it != outMap.end()) {
        // Increment the value of this key.
        it->second += 1;

      } else {
        // Add to the map.
        outMap.emplace(std::make_pair(outdeg, 1));
      }
    }
  }

  // FOR TESTING PURPOSES: PRINT TO OUTPUT FILE
  outDegMapFile << "(out-degree)" << ", " << "(# of nodes)" << "\n";
  std::map<int, int>::iterator it = outMap.begin();
  while (it != outMap.end()) {
    outDegMapFile << it->first << "," << it->second << "\n";
    it++;
  }

  outDegMapFile.close();
}

void getHistogramInDegree(graph& g) {
  in_degree_initialize(g);
  // FOR TESTING PURPOSES: OUTPUT FILE
  std::ofstream inDegMapFile;
  inDegMapFile.open("in_degree_map.txt");
  // Map.
  std::map<int, int> inMap;

  // Loop through graph in-edges.
  for (auto n = g.begin(); n < g.end(); n++) {
    // Get out degree for each node.
    int indeg = g.get_out_degree(n);
    if (indeg != 0) {
      // Try and find this out degree in the map.
      std::map<int, int>::iterator it = inMap.find(indeg);
      if (it != inMap.end()) {
        // Increment the value of this key.
        it->second += 1;

      } else {
        // Add to the map.
        inMap.emplace(std::make_pair(indeg, 1));
      }
    }
  }

  // FOR TESTING PURPOSES: PRINT TO OUTPUT FILE
  inDegMapFile << "(in-degree)" << "," << "(# of nodes)" << "\n";
  std::map<int, int>::iterator it = inMap.begin();
  while (it != inMap.end()) {
    inDegMapFile << it->first << "," << it->second << "\n";
    it++;
  }

  inDegMapFile.close();
  
}

int main(int argc, char *argv[]) {
  if (argc < 4) {
    std::cerr << "Usage: " << argv[0] << " <input.dimacs> <pull|push> <threshold> [sorted]\n";
    return 0;
  }

  graph g;
  if (!g.construct_from_dimacs(argv[1])) {
    return 0;
  }

#if 0
  g.print_csr();
  g.print_csr_transposed();
  g.print_edges();
  g.print_in_edges();
#endif

  std::string algo = argv[2];
  if ("pull" != algo && "push" != algo) {
    std::cerr << "Unknown algorithm " << algo << std::endl;
    return 0;
  }

  pagerank pr(g, std::stod(argv[3]));
  if ("pull" == algo) {
    pr.compute_pull();
  } else if ("push" == algo) {
    pr.compute_push();
  }

  //getHistogramOutDegree(g);
  //getHistogramInDegree(g);

//  pr.print();
  return 0;
}
