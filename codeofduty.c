#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define INPUT_FILE "input.txt"
#define OUTPUT_FILE "output.txt"

typedef struct {
    int size;
    int total;
    int data[];
} Vector;
typedef Vector * VectorRef;

typedef struct _VectorsList VectorsList;
typedef VectorsList * VectorsListRef;
struct _VectorsList {
    VectorRef vector;
    VectorsListRef next;
};

static void 
read_line(FILE * input)
{
    while (fgetc(input) != '\n');
}

static int
read_int(FILE * input)
{
    int n = 0;
    char current;
    while ((current = fgetc(input)) != '\n' && current != ' ')
        n = n * 10 + current - '0';
    return n;
}

static void
read_data(FILE * input, VectorRef vector)
{
    for (int i = 0; i < vector->size; ++i) {
        int cur_val = read_int(input);
        vector->data[i] = cur_val;
        vector->total += cur_val;
    }
}

static VectorRef
read_vector(FILE * input)
{
    int cur_size = read_int(input);
    if (cur_size == 0)
        return NULL;
    VectorRef vector = (VectorRef) malloc(sizeof(Vector) + cur_size * sizeof(int));
    vector->size = cur_size;
    vector->total = 0;
    read_data(input, vector);
    read_line(input);
    return vector;
}

static VectorRef
copy_vector(VectorRef in)
{
    VectorRef out = (VectorRef) malloc(sizeof(Vector) + in->size * sizeof(int));
    out->size = in->size;
    out->total = in->total;
    for (int i = 0; i < in->size; ++i)
        out->data[i] = in->data[i];
    return out;
}

static bool
deal_with_next_vector(FILE * input, FILE * output)
{
    VectorRef vector = read_vector(input);
    if (vector == NULL)
        return false;
    if (vector->total % vector->size)
    {
        fprintf(output, "-1\n\n");
        goto out;
    }
    int value_needed = (vector->total / vector->size);
    int good_ones;
    VectorsListRef history = (VectorsListRef) malloc(sizeof(VectorsList));
    history->vector = copy_vector(vector);
    VectorsListRef prev = history;
    int iterations = 0;
    do
    {
        bool anything_changed = false;
        int left_balance = vector->data[0] - value_needed;
        if (left_balance > 0)
        {
            --vector->data[0];
            ++vector->data[1];
            --left_balance;
            anything_changed = true;
        }
        good_ones = (vector->data[0] == value_needed) ? 1 : 0;
        for (int i = 1; i < vector->size; ++i)
        {
            int old_balance = left_balance;
            left_balance += (vector->data[i] - value_needed);
            if (vector->data[i] > 0)
            {
                if (old_balance < 0)
                {
                    ++vector->data[i-1];
                    --vector->data[i];
                    if (vector->data[i-1] == value_needed)
                        ++good_ones;
                    anything_changed = true;
                }
                else if (left_balance > 0)
                {
                    ++vector->data[i+1];
                    --vector->data[i];
                    --left_balance;
                    anything_changed = true;
                }
            }
            if (vector->data[i] == value_needed)
                ++good_ones;
        }
        if (anything_changed)
        {
            VectorsListRef current = (VectorsListRef) malloc(sizeof(VectorsList));
            current->vector = copy_vector(vector);
            prev->next = current;
            prev = current;
            ++iterations;
        }
    } while (good_ones != vector->size);
    prev->next = NULL;
    fprintf(output, "%d\n", iterations);
    VectorsListRef current = history;
    for (int i = 0; current != NULL; ++i)
    {
        VectorRef v = current->vector;
        prev = current;
        current = prev->next;
        free(prev);
        fprintf(output, "%d : (%d", i, v->data[0]);
        for (int j = 1; j < v->size; ++j)
            fprintf(output, ", %d", v->data[j]);
        fprintf(output, ")\n");
        free(v);
    }
    fprintf(output, "\n");
out:
    free(vector);
    return true; 
}

int
main()
{
    const char * error = NULL;
    FILE * input = fopen(INPUT_FILE, "r");
    if (input == NULL) {
        error = "Could not open file: " INPUT_FILE;
        goto error;
    }
    FILE * output = fopen(OUTPUT_FILE, "w");
    if (output == NULL) {
        error = "Could not open file: " OUTPUT_FILE;
        goto error;
    }
    while (deal_with_next_vector(input, output));
    fclose(input);
    fclose(output);
    return EXIT_SUCCESS;
error:
    fprintf(stderr, "%s\n", error);
    return EXIT_FAILURE;
}
