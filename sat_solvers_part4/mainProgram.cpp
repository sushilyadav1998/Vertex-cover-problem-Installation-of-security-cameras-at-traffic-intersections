#include <iostream>
#include <sstream>
#include <vector>
#include <memory>
#include <algorithm>
#include "minisat/core/SolverTypes.h"
#include "minisat/core/Solver.h"
#include <pthread.h>
#include <time.h>
#include <chrono>
#include <mutex>
#include <set>
#include <condition_variable>
#include <iomanip>

/**
 * Used to define log levels
 * Make sure log_threshold is 2 before the final submission
 */
int log_threshold = 2;

class my_stream_buf : public std::streambuf {
};

my_stream_buf no_stream_buf;
std::ostream no_cout(&no_stream_buf);
#define log(x) ((x >= log_threshold)? std::cout : no_cout)

/**
 * For analysis change this to 10 but make sure to change it back to 1 before the final submission
 */
const int number_of_times = 1;

/**
 * Wait timeout for cnf and 3cnf
 * Time unit is milliseconds
 */
const int wait_time_cutoff = 10000;

/**
 * indexing
 */
const int indexing = 0;

std::vector<std::pair<int, int>> get_parsed_edges(std::istringstream &input, int V, bool &is_correct);

void find_vertex_cover(int V, std::vector<std::pair<int, int>> &edges);

void initialize_literal(Minisat::vec<Minisat::Lit> litV[], int n, int m, std::unique_ptr<Minisat::Solver> &solver);

void at_least_one_vertex(Minisat::vec<Minisat::Lit> litV[], int n, int m, std::unique_ptr<Minisat::Solver> &solver);

void no_vertex_twice(Minisat::vec<Minisat::Lit> litV[], int n, int m, std::unique_ptr<Minisat::Solver> &solver);

void no_more_than_one_on_a_single_position(Minisat::vec<Minisat::Lit> litV[], int n, int m,
                                           std::unique_ptr<Minisat::Solver> &solver);

void every_edge_covered(Minisat::vec<Minisat::Lit> litV[], int n, int m, std::unique_ptr<Minisat::Solver> &solver,
                        std::vector<std::pair<int, int>> &edges);

void print_vertex_cover(Minisat::vec<Minisat::Lit> litV[], int V, int ans_k,
                        std::unique_ptr<Minisat::Solver> &solver, int methodNumber);

void clear_everything();

void clear_everything_within_10_runs();

void clear_top();

void find_VC_using_various_methods();

/**
 *
 * 3CNF
 */
void initialize_literal_3cnf(Minisat::vec<Minisat::Lit> litV[], int n, int m, std::unique_ptr<Minisat::Solver> &solver);

void find_vertex_cover_3cnf(int V, std::vector<std::pair<int, int>> &edges);

void no_more_than_one_on_a_single_position_3cnf(Minisat::vec<Minisat::Lit> litV[], int n, int m,
                                                std::unique_ptr<Minisat::Solver> &solver);

void no_vertex_twice_3cnf(Minisat::vec<Minisat::Lit> litV[], int n, int m,
                          std::unique_ptr<Minisat::Solver> &solver);

void every_edge_covered_3cnf(Minisat::vec<Minisat::Lit> litV[], int n, int m, std::unique_ptr<Minisat::Solver> &solver,
                             std::vector<std::pair<int, int>> &edges);

void print_vertex_cover_3cnf(Minisat::vec<Minisat::Lit> litV[], int V, int ans_k,
                             std::unique_ptr<Minisat::Solver> &solver);

void
at_least_one_vertex_3cnf(Minisat::vec<Minisat::Lit> litV[], int n, int m, std::unique_ptr<Minisat::Solver> &solver);


/**
 * thread handlers
 */
void *input_output_handler(void *arg);

void *find_vertex_cover_cnf_sat_handler(void *arg);

void *find_vertex_cover_3cnf_sat_handler(void *arg);

void *find_vertex_cover_approx_vc1_handler(void *arg);

void *find_vertex_cover_approx_vc2_handler(void *arg);

void *find_vertex_cover_refined_vc1_handler(void *arg);

void *find_vertex_cover_refined_vc2_handler(void *arg);

void *timeout_cnf_sat_handler(void *arg);

void *timeout_3cnf_sat_handler(void *arg);

