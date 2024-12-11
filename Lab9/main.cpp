#include <iostream>
#include <limits>
#include <queue>
#include <unordered_map>
#include <vector>
#define fast_io                       \
    std::ios::sync_with_stdio(false); \
    std::cin.tie(0);                  \
    std::cout.tie(0)

constexpr int64_t INF = std::numeric_limits<int64_t>::max();

std::vector<int64_t> dijkstra(
    const std::vector<std::vector<std::pair<int, int64_t>>>& graph, int start) {
    std::vector<int64_t> result(graph.size(), INF);

    std::priority_queue < std::pair<int64_t, int>,
        std::vector<std::pair<int64_t, int>>,
        std::greater<std::pair<int64_t, int>>> q;  // {len, vertex}
    result[start] = 0;
    q.push({0, start});
    while (!q.empty()) {
        auto [len, cur] = q.top();
        q.pop();
        if (len > result[cur]) {
            continue;
        }

        for (auto& [next, w] : graph[cur]) {
            if (result[cur] + w < result[next]) {
                result[next] = result[cur] + w;
                q.push({result[cur] + w, next});
            }
        }
    }
    return result;
}

int main() {
    fast_io;

    int n, m, start, finish;
    std::cin >> n >> m >> start >> finish;
    --start;
    --finish;
    std::vector<std::vector<std::pair<int, int64_t>>> graph(n);

    for (int i = 0; i < m; ++i) {
        int u, v, w;
        std::cin >> u >> v >> w;
        --u;
        --v;
        graph[u].push_back({v, w});
        graph[v].push_back({u, w});
    }

    auto lengths = dijkstra(graph, start);

    if (lengths[finish] == INF) {
        std::cout << "No solution\n";
    } else {
        std::cout << lengths[finish] << std::endl;
    }
}