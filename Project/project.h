int gillespie_simulation(int *x0, int *x, double* times);
void update_state(int* x, int reaction_index);
void print_histogram(int* bins, int* bin_counts, int nr_bins, int min, int max);
void write_file(char* filename, int* bins, int* bin_counts, int nr_bins, int min, int max, int N, double time);