/**
 * thread creator functions
 */
void find_vertex_cover_cnf_sat();

void find_vertex_cover_3cnf_sat();

void find_vertex_cover_approx_vc1();

void find_vertex_cover_approx_vc2();

void find_vertex_cover_refined_vc1();

void find_vertex_cover_refined_vc2();

void time_out_cnf_sat();

void time_out_3cnf_sat();

/**
 *
 0-MAIN-THREAD
 1-CNF-SAT-VC
 2-CNF-3-SAT-VC
 3-APPROX-VC-1
 4-APPROX-VC-2
 5-REFINED-APPROX-VC-1
 6-REFINED-APPROX-VC-2
 *
 */
std::vector<std::pair<std::string, int>> globalMethodMap = {{"CNF-SAT-VC",          1},
                                                            {"CNF-3-SAT-VC",        2},
                                                            {"APPROX-VC-1",         3},
                                                            {"APPROX-VC-2",         4},
                                                            {"REFINED-APPROX-VC-1", 5},
                                                            {"REFINED-APPROX-VC-2", 6}};

pthread_t thread_arr[7];
pthread_t timeout_thread[2];

/**
 * Only clear for new input run
 */
std::vector<std::chrono::microseconds> elapsed_times[7];
std::chrono::microseconds elapsed_time_local[7];

/**
 * Global variable section - make sure to clear all these
 */
int globalVertices = 0;
std::vector<std::pair<int, int>> globalEdges;
std::vector<int> globalVertexCover[7];
std::vector<int> approx_vc1_ans;
std::vector<int> approx_vc2_ans;
std::vector<int> refined_vc1_ans;
std::vector<int> refined_vc2_ans;


std::vector<long double> appx_ratio[7];

/**
 * Variables to synchronize dependent threads
 */
std::mutex mu1, mu2, mu3, mu4;
bool ready1 = false, ready2 = false;
std::condition_variable cv1, cv2;
bool cnf_sat_flag_break = false, three_cnf_sat_flag_break = false;
std::condition_variable cnv_sat, cnv_sat_3;

/**
 * Sat solver instances
 */
std::unique_ptr<Minisat::Solver> solver1(new Minisat::Solver());
std::unique_ptr<Minisat::Solver> solver2(new Minisat::Solver());


int main(int argc, char **argv) {
    clear_top();
    pthread_create(&thread_arr[0], NULL, &input_output_handler, NULL);
    pthread_join(thread_arr[0], NULL);

    log(1) << "---------------Overall Analysis for all runs------------------\n";
    /**
     * After 100 runs of input
     */

    /**
     * Mean calculation
     */
    long double avg = 0;
    std::vector<long double> time_average_for_methods;
    log(1) << std::fixed;
    for (int i = 1; i < 7; i++) {
        avg = 0;
        long double elapsed_times_count = elapsed_times[i].size();
        for (int j = 0; j < elapsed_times_count; j++) {
            avg += elapsed_times[i][j].count();
        }
        avg /= elapsed_times_count;
        log(1) << "Average " << globalMethodMap[i - 1].first << ": " << avg << '\n';
        time_average_for_methods.push_back(avg);
    }
    log(1) << "----------------------------\n";
    /**
     * standard deviation calculation
     */
    long double sd = 0;
    std::vector<long double> sd_for_methods;
    for (int i = 1; i < 7; i++) {
        sd = 0;
        long double elapsed_times_count = elapsed_times[i].size();
        for (int j = 0; j < elapsed_times_count; j++) {
            sd += pow((time_average_for_methods[i - 1] - elapsed_times[i][j].count()), 2);
        }
        sd /= elapsed_times_count;
        sd = sqrt(sd);
        log(1) << "Standard deviation " << globalMethodMap[i - 1].first << ": " << sd << '\n';
        sd_for_methods.push_back(sd);
    }
    log(1) << "-------------------------\n";
    /**
     * Approximation ratio calculation
     */
    long double apx_avg = 0;
    std::vector<long double> apx_ratio_avg;
    for (int i = 1; i < 7; i++) {
        apx_avg = 0;
        long double appx_ratio_size = appx_ratio[i].size();
        for (int j = 0; j < (int) appx_ratio[i].size(); j++) {
            apx_avg += appx_ratio[i][j];
        }
        apx_avg /= appx_ratio_size;
        apx_ratio_avg.push_back(apx_avg);
        log(1) << "Approximation ratio " << globalMethodMap[i - 1].first << ": " << apx_avg << '\n';
    }
}

