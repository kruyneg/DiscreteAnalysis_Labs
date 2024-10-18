#include <iostream>
#include <vector>
#include <list>

// using cost_data = std::pair<int64_t, int64_t>; // cost_sum | amount
using cost_data = int64_t;

// inline std::ostream& operator<<(std::ostream& out, const cost_data& data) {
//     return out << data.first << ',' << data.second;
// }

void print(const std::vector<std::vector<cost_data>>& matrix) {
    std::cout << '\t';
    for (int i = 0; i < matrix.size(); ++i){
        std::cout << i << '\t';
    }
    std::cout << '\n';
    std::cout << std::string(matrix.size() * 8 + 8, '-') << '\n';

    for (int j = 0; j < matrix.back().size(); ++j) {
        std::cout << j << '|' << '\t';
        for (int i = 0; i < matrix.size(); ++i) {
            std::cout << matrix[i][j] << '\t';
        }
        std::cout << "\n";
    }
}

int main() {
    int64_t n, m;
    std::cin >> n >> m;
    std::vector<std::pair<int64_t, int64_t>> objects(n);
    for (auto& [w, c] : objects) {
        std::cin >> w >> c;
    }

    std::vector< std::vector<std::vector<cost_data>> >
        dp(
            n + 1,
            std::vector<std::vector<cost_data>>(
                n + 1,
                std::vector<cost_data>(m + 1, 0)
            )
        );

    for (size_t k = 1; k <= n; ++k) {
        const auto& dp_prev = dp[k - 1];
        auto& dp_cur = dp[k];
        for (size_t i = 1; i <= n; ++i) {
            auto [weight, cost] = objects[i - 1];
            for (size_t w = 0; w <= m; ++w) {
                if (w < weight) {
                    dp_cur[i][w] = dp_cur[i - 1][w];
                } else {
                    int64_t new_cost = 0;
                    if (k == 1) {
                        new_cost = cost;
                    } else {
                        if (dp_prev[i - 1][w - weight] != 0)
                            new_cost = (dp_prev[i - 1][w - weight] / (k - 1) + cost) * k;
                    }
                    dp_cur[i][w] = std::max(dp_cur[i - 1][w], new_cost);

                    // if ((dp[i - 1][w - weight].first + cost) * (dp[i - 1][w - weight].second + 1) >=
                    //         (dp[i - 1][w].first * dp[i - 1][w].second)
                    // ) {
                    //     dp[i][w] = {dp[i - 1][w - weight].first + cost, dp[i - 1][w - weight].second + 1};
                    // } else {
                    //     dp[i][w] = dp[i - 1][w];
                    // }
                }
            }
        }
        // std::cout << std::string(n * 8 + 16, '=') << std::endl;
        // print(dp[k]);
    }
    int64_t ans = 0;
    size_t best_k = 0;
    for (int k = 1; k <= n; ++k) {
        if (ans < dp[k][n][m]) {
            ans = dp[k][n][m];
            best_k = k;
        }
    }
    std::cout << ans << std::endl;

    // Восстановление ответа
    std::list<size_t> answer;
    for (size_t k = best_k, i = n, w = m; i != 0;) {
        if (dp[k][i][w] == dp[k][i - 1][w]) {
            --i;
        } else {
            answer.push_front(i);
            w -= objects[i - 1].first;
            --k;
            --i;
        }
    }
    for (const auto& elem : answer) { std::cout << elem << ' '; }
    std::cout << std::endl;
}