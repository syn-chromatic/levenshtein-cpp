#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <tuple>

class Levenshtein
{
public:
    Levenshtein(std::string seq1, std::string seq2) : seq1(seq1), seq2(seq2) {}

    int distance()
    {
        const std::string &s1 = seq1;
        const std::string &s2 = seq2;
        auto result = calculate_distance(s1, s2);
        return std::get<1>(result);
    }

    std::vector<std::vector<int>> sequence_array()
    {
        const std::string &s1 = seq1;
        const std::string &s2 = seq2;
        auto result = calculate_distance(s1, s2);
        return std::get<0>(result);
    }

private:
    std::string seq1;
    std::string seq2;

    std::tuple<std::vector<std::vector<int>>, int> calculate_distance(const std::string &s1, const std::string &s2)
    {
        auto result = get_sequence_array(s1, s2);
        std::vector<std::vector<int>> &seq_array = std::get<0>(result);
        std::vector<std::vector<std::pair<size_t, char>>> &seq_lookup = std::get<1>(result);

        const int ops_costs[] = {0, 0, 1, 1, 1};

        size_t s1_len = s1.size() + 1;
        size_t s2_len = s2.size() + 1;

        for (int x = 0; x < static_cast<int>(s1_len); ++x)
        {
            for (int y = 0; y < static_cast<int>(s2_len); ++y)
            {
                std::array<int, 4> op_values = dynamic_operations(x, y, seq_array, seq_lookup, s1_len, s2_len);
                int op_value = op_values[2];
                int op_key = op_values[3];
                int op_cost = ops_costs[op_key];
                seq_array[y][x] = op_value + op_cost;
            }
        }
        int dist_val = seq_array[s2_len - 1][s1_len - 1];
        return std::make_tuple(seq_array, dist_val);
    }

    std::tuple<std::vector<std::vector<int>>, std::vector<std::vector<std::pair<size_t, char>>>> get_sequence_array(const std::string &s1, const std::string &s2)
    {
        std::vector<char> s1_c = {'\0'};
        s1_c.insert(s1_c.end(), s1.begin(), s1.end());
        std::vector<char> s2_c = {'\0'};
        s2_c.insert(s2_c.end(), s2.begin(), s2.end());

        std::vector<std::vector<int>> seq_arr(s2_c.size(), std::vector<int>(s1_c.size(), 0));
        std::vector<std::vector<std::pair<size_t, char>>> seq_lookup(2);

        for (size_t s_index = 0; s_index < 2; ++s_index)
        {
            const std::vector<char> &seq = (s_index == 0) ? s1_c : s2_c;
            for (size_t l_index = 0; l_index < seq.size(); ++l_index)
            {
                seq_lookup[s_index].emplace_back(l_index, seq[l_index]);
            }
        }

        return std::make_tuple(seq_arr, seq_lookup);
    }

    std::array<int, 4> get_min_ops(const std::array<std::array<int, 4>, 3> &ops_array)
    {
        std::array<int, 4> min_ops = {0, 0, 0, 0};
        bool min_ops_initialized = false;

        for (const auto &op : ops_array)
        {
            if (op[0] >= 0 && op[1] >= 0)
            {
                if (!min_ops_initialized)
                {
                    min_ops = op;
                    min_ops_initialized = true;
                }
                else if (op[2] < min_ops[2])
                {
                    min_ops = op;
                }
            }
        }

        if (!min_ops_initialized)
        {
            throw std::runtime_error("Failed to retrieve Minimum Operation.");
        }
        return min_ops;
    }

    std::pair<int, int> get_insert(int x, int y, size_t s1_len, size_t s2_len)
    {
        if (s1_len < s2_len)
        {
            return std::make_pair(x, y - 1);
        }
        return std::make_pair(x - 1, y);
    }

    std::pair<int, int> get_replace(int x, int y)
    {
        return std::make_pair(x - 1, y - 1);
    }

    std::pair<int, int> get_delete(int x, int y, size_t s1_len, size_t s2_len)
    {
        if (s1_len < s2_len)
        {
            return std::make_pair(x - 1, y);
        }
        return std::make_pair(x, y - 1);
    }

    int get_value(int x, int y, const std::vector<std::vector<int>> &seq_array)
    {
        if (x >= 0 && y >= 0)
        {
            return seq_array[y][x];
        }
        return 0;
    }

    std::array<int, 4> dynamic_operations(
        int x, int y,
        const std::vector<std::vector<int>> &seq_array,
        const std::vector<std::vector<std::pair<size_t, char>>> &seq_lookup,
        size_t s1_len, size_t s2_len)
    {

        const auto &[_, char1] = seq_lookup[0][x];
        const auto &[__, char2] = seq_lookup[1][y];

        auto [x_ins, y_ins] = get_insert(x, y, s1_len, s2_len);
        auto [x_rep, y_rep] = get_replace(x, y);
        auto [x_del, y_del] = get_delete(x, y, s1_len, s2_len);

        int ins_val = get_value(x_ins, y_ins, seq_array);
        int rep_val = get_value(x_rep, y_rep, seq_array);
        int del_val = get_value(x_del, y_del, seq_array);

        bool onset_state = x_rep + y_rep == -2;
        bool match_state = char1 == char2;
        std::array<int, 4> op_array;

        if (onset_state)
        {
            op_array = {0, 0, 0, 0}; // x, y, val, onset;
        }
        else if (match_state)
        {
            op_array = {x_rep, y_rep, rep_val, 1}; // x, y, val, match;
        }
        else
        {
            std::array<std::array<int, 4>, 3> ops_array = {
                std::array<int, 4>{x_ins, y_ins, ins_val, 2}, // x, y, val, insert
                std::array<int, 4>{x_rep, y_rep, rep_val, 3}, // x, y, val, replace
                std::array<int, 4>{x_del, y_del, del_val, 4}, // x, y, val, delete
            };

            op_array = get_min_ops(ops_array);
        }
        return op_array;
    }
};

int main()
{
    int distance = Levenshtein(
                       "oqwgleTdQwwOrwczTPAfFpYkRBhCwHKGvVESJdCSuRlTWtZUqeLvWpapeWlA",
                       "fWnqFjXvsZQDwPVTEHqIdxRmctJmjnDlzpZIMLTSoqwgleTdQwwOrwczTPAf")
                       .distance();
    return 0;
}