void *input_output_handler(void *arg) {
    int vertex_count = 0;
    while (!std::cin.eof()) {
        clear_everything();
        std::string line;
        std::getline(std::cin, line);
        if (std::cin.eof())
            break;
        std::istringstream input(line);
        char command;
        input >> command;

        if (command == 'V') {
            input >> vertex_count;
            if (vertex_count < 0) {
                std::cerr << "Error: wrong vertex input\n";
                continue;
            }
        } else if (command == 'E') {
            bool is_correct = true;
            std::vector<std::pair<int, int>> edges = get_parsed_edges(input, vertex_count, is_correct);
            if (!is_correct) {
                std::cerr << "Error: wrong edge input\n";
                continue;
            }
            globalVertices = vertex_count;
            globalEdges = edges;
            find_VC_using_various_methods();

            /**
            * vertex cover output
            */
            for (auto itr: globalMethodMap) {
                log(2) << itr.first << ": ";
                std::sort(globalVertexCover[itr.second].begin(), globalVertexCover[itr.second].end());
                if ((int) globalVertexCover[itr.second].size() == 0 && globalEdges.size() != 0) {
                    log(2) << "timeout\n";
                    continue;
                }
                for (int i = 0; i < (int) globalVertexCover[itr.second].size(); i++) {
                    log(2) << globalVertexCover[itr.second][i] + indexing;
                    if (i + 1 != (int) globalVertexCover[itr.second].size()) log(2) << ',';
                }
                log(2) << '\n';
            }

            log(1) << "-----------------List average for every 10 runs-----------------\n";
            for (int i = 1; i < 7; i++) {
                log(1) << "Elapsed time " << globalMethodMap[i - 1].first << ": ";
                for (int j = 0; j < (int) elapsed_times[i].size(); j++) {
                    log(1) << elapsed_times[i][j].count();
                    if (j + 1 != (int) elapsed_times[i].size()) log(1) << ' ';
                }
                log(1) << '\n';
            }
            log(1) << "------------------------End----------------------------------\n";
            /**
             * Calculate approximation ratio will remain same for all 10 runs so it is calculated here
             */
            for (int i = 1; i < 7; i++) {
                if (!globalVertexCover[1].empty()) {
                    long double numerator = globalVertexCover[i].size();
                    long double denominator = globalVertexCover[1].size();
                    appx_ratio[i].push_back(numerator / denominator);
                } else if (!globalVertexCover[2].empty()) {
                    long double numerator = globalVertexCover[i].size();
                    long double denominator = globalVertexCover[2].size();
                    appx_ratio[i].push_back(numerator / denominator);
                }
            }
        }
    }
    return NULL;
}

void find_VC_using_various_methods() {
    for (int i = 0; i < number_of_times; i++) {
        clear_everything_within_10_runs();

        find_vertex_cover_cnf_sat();
        find_vertex_cover_3cnf_sat();
        find_vertex_cover_approx_vc1();
        find_vertex_cover_approx_vc2();
        find_vertex_cover_refined_vc1();
        find_vertex_cover_refined_vc2();

        time_out_cnf_sat();
        time_out_3cnf_sat();

        pthread_join(timeout_thread[0], NULL);
        pthread_join(timeout_thread[1], NULL);

        pthread_join(thread_arr[1], NULL);
        pthread_join(thread_arr[2], NULL);
        pthread_join(thread_arr[3], NULL);
        pthread_join(thread_arr[4], NULL);
        pthread_join(thread_arr[5], NULL);
        pthread_join(thread_arr[6], NULL);


        /**
         * Time calculation after every run
         */
        for (int j = 1; j < 7; j++) {
            elapsed_times[j].push_back(elapsed_time_local[j]);
        }
    }
}

/**
 * Thread creation
 */
void find_vertex_cover_cnf_sat() {
    pthread_create(&thread_arr[1], NULL, &find_vertex_cover_cnf_sat_handler, NULL);
}

void find_vertex_cover_3cnf_sat() {
    pthread_create(&thread_arr[2], NULL, &find_vertex_cover_3cnf_sat_handler, NULL);
}

