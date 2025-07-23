#include "Leaderboard.hpp"

/**
 * @brief Constructor for RankingResult with top players, cutoffs, and elapsed time.
 *
 * @param top Vector of top-ranked Player objects, in sorted order.
 * @param cutoffs Map of player count thresholds to minimum level cutoffs.
 *   NOTE: This is only ever non-empty for Online::rankIncoming().
 *         This parameter & the corresponding member should be empty
 *         for all Offline algorithms.
 * @param felapsed Time taken to calculate the ranking, in seconds.
 */
RankingResult::RankingResult(const std::vector<Player>& top, const std::unordered_map<size_t, size_t>& cutoffs, double elapsed)
    : top_ { top }
    , cutoffs_ { cutoffs }
    , elapsed_ { elapsed }
{
}

/**
 * @brief A helper method that replaces the minimum element
 * in a min-heap with a target value & preserves the heap
 * by percolating the new value down to its correct position.
 *
* Performs in O(log N) time.
*
* @pre The range [first, last) is a min-heap.
*
* @param first An iterator to a vector of Player objects
*      denoting the beginning of a min-heap
*      NOTE: Unlike the textbook, this is *not* an empty slot
*      used to store temporary values. It is the root of the heap.
*
* @param last An iterator to a vector of Player objects
*      denoting one past the end of a min-heap
*      (i.e. it is not considering a valid index of the heap)
*
* @param target A reference to a Player object to be inserted into the heap
* @post
* - The vector slice denoted from [first,last) is a min-heap
*   into which `target` has been inserted.
* - The contents of `target` is not guaranteed to match its original state
*   (ie. you may move it).
*/
void Online::replaceMin(PlayerIt first, PlayerIt last, Player& target) {
    // replace the first value with the target
    *first = target;
    // std::cout << "\n new_min" << target.level_;
    Online::percolateDown(first, first, last, false);

    // std::cout << "\nheap: ";
    // PlayerIt itr = first;
    // while (itr != last) {
    //     std::cout << (*itr).name_[0] << (*itr).level_ << " ";
    //     itr++;
    // }

}

/**
 * @brief Creates a heap from array and current subtree.
 * 
 * @param root Itr of root.
 * @param end Itr of end.
 * @param max Bool of if max or min heap
 * 
 * @post The array is now sorted as a max heap. 
 */
void Online::percolateDown(PlayerIt root, PlayerIt start, PlayerIt end, bool max) {
    auto itr = root;
    auto i = std::distance(start,itr);

    auto temp = *root;
    PlayerIt child;

    // while (itr + 2*i < end) {
    while (2*i+1 < std::distance(start,end)) { // child exist
        child = start + 2*i + 1;

        // in max heap swap with larger child
        if ((child+1) != end)  {
            if (max && *child < *(child+1)) { 
                child++;
            // in min heap swap with smaller child
            } else if (!max && *(child+1) < *child) {
                child++;
            } // else if (max && child+1 >= end) { 
            //     break;
        }

        // std::cout << " child" << std::distance(root, child) << "-" << (*child).level_;

        if (max && *child > temp) {
            std::iter_swap(itr,child);
        } else if (!max && *child < temp) {
            // std::cout << " swap" << (*child).level_ << "<->" << temp.level_;
            std::iter_swap(itr,child);
        } else {
            break;
        } 

        // update itr and i
        // if (max) {
        //     child--; // correct index
        // }
        itr = child;
        i = std::distance(start, itr);
    }
    // *itr = temp;
}

