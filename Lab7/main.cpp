#include <iostream>
#include <vector>
#include <unordered_set>
#include <functional>
#include <list>

using graph_t = std::vector<std::vector<int64_t>>;

bool have_cycles(const graph_t& graph, const std::unordered_set<int64_t>& starts) {
    std::vector<int8_t> visited(graph.size(), 0);
    bool result = false;
    std::function<void(int64_t)> dfs = 
        [&result, &visited, &graph, &dfs] (int64_t cur) {
            if (result) {
                return;
            }
            if (visited[cur] == 1) {
                result = true;
                return;
            }
            visited[cur] = 1;
            for (const auto& next : graph[cur]) {
                dfs(next);
            }
            visited[cur] = 2;
            return;
        };
    for (const auto& start : starts) {
        dfs(start);
    }
    return result;
}

int main() {
    int64_t n, m;
    std::cin >> n >> m;
    graph_t g(n);

    // указываем, какая вершина может быть началом графа
    std::unordered_set<int64_t> starts;
    for (int64_t i = 0; i < n; ++i) starts.insert(i);

    for (int64_t i = 0; i < m; ++i) {
        int64_t a, b;
        std::cin >> a >> b;
        --a; --b;
        starts.erase(b);
        g[a].push_back(b);
    }

    if (starts.empty()) {
        std::cout << -1 << std::endl;
        return 0;
    }
    if (have_cycles(g, starts)) {
        std::cout << -1 << std::endl;
        return 0;
    }
    
    std::list<int64_t> seq;
    std::vector<char> visited(n, false);
    std::function<void(int64_t)> dfs = 
        [&seq, &g, &visited, &dfs] (int64_t cur) {
            if (visited[cur]) {
                return;
            }
            visited[cur] = true;

            for (const auto& next : g[cur]) {
                dfs(next);
            }
            seq.push_front(cur);
        };
    
    for (const auto& start : starts) {
        dfs(start);
    }

    for (auto itr = seq.begin(); itr != std::prev(seq.end()); ++itr) {
        std::cout << *itr + 1 << ' ' << *next(itr) + 1 << '\n';
    }
}