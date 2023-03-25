#include <valarray>
#include <bits/stdc++.h>

using namespace std;

struct Args {
    map<int, vector<int> >* collection;
    pthread_barrier_t* barrier;
    pthread_mutex_t* mutex;
    int M_threads, R_threads, num_files, pos;
    vector<string> files;
};

struct ids {
    Args* args;
    int id_thread;
};

double fast_power(double base, int pow) {
    double res = 1;
    while (pow > 0) {

        if (pow % 2 == 1) {
            res = (res * base);
        }
        base = (base * base);
        pow = pow / 2;
    }
    return res;
}

double newton_approximation(int n, int pow) {

    double x = 2;
    double r = 1;
    while (abs(r) > 0.05) {
        r = (fast_power(x, pow) - n) / (pow * fast_power(x, pow - 1));
        x = x - (fast_power(x, pow) - n) / (pow * fast_power(x, pow - 1));
    }
    return x;
}

unsigned long long quick(unsigned long long a, unsigned long long b)
{
    if (b == 0)
        return 1LL;

    if (b % 2 == 0)
        return quick(a * a, b / 2);
    else
        return a * quick(a * a, b / 2);
}

void* add_map(void* arg) {
    ids* values = (ids*)arg;

    string file_name;
    ifstream file;
    int file_numbers, num;

    while (values->args->pos < (values->args->num_files)) {

        pthread_mutex_lock(values->args->mutex);
        file_name = values->args->files[values->args->pos];
        values->args->pos++;
        pthread_mutex_unlock(values->args->mutex);

        file.open(file_name);
        file >> file_numbers;

        for (int j = 1; j <= file_numbers; j++) {

            file >> num;

            if (num == 0)
                continue;

            for (int i = 2; i <= values->args->R_threads + 1; i++) {
                int aux = round(newton_approximation(num, i));

                if (num == 1 || quick(aux, i) == num || quick(aux - 1, i) == num || quick(aux + 1, i) == num) {
                
                    pthread_mutex_lock(values->args->mutex);
                    (*values->args->collection)[i].push_back(num);
                    pthread_mutex_unlock(values->args->mutex);
                }
            }
        }
        file.close();
    }

    pthread_barrier_wait(values->args->barrier);
    return nullptr;
}

void* reduce(void* arg) {
    ids* values = (ids*)arg;

    pthread_barrier_wait(values->args->barrier);

    int id = (values->id_thread - values->args->M_threads) + 2;

    // sort and clear arrays
    sort((*values->args->collection)[id].begin(), (*values->args->collection)[id].end());
    (*values->args->collection)[id].erase(unique((*values->args->collection)[id].begin(), (*values->args->collection)[id].end()), (*values->args->collection)[id].end());

    string output_name = "out" + to_string(values->id_thread - values->args->M_threads + 2) + ".txt";
    ofstream file(output_name);

    file << (*values->args->collection)[id].size();
    file.close();
    return nullptr;
}

int main(int argc, char** argv)
{
    int M_threads, R_threads;

    // init collection
    map<int, vector<int> > map;

    // read number of threads
    M_threads = stoi(argv[1]);
    R_threads = stoi(argv[2]);
    string name_file_console(argv[3]);

    // init threads
    pthread_t threads_M[M_threads];
    pthread_t threads_R[R_threads];

    ids arguments[M_threads + R_threads];
    Args arg;
    arg.M_threads = M_threads;
    arg.R_threads = R_threads;
    arg.collection = &map;

    int chk;
    void* status;

    // init barrier
    pthread_barrier_t barrier;
    pthread_barrier_init(&barrier, nullptr, M_threads + R_threads);
    arg.barrier = &barrier;

    // init mutex
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, nullptr);
    arg.mutex = &mutex;

    // read test.txt
    ifstream my_test_file;
    my_test_file.open(name_file_console);
    my_test_file >> arg.num_files;

    arg.pos = 0;

    for (int i = 0; i < arg.num_files; i++) {
        string file;
        my_test_file >> file;
        arg.files.push_back(file);
    };

    for (int i = 0; i < M_threads + R_threads; i++)
        arguments[i].args = &arg;

    // call collection threads
    for (int i = 0; i < M_threads; i++) {

        arguments[i].id_thread = i;
        chk = pthread_create(&threads_M[i], nullptr, add_map, &arguments[i]);

        if (chk) {
            printf("Eroare la crearea thread-ului %d\n", i);
            exit(-1);
        }
    }

    // call reduce threads
    for (int i = M_threads; i < M_threads + R_threads; i++) {

        arguments[i].id_thread = i;
        chk = pthread_create(&threads_R[i], nullptr, reduce, &arguments[i]);

        if (chk) {
            printf("Eroare la crearea thread-ului %d\n", i);
            exit(-1);
        }
    }

    for (int i = 0; i < M_threads; i++) {
        chk = pthread_join(threads_M[i], &status);

        if (chk) {
            printf("Eroare la asteptarea thread-ului %d\n", i);
            exit(-1);
        }
    }

    for (int i = M_threads; i < M_threads + R_threads; i++) {
        pthread_join(threads_R[i], &status);

        if (chk) {
            printf("Eroare la asteptarea thread-ului %d\n", i);
            exit(-1);
        }
    }

    pthread_barrier_destroy(arg.barrier);
    pthread_mutex_destroy(arg.mutex);

    return 0;
}