/**
* @brief Exhausts a stream of Players (ie. until there are none left) such that we:
* 1) Maintain a running collection of the <reporting_interval> highest leveled players
* 2) Record the Player level after reading every <reporting_interval> players
*    representing the minimum level required to be in the leaderboard at that point.
*
* @note You should use NOT use a priority-queue.
*       Instead, use a vector, the STL heap operations, & `replaceMin()`
*
* @param stream A stream providing Player objects
* @param reporting_interval The frequency at which to record cutoff levels
* @return A RankingResult in which:
* - top_       -> Contains the top <reporting_interval> Players read in the stream in
*                 sorted (least to greatest) order
* - cutoffs_   -> Maps player count milestones to minimum level required at that point
*                 including the minimum level after ALL players have been read, regardless
*                 of being a multiple of the reporting interval
* - elapsed_   -> Contains the duration (ms) of the selection/sorting operation
*                 excluding fetching the next player in the stream
*
* @post All elements of the stream are read until there are none remaining.
*
* @example Suppose we have:
* 1) A stream with 132 players
* 2) A reporting interval of 50
*
* Then our resulting RankingResult might contain something like:
* top_ = { Player("RECLUSE", 994), Player("WYLDER", 1002), ..., Player("DUCHESS", 1399) }, with length 50
* cutoffs_ = { 50: 239, 100: 992, 132: 994 } (see RankingResult explanation)
* elapsed_ = 0.003 (Your runtime will vary based on hardware)
*/
RankingResult Online::rankIncoming(PlayerStream& stream, const size_t& reporting_interval) {
    auto start = std::chrono::high_resolution_clock::now();

    Player current_player;
    size_t read = 0;

    std::vector<Player> min_top; 
    std::unordered_map<size_t, size_t> cutoffs_;

    while (stream.remaining() > 0) {
        current_player = stream.nextPlayer();
        read++;
        // players_streamed.push_back(current_player);

        // std::cout << "READ"<<read;
        if (read < reporting_interval) {
            min_top.push_back(current_player);

        } else if (read == reporting_interval) {
            min_top.push_back(current_player);

            // std::cout << "\nvector: ";
            // if (min_top.size() > 1) {
            //     PlayerIt itr = min_top.begin();
            //     while (itr != min_top.end()) {
            //         std::cout << (*itr).name_[0] << (*itr).level_ << " ";
            //         itr++;
            //     }
            // }
            // std::cout << "\n";

            std::make_heap(min_top.begin(), min_top.end(), std::greater<Player>());
            // Online::buildHeap(min_top.begin(), min_top.end());

            // std::cout << "\nMAKE HEAP: ";
            // if (min_top.size() > 1) {
            //     PlayerIt itr = min_top.begin();
            //     while (itr != min_top.end()) {
            //         std::cout << (*itr).name_[0] << (*itr).level_ << " ";
            //         itr++;
            //     }
            // }
        } else {
            // if next player has higher score than player on leaderboard, replace min
            if (current_player.level_ >= min_top.front().level_) {
                Online::replaceMin(min_top.begin(), min_top.end(), current_player);
            }
        }

        if (read % reporting_interval == 0 && !min_top.empty()) {
            cutoffs_.insert({read,min_top[0].level_});
        }
    }

    cutoffs_.insert({read, min_top[0].level_});

    // std::cout << "\nview HEAP: ";
    // if (min_top.size() > 1) {
    //     PlayerIt itr = min_top.begin();
    //     while (itr != min_top.end()) {
    //         std::cout << (*itr).name_[0] << (*itr).level_ << " ";
    //         itr++;
    //     }
    // }
    // min_top.push_back(Player());
    // std::sort_heap(min_top.begin(), min_top.end(), std::less<Player>());
    // min_top.pop();
    std::sort(min_top.begin(), min_top.end());

    auto end = std::chrono::high_resolution_clock::now();
    return RankingResult(min_top,cutoffs_,std::chrono::duration<double>(end-start).count());
}

/**
 * @brief buildHeaps min heap with percolateDown/
 * 
 * @param first Begin itr for player vector.
 * @param last End itr for player vector.
 * 
 * @post The array is now sorted as a min heap. 
 */
void Online::buildHeap(PlayerIt first, PlayerIt last) {
    int length = std::distance(first,last);
    auto mid_itr = first + (length/2);

    while (mid_itr != first-1) {
        // std::cout << " build" << std::distance(first, mid_itr);
        Online::percolateDown(mid_itr, first, last, false);
        mid_itr--;
    }
}

/**
 * @brief Creates a heap from array and current subtree.
 * 
 * @param heap The heap to sort.
 * @param i The index of current subroot.
 * 
 * @post The array is now sorted as a max heap. 
 */
template <typename T>
void Offline::heapify(std::vector<T>& heap, int i, int end) {
    while (true) {
        // find root, left, right child index
        int j = i; // index of largest node
        int left_i = 2*i + 1;
        int right_i = 2*i + 2; 

        // largest = 0;
        // check if left or right should be root
        if (left_i < end && heap[left_i] > heap[j]) {
            j = left_i;
        }
        if (right_i < end && heap[right_i] > heap[j]) {
            j = right_i;
        }

        // if root not largest, change
        if (j != i) {
            // std::swap(heap[i], heap[root]);
            std::iter_swap(heap.begin()+i, heap.begin()+j);
            i = j;
            // recursively change subtree
            // Offline::heapify(heap, j, end);
        } else {
            break;
        }
    }
}

/**
 * @brief Uses an early-stopping version of heapsort to
 *        select and sort the top 10% of players in-place
 *        (excluding the returned RankingResult vector)
 *
 * @param players A reference to the vector of Player objects to be ranked
 * @return A Ranking Result object whose
 * - top_ vector -> Contains the top 10% of players from the input in sorted order (ascending)
 * - cutoffs_    -> Is empty
 * - elapsed_    -> Contains the duration (ms) of the selection/sorting operation
 *
 * @post The order of the parameter vector is modified.
 */