void find_vertex_cover_approx_vc1() {
    pthread_create(&thread_arr[3], NULL, &find_vertex_cover_approx_vc1_handler, NULL);
}

void find_vertex_cover_approx_vc2() {
    pthread_create(&thread_arr[4], NULL, &find_vertex_cover_approx_vc2_handler, NULL);
}

void find_vertex_cover_refined_vc1() {
    pthread_create(&thread_arr[5], NULL, &find_vertex_cover_refined_vc1_handler, NULL);
}

void find_vertex_cover_refined_vc2() {
    pthread_create(&thread_arr[6], NULL, &find_vertex_cover_refined_vc2_handler, NULL);
}

void time_out_cnf_sat() {
    pthread_create(&timeout_thread[0], NULL, &timeout_cnf_sat_handler, NULL);
}

void time_out_3cnf_sat() {
    pthread_create(&timeout_thread[1], NULL, &timeout_3cnf_sat_handler, NULL);
}

/**
 * Handlers
 */

void *timeout_cnf_sat_handler(void *arg) {
    /**
     * timeout on cnf sat thread
     */
    try {
        std::unique_lock<std::mutex> lock(mu3);
        std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
        cnv_sat.wait_until(lock, begin + std::chrono::milliseconds(wait_time_cutoff),
                           [] { return cnf_sat_flag_break; });
        cnf_sat_flag_break = true;
        solver1->interrupt();
    } catch (...) {
        return NULL;
    }
    return NULL;
}

void *timeout_3cnf_sat_handler(void *arg) {
    /**
     * timeout on 3-cnf sat thread
     */
    try {
        std::unique_lock<std::mutex> lock(mu4);
        std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
        cnv_sat_3.wait_until(lock, begin + std::chrono::milliseconds(wait_time_cutoff),
                             [] { return three_cnf_sat_flag_break; });
        three_cnf_sat_flag_break = true;
        solver2->interrupt();
    } catch (...) {
        return NULL;
    }
    return NULL;
}

//handler 1
void *find_vertex_cover_cnf_sat_handler(void *arg) {
    std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
    find_vertex_cover(globalVertices, globalEdges);
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::microseconds elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    elapsed_time_local[1] = elapsed_time;
    return NULL;
}

//handler 2
void *find_vertex_cover_3cnf_sat_handler(void *arg) {
    std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
    find_vertex_cover_3cnf(globalVertices, globalEdges);
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::microseconds elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    elapsed_time_local[2] = elapsed_time;
    return NULL;
}

//handler 3
void *find_vertex_cover_approx_vc1_handler(void *arg) {
    std::unique_lock<std::mutex> lock(mu1);
    ready1 = true;
    std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
    std::vector<std::pair<int, int>> localEdge = globalEdges;
    int localVertices = globalVertices;
    std::vector<int> inDegree;
    inDegree.reserve(localVertices);
    for (int i = 0; i < localVertices; i++) {
        inDegree[i] = 0;
    }
    std::set<std::pair<int, int>> edgeSet;
    for (std::pair<int, int> p: localEdge) {
        inDegree[p.first]++, inDegree[p.second]++;
        edgeSet.insert(p);
    }
    std::vector<std::pair<int, int>> inDegree_set;
    inDegree_set.reserve(localVertices);
    for (int i = 0; i < localVertices; i++) {
        inDegree_set.emplace_back(inDegree[i], i);
    }
    std::sort(inDegree_set.begin(), inDegree_set.end());
    while (!edgeSet.empty()) {
        if (inDegree_set.empty()) break;
        std::pair<int, int> val = inDegree_set.back();
        inDegree_set.pop_back();
        approx_vc1_ans.push_back(val.second);
        std::vector<std::pair<int, int>> to_be_cleared;
        for (auto itr: edgeSet) {
            if (itr.first == val.second || itr.second == val.second) {
                to_be_cleared.emplace_back(itr);
            }
        }
        for (auto itr: to_be_cleared) edgeSet.erase(itr);
    }
    for (auto itr: approx_vc1_ans) {
        globalVertexCover[3].push_back(itr);
    }
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::microseconds elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    elapsed_time_local[3] = elapsed_time;
    cv1.notify_all();
    return NULL;
}

