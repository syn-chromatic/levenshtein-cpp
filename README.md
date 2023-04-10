# levenshtein-cpp
Compute operational differences between two sequences using the Levenshtein algorithm. 


___
## Usage:
#### Regular Usage:
```cpp

Levenshtein lev = Levenshtein("test", "text");
int distance = lev.distance();
std::vector<std::vector<int>> array = lev.sequence_array();
```