RankingResult Offline::heapRank(std::vector<Player>& players) { // o(n)
    if (players.size() <= 1) {
        return players;
    }
    
    // index of where the top players would be
    int top_10 = std::floor(0.1 * players.size());

    auto start = std::chrono::high_resolution_clock::now();

    std::make_heap(players.begin(), players.end(), std::less<Player>()); // o(n)

    // deleteMax top_10 times
    for (int i = 0; i < top_10; ++i) { // move to the start of top 10 section
        std::pop_heap(players.begin(), players.end()-i); // o(logn)
    }

    auto end = std::chrono::high_resolution_clock::now();

    return RankingResult(std::vector<Player>(players.end()-top_10, players.end()),{},std::chrono::duration<double>(end-start).count());
}

/**
* @brief Uses a mixture of quickselect/quicksort to
* select and sort the top 10% of players in-place
* (excluding the returned RankingResult vector)
*
* @param players A reference to the vector of Player objects to be ranked
* @return A Ranking Result object whose
* - top_ vector -> Contains the top 10% of players from the input in sorted order (ascending)
* - cutoffs_ -> Is empty
* - elapsed_ -> Contains the duration (ms) of the selection/sorting operation
*
* @post The order of the parameter vector is modified.
*/
RankingResult Offline::quickSelectRank(std::vector<Player>& players) { // o(n^2)
    int top_10 = std::floor(0.1 * players.size());

    auto start = std::chrono::high_resolution_clock::now();

    // quickSelect for top 10%
    quickSelect(players, 0, players.size()-1, top_10); // o(n^2)

    // std::cout << "after selection: ";
    // for (auto p : players) {
    //     std::cout << p.name_[0] << p.level_ << " ";
    // }

    // quickSort top 10%
    quickSort(players, 0, top_10-1); // o(n^2)*0.10

    // std::cout << "\nafter sort: ";
    // for (auto p : players) {
    //     std::cout << p.name_[0] << p.level_ << " ";
    // }

    auto end = std::chrono::high_resolution_clock::now();

    return RankingResult(std::vector<Player>(players.begin(),players.begin()+top_10),{},std::chrono::duration<double>(end-start).count());
}

/**
 * @brief Helper for the quickSort function.
 * 
 * @param array The array to quicksort.
 * @param low The lower range of section to sort
 * @param high The upper range of section to sort
 * 
 * @return The pivot for the inputs given.
 */
template <typename T>
int Offline::lomuto_partition(std::vector<T>& array, int low, int high, int pivot) {
    // int i = low-1;
    // T pivot_value = array[pivot];
    // std::swap(array[pivot], array[high]);
    // for (int j = i; j < high; ++j) {
    //     if (array[j] > pivot_value || array[j] == pivot_value) { // or greater?
    //         // std::iter_swap(array.begin()+i,array.begin()+j);
    //         std::swap(array[i], array[j]);
    //         i++;
    //     }
    // }
    // // move pivot
    // std::swap(array[high], array[pivot]);

    // return i+1;

    int i = low;
    for (int j = i; j < high; ++j) {
        if (array[j] > array[pivot]) {
            std::swap(array[i], array[j]);
            i++;
        }
    }
    std::swap(array[pivot], array[i]);
    return i;

}

template <typename T>
int Offline::hoare_partition(std::vector<T>& array, int low, int high, int pivot) {    
    int i = low-1;
    int j = high+1; 

    T pivot_value = array[pivot];
    
    while (true) {
        do {
            i++;
        } while ((array[i]<pivot_value) && i <= high);
        do {
            j--;
        } while ((array[j]>pivot_value) && j >= low);

        if (i > j) {
            break;
        }
        std::swap(array[i],array[j]);
    }
    return j;
}

/**
 * @param array The array for quickselection.
 * @param low The lower range of section to sort
 * @param high The upper range of section to sort
 * @param k The kth element that is the cutoff for sorting.
 * 
 * 
 * @post The array is now by 10% line.
 */
template <typename T>
void Offline::quickSelect(std::vector<T>& array, int low, int high, int k) {
    if (low <= high && low >= 0) {
        int index = Offline::lomuto_partition(array, low, high, high); //(low+high)/2
        
        // std::cout << "\n" << array[high].name_[0] << array[high].level_ << ": ";
        // for (auto t : array) {
        //     std::cout << t.name_[0] << t.level_ << " ";
        // }

        if (index < k) {
            quickSelect(array, index+1, high, k);
        } else {
            quickSelect(array, low, index-1, k);
        }
    }
}

/**
 * @param array The array to quicksort.
 * @param low The lower range of section to sort
 * @param high The upper range of section to sort
 * 
 * @post The array is now sorted.
 */
template <typename T>
void Offline::quickSort(std::vector<T>& array, int low, int high) {
    if (low < high && low >= 0) {
        int pivot = Offline::hoare_partition(array, low, high, high);
        quickSort(array, low, pivot);
        quickSort(array, pivot+1, high);
    }
}