//handler 4
void *find_vertex_cover_approx_vc2_handler(void *arg) {
    std::unique_lock<std::mutex> lock(mu2);
    ready2 = true;
    std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
    std::vector<std::pair<int, int>> localEdge = globalEdges;
    std::set<std::pair<int, int>> edgeSet;
    for (std::pair<int, int> p: localEdge) {
        edgeSet.insert(p);
    }
    for (auto itr: localEdge) {
        if (edgeSet.find(itr) == edgeSet.end()) continue;
        approx_vc2_ans.push_back(itr.first);
        approx_vc2_ans.push_back(itr.second);
        std::vector<std::pair<int, int>> to_be_removed;
        for (auto itr_edgeSet: edgeSet) {
            if (itr.first == itr_edgeSet.first || itr.first == itr_edgeSet.second
                || itr.second == itr_edgeSet.first || itr.second == itr_edgeSet.second)
                to_be_removed.emplace_back(itr_edgeSet);
        }
        for (auto itr_to_be_removed: to_be_removed) {
            edgeSet.erase(itr_to_be_removed);
        }
        if (edgeSet.empty()) break;
    }
    std::set<int> holder;
    for (auto itr: approx_vc2_ans) {
        globalVertexCover[4].push_back(itr);
    }
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::microseconds elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    elapsed_time_local[4] = elapsed_time;
    cv2.notify_all();
    return NULL;
}

//handler 5
void *find_vertex_cover_refined_vc1_handler(void *arg) {
    std::unique_lock<std::mutex> lock(mu1);
    cv1.wait(lock, [] { return ready1; });
    std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
    std::vector<std::pair<int, int>> localEdge = globalEdges;
    int localVertices = globalVertices;
    std::vector<int> inDegree;
    inDegree.reserve(localVertices);
    for (int i = 0; i < localVertices; i++) {
        inDegree[i] = 0;
    }
    std::set<std::pair<int, int>> edgeSet;
    for (std::pair<int, int> p: localEdge) {
        inDegree[p.first]++, inDegree[p.second]++;
        edgeSet.insert(p);
    }
    std::vector<std::pair<int, int>> inDegree_set;
    inDegree_set.reserve(localVertices);
    for (int i = 0; i < localVertices; i++) {
        bool isInAVC1 = false;
        for (int j = 0; j < (int) approx_vc1_ans.size(); j++) {
            if (i == approx_vc1_ans[j]) {
                isInAVC1 = true;
                break;
            }
        }
        if (isInAVC1) {
            inDegree_set.emplace_back(inDegree[i], i);
        }
    }
    std::sort(inDegree_set.begin(), inDegree_set.end());
    std::set<int> unnecessary_vertex;
    for (int iter = 0; iter < (int) inDegree_set.size(); iter++) {
        std::vector<std::pair<int, int>> toBeRemoved;
        std::set<std::pair<int, int>> edgeSet_status = edgeSet;
        for (int j = 0; j < (int) inDegree_set.size(); j++) {
            if (j != iter && unnecessary_vertex.find(inDegree_set[j].second) == unnecessary_vertex.end()) {
                for (auto set_itr: edgeSet) {
                    if (inDegree_set[j].second == set_itr.first || inDegree_set[j].second == set_itr.second)
                        toBeRemoved.push_back(set_itr);
                }
            }
        }
        for (auto toBeRemoved_itr: toBeRemoved) {
            edgeSet_status.erase(toBeRemoved_itr);
        }
        if (edgeSet_status.empty()) {
            unnecessary_vertex.insert(inDegree_set[iter].second);
        }
    }
    for (int iter = 0; iter < (int) inDegree_set.size(); iter++) {
        if (unnecessary_vertex.find(inDegree_set[iter].second) == unnecessary_vertex.end())
            globalVertexCover[5].push_back(inDegree_set[iter].second);
    }
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::microseconds elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    elapsed_time_local[5] = elapsed_time;
    return NULL;
}

//handler 6
void *find_vertex_cover_refined_vc2_handler(void *arg) {
    std::unique_lock<std::mutex> lock(mu2);
    cv2.wait(lock, [] { return ready2; });
    std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
    std::vector<std::pair<int, int>> localEdge = globalEdges;
    int localVertices = globalVertices;
    std::vector<int> inDegree;
    inDegree.reserve(localVertices);
    for (int i = 0; i < localVertices; i++) {
        inDegree[i] = 0;
    }
    std::set<std::pair<int, int>> edgeSet;
    for (std::pair<int, int> p: localEdge) {
        inDegree[p.first]++, inDegree[p.second]++;
        edgeSet.insert(p);
    }
    std::vector<std::pair<int, int>> inDegree_set;
    inDegree_set.reserve(localVertices);
    for (int i = 0; i < localVertices; i++) {
        bool isInAVC2 = false;
        for (int j = 0; j < (int) approx_vc2_ans.size(); j++) {
            if (i == approx_vc2_ans[j]) {
                isInAVC2 = true;
                break;
            }
        }
        if (isInAVC2) {
            inDegree_set.emplace_back(inDegree[i], i);
        }
    }
    std::sort(inDegree_set.begin(), inDegree_set.end());
    std::set<int> unnecessary_vertex;
    for (int iter = 0; iter < (int) inDegree_set.size(); iter++) {
        std::vector<std::pair<int, int>> toBeRemoved;
        std::set<std::pair<int, int>> edgeSet_status = edgeSet;
        for (int j = 0; j < (int) inDegree_set.size(); j++) {
            if ((j != iter) && (unnecessary_vertex.find(inDegree_set[j].second) == unnecessary_vertex.end())) {
                for (auto set_itr: edgeSet) {
                    if (inDegree_set[j].second == set_itr.first || inDegree_set[j].second == set_itr.second)
                        toBeRemoved.push_back(set_itr);
                }
            }
        }
        for (auto toBeRemoved_itr: toBeRemoved) {
            edgeSet_status.erase(toBeRemoved_itr);
        }
        if (edgeSet_status.empty()) {
            unnecessary_vertex.insert(inDegree_set[iter].second);
        }
    }
    for (int iter = 0; iter < (int) inDegree_set.size(); iter++) {
        if (unnecessary_vertex.find(inDegree_set[iter].second) == unnecessary_vertex.end())
            globalVertexCover[6].push_back(inDegree_set[iter].second);
    }
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::microseconds elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    elapsed_time_local[6] = elapsed_time;
    return NULL;
}


/**
 * CNF
 */
void find_vertex_cover(int V, std::vector<std::pair<int, int>> &edges) {
    bool found = false;
    solver1.reset(new Minisat::Solver());
    for (int k = 0; k <= V; k++) {
        if (cnf_sat_flag_break) break;
        Minisat::vec<Minisat::Lit> litV[V];
        initialize_literal(litV, V, k, solver1);
        at_least_one_vertex(litV, V, k, solver1);
        no_vertex_twice(litV, V, k, solver1);
        every_edge_covered(litV, V, k, solver1, edges);
        no_more_than_one_on_a_single_position(litV, V, k, solver1);
        log(0) << "Going to call solver from cnf " << k << '\n';
        int res = solver1->solve();
        log(0) << "res from solver cnf " << res << " k: " << k << '\n';
        if (res == 1) {
            log(0) << "cnf sat for value k : " << k << '\n';
            print_vertex_cover(litV, V, k, solver1, 1);
            found = true;
            cnf_sat_flag_break = true;
            cnv_sat.notify_all();
            break;
        }
        solver1.reset(new Minisat::Solver());
    }
    if (cnf_sat_flag_break) return;
    if (!found) std::cerr << "Error: vertex cover not found\n";
}

void print_vertex_cover(Minisat::vec<Minisat::Lit> litV[], int V, int ans_k,
                        std::unique_ptr<Minisat::Solver> &solver, int methodNumber) {
    std::vector<int> ans;
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < ans_k; j++) {
            if (solver->modelValue(litV[i][j]) == Minisat::l_True) {
                globalVertexCover[methodNumber].push_back(i);
            }
        }
    }
}

void every_edge_covered(Minisat::vec<Minisat::Lit> litV[], int n, int m, std::unique_ptr<Minisat::Solver> &solver,
                        std::vector<std::pair<int, int>> &edges) {
    Minisat::vec<Minisat::Lit> lit_vector;
    for (std::pair<int, int> p: edges) {
        int x = p.first;
        int y = p.second;
        lit_vector.clear();
        for (int i = 0; i < m; i++) {
            lit_vector.push(litV[x][i]);
            lit_vector.push(litV[y][i]);
        }
        solver->addClause_(lit_vector);
    }
}

void no_vertex_twice(Minisat::vec<Minisat::Lit> litV[], int n, int m,
                     std::unique_ptr<Minisat::Solver> &solver) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            for (int k = 0; k < m; k++) {
                if (j < k) {
                    solver->addClause(~litV[i][j], ~litV[i][k]);
                }
            }
        }
    }
}

void no_more_than_one_on_a_single_position(Minisat::vec<Minisat::Lit> litV[], int n, int m,
                                           std::unique_ptr<Minisat::Solver> &solver) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                if (j < k) {
                    solver->addClause(~litV[j][i], ~litV[k][i]);
                }
            }
        }
    }
}

void at_least_one_vertex(Minisat::vec<Minisat::Lit> litV[], int n, int m, std::unique_ptr<Minisat::Solver> &solver) {
    Minisat::vec<Minisat::Lit> lit_vector;
    for (int i = 0; i < m; i++) {
        lit_vector.clear();
        for (int j = 0; j < n; j++) {
            lit_vector.push(litV[j][i]);
        }
        solver->addClause_(lit_vector);
    }
}

void initialize_literal(Minisat::vec<Minisat::Lit> litV[], int n, int m, std::unique_ptr<Minisat::Solver> &solver) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            litV[i].push(Minisat::mkLit(solver->newVar()));
        }
    }
}

/**
 * 3 CNF Implementation
 */
void find_vertex_cover_3cnf(int V, std::vector<std::pair<int, int>> &edges) {
    bool found = false;
    solver2.reset(new Minisat::Solver());
    for (int k = 0; k <= V; k++) {
        if (three_cnf_sat_flag_break) break;
        Minisat::vec<Minisat::Lit> litV[V];
        // reduction steps
        initialize_literal_3cnf(litV, V, k, solver2);
        at_least_one_vertex_3cnf(litV, V, k, solver2);
        no_vertex_twice_3cnf(litV, V, k, solver2);
        every_edge_covered_3cnf(litV, V, k, solver2, edges);
        no_more_than_one_on_a_single_position_3cnf(litV, V, k, solver2);
        log(0) << "Going to call solver from 3cnf " << k << '\n';
        int res = solver2->solve();
        log(0) << "res from solver 3cnf " << res << " k: " << k << '\n';
        if (res == 1) { //SAT
            log(0) << "3cnf sat for value k : " << k << '\n';
            print_vertex_cover_3cnf(litV, V, k, solver2);
            found = true;
            three_cnf_sat_flag_break = true;
            cnv_sat_3.notify_all();
            break;
        }
        //reset before next try
        solver2.reset(new Minisat::Solver());
    }
    if (three_cnf_sat_flag_break) return;
    if (!found) std::cerr << "Error: vertex cover not found\n";
}

void print_vertex_cover_3cnf(Minisat::vec<Minisat::Lit> litV[], int V, int ans_k,
                             std::unique_ptr<Minisat::Solver> &solver) {
    std::vector<int> ans;
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < ans_k; j++) {
            if (solver->modelValue(litV[i][j]) == Minisat::l_True) {
                ans.push_back(i);
            }
        }
    }
    sort(ans.begin(), ans.end());
    int ans_size = ans.size();
    for (int i = 0; i < ans_size; i++) {
        globalVertexCover[2].push_back(ans[i]);
    }
}

void every_edge_covered_3cnf(Minisat::vec<Minisat::Lit> litV[], int n, int m, std::unique_ptr<Minisat::Solver> &solver,
                             std::vector<std::pair<int, int>> &edges) {
    for (std::pair<int, int> p: edges) {
        Minisat::vec<Minisat::Lit> lit_vector;
        int x = p.first;
        int y = p.second;
        for (int i = 0; i < m; i++) {
            lit_vector.push(litV[x][i]);
            lit_vector.push(litV[y][i]);
        }
        if (lit_vector.size() <= 2) {
            solver->addClause(lit_vector);
            continue;
        }
        Minisat::Lit litCurrent = Minisat::mkLit(solver->newVar());
        solver->addClause(lit_vector[0], lit_vector[1], litCurrent);
        int lit_vector_size = lit_vector.size();
        Minisat::vec<Minisat::Lit> lit_vector_last;
        lit_vector_last.push(litCurrent);
        for (int i = 2; i < lit_vector_size; i++) {
            Minisat::Lit litCurrent2 = Minisat::mkLit(solver->newVar());
            if (i + 2 >= lit_vector_size) {
                solver->addClause(lit_vector[i], lit_vector[i + 1], ~lit_vector_last.last());
                break;
            } else {
                solver->addClause(lit_vector[i], litCurrent2, ~lit_vector_last.last());
            }
            lit_vector_last.push(litCurrent2);
        }
    }
}

void no_vertex_twice_3cnf(Minisat::vec<Minisat::Lit> litV[], int n, int m,
                          std::unique_ptr<Minisat::Solver> &solver) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            for (int k = 0; k < m; k++) {
                if (j < k) {
                    solver->addClause(~litV[i][j], ~litV[i][k]);
                }
            }
        }
    }
}

void no_more_than_one_on_a_single_position_3cnf(Minisat::vec<Minisat::Lit> litV[], int n, int m,
                                                std::unique_ptr<Minisat::Solver> &solver) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                if (j < k) {
                    solver->addClause(~litV[j][i], ~litV[k][i]);
                }
            }
        }
    }
}

void
at_least_one_vertex_3cnf(Minisat::vec<Minisat::Lit> litV[], int n, int m, std::unique_ptr<Minisat::Solver> &solver) {
    Minisat::vec<Minisat::Lit> lit_vector;
    for (int i = 0; i < m; i++) {
        lit_vector.clear();
        for (int j = 0; j < n; j++) {
            lit_vector.push(litV[j][i]);
        }
        if (lit_vector.size() <= 2) {
            solver->addClause(lit_vector);
            continue;
        }
        Minisat::Lit litCurrent = Minisat::mkLit(solver->newVar());
        solver->addClause(lit_vector[0], lit_vector[1], litCurrent);
        int lit_vector_size = lit_vector.size();
        Minisat::vec<Minisat::Lit> lit_vector_last;
        lit_vector_last.push(litCurrent);
        for (int i = 2; i < lit_vector_size; i++) {
            Minisat::Lit litCurrent2 = Minisat::mkLit(solver->newVar());
            if (i + 2 >= lit_vector_size) {
                solver->addClause(lit_vector[i], lit_vector[i + 1], ~lit_vector_last.last());
                break;
            } else {
                solver->addClause(lit_vector[i], litCurrent2, ~lit_vector_last.last());
            }
            lit_vector_last.push(litCurrent2);
        }
    }
}

void
initialize_literal_3cnf(Minisat::vec<Minisat::Lit> litV[], int n, int m, std::unique_ptr<Minisat::Solver> &solver) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            litV[i].push(Minisat::mkLit(solver->newVar()));
        }
    }
}

void clear_top() {
    for (int i = 0; i < 7; i++) {
        elapsed_times[i].clear();
        appx_ratio[i].clear();
    }

}

void clear_everything() {
    globalEdges.clear();
    globalVertices = 0;
    for (int i = 0; i < 7; i++) globalVertexCover[i].clear();
}

void clear_everything_within_10_runs() {
    for (int i = 0; i < 7; i++) globalVertexCover[i].clear();
    approx_vc1_ans.clear();
    approx_vc2_ans.clear();
    refined_vc1_ans.clear();
    refined_vc2_ans.clear();
    ready1 = false;
    ready2 = false;
    cnf_sat_flag_break = false;
    three_cnf_sat_flag_break = false;
}

/**
 * This part covers reading the input
 */
std::vector<std::pair<int, int>> get_parsed_edges(std::istringstream &input, int V, bool &is_correct) {
    std::vector<std::pair<int, int>> edges;
    char c = '#', sep = ',';
    while (!input.eof()) {
        if (c == '<') {
            int num1 = 0, num2 = 0;
            input >> num1;
            input >> c;
            if (c == sep) {
                input >> num2;
            }
            edges.emplace_back(num1 - indexing, num2 - indexing);
        }
        input >> c;
    }
    for (std::pair<int, int> p: edges) {
        if (p.second > V || p.first > V || p.second < 0 || p.first < 0 || (p.first == p.second)) {
            is_correct = false;
            return {};
        }
    }
    return edges;
}