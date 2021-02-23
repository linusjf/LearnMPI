#include <mpi.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <numeric>
#include <cassert>
#include <chrono>

using namespace std::chrono;

using container = std::vector<int>;

//const size_t ELEMENTS_PER_PROCESS = 100000;
const size_t ELEMENTS_PER_PROCESS = 10000;

const int ROOT_RANK = 0;


container generate_array(size_t size) 
{
    container c (size);
    std::generate(c.begin(), c.end(), [](void){ return std::rand() % 100;} );
   // return std::move(c);
   return c;
}

void sequential_sort(container arrayToSort)
{
  int size = arrayToSort.size();
    int sorted = 0;
    while (sorted == 0)
    {
        sorted = 1;
        int i;
        for (i = 1; i < size - 1; i += 2)
        {
            if (arrayToSort[i] > arrayToSort[i + 1])
            {
                int temp = arrayToSort[i + 1];
                arrayToSort[i + 1] = arrayToSort[i];
                arrayToSort[i] = temp;
                sorted = 0;
            }
        }
        for (i = 0; i < size - 1; i += 2)
        {
            if (arrayToSort[i] > arrayToSort[i + 1])
            {
                int temp = arrayToSort[i + 1];
                arrayToSort[i + 1] = arrayToSort[i];
                arrayToSort[i] = temp;
                sorted = 0;
            }
        }
    }
}

std::string array_to_string(const container& c) 
{
    std::stringstream ss;
    for(auto n : c) 
        ss << n << "  ";
    return ss.str();
}

void compare_split_min(int cmp_proc_id, container& sub_array) 
{
    int proc_count;
    MPI_Comm_size(MPI_COMM_WORLD, &proc_count);

    int proc_id;
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);

    assert(cmp_proc_id < proc_count && "Wrong proc id");
    assert(cmp_proc_id >= 0 && "Wrong proc id");
    assert(cmp_proc_id != proc_id && "Wrong proc id");

    container other_sub_array(ELEMENTS_PER_PROCESS);

    //get another process array. 
    //Lesser proc_id is always initiator of exchange 
    // std::cout << "#" << proc_id << " send array to " << cmp_proc_id << std::endl;
    MPI_Send(&sub_array[0], sub_array.size(), MPI_INT, 
             cmp_proc_id, 0, MPI_COMM_WORLD);
    
    // std::cout << "#" << proc_id << " wait for array from " << cmp_proc_id << std::endl;

    MPI_Recv(&other_sub_array[0], other_sub_array.size(), MPI_INT, 
             cmp_proc_id, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    //merge array, but store only its lesser part
    //TODO: implement more sophisticated algorithm
    container merged;
    merged.reserve(sub_array.size() + other_sub_array.size());

//    std::merge( sub_array.begin(), sub_array.end(), 
  //              other_sub_array.begin(), other_sub_array.end(), 
    //            std::back_inserter(merged));

    std::set_union(sub_array.begin(), 
        sub_array.end(), 
        other_sub_array.begin(), 
        other_sub_array.end(), 
        merged.begin());

    sequential_sort(merged);

    for(int i = 0; i < sub_array.size(); i++) 
        sub_array[i] = merged[i];
}

void compare_split_max(int cmp_proc_id, container& sub_array) 
{
    int proc_count;
    MPI_Comm_size(MPI_COMM_WORLD, &proc_count);

    int proc_id;
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);

    assert(cmp_proc_id < proc_count && "Wrong proc id");
    assert(cmp_proc_id >= 0 && "Wrong proc id");
    assert(cmp_proc_id != proc_id && "Wrong proc id");

    container other_sub_array(ELEMENTS_PER_PROCESS);

    // std::cout << "#" << proc_id << " wait for array from " << cmp_proc_id << std::endl;
    //get another process array. 
    //Lesser is always initiator of exchange 
    MPI_Recv(&other_sub_array[0], other_sub_array.size(), MPI_INT, 
             cmp_proc_id, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    // std::cout << "#" << proc_id << " send array to " << cmp_proc_id << std::endl;
    
    MPI_Send(&sub_array[0], sub_array.size(), MPI_INT, 
             cmp_proc_id, 0, MPI_COMM_WORLD);
    
    //merge array, but store only its higher part
    //TODO: implement more sophisticated algorithm
    container merged;
    merged.reserve(sub_array.size() + other_sub_array.size());
    
    //std::merge( sub_array.begin(), sub_array.end(),
      //          other_sub_array.begin(), other_sub_array.end(), 
        //        std::back_inserter(merged));
    std::set_union(sub_array.begin(), 
        sub_array.end(), 
        other_sub_array.begin(), 
        other_sub_array.end(), 
        merged.begin());

    sequential_sort(merged);

    for(int i = other_sub_array.size(); i < merged.size() ; i++) {
        auto idx_in_sub_arr = i - other_sub_array.size();
        sub_array[idx_in_sub_arr] = merged[i];
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int proc_id;
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);

    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    container array;
    container array_copy;

    if (proc_id == ROOT_RANK) {
        //array = std::move(generate_array(ELEMENTS_PER_PROCESS * world_size));
        array = generate_array(ELEMENTS_PER_PROCESS * world_size);
        array_copy = array;
        // std::cout << array_to_string(array) << std::endl;
    }

    auto start_time = std::chrono::high_resolution_clock::now();

    container sub_array(ELEMENTS_PER_PROCESS);

    //devide data to subprocesses (ROOT also perform computing)
    MPI_Scatter(&array[0], ELEMENTS_PER_PROCESS, MPI_INT,
                &sub_array[0], ELEMENTS_PER_PROCESS, MPI_INT,
                0, MPI_COMM_WORLD);
    // std::cout << "#" << proc_id << ", sub_array: " << array_to_string(sub_array) << std::endl;

    //sort
    std::sort(sub_array.begin(), sub_array.end(), std::less<container::value_type>());
    //std::cout << "#" << proc_id << ", sorted sub_array: " << array_to_string(sub_array) << std::endl;

    //interprocess_sort()
    for(int i = 0; i < world_size; i++) {
        //odd iteration (p1, p2), (p3, p4)
        if( i % 2 == 1) {
            //odd process id
            if( proc_id % 2 == 1 ) {
                if(proc_id < world_size -1) {
                    //compare this process with right one.  save lesser part of the joined array.
                    compare_split_min(proc_id + 1, sub_array);
                }
            }
            else if (proc_id > 0) {
                //compare this process with left one. save greater part of the joined array.
                compare_split_max(proc_id - 1, sub_array);
            }
            // std::cout << "#" << proc_id << ", Odd exchange complete, i : " << i << std::endl;
        }
        //even iteration (p0, p1), (p2, p3)
        else {
            //even proc id
            if( proc_id % 2 == 0 ) {
                if(proc_id < world_size -1) {
                    //compare this process with right one. save lesser part of the joined array.
                    compare_split_min(proc_id + 1, sub_array);
                }
            }
            else if (proc_id > 0)  {
                //compare this process with left one. save greater part of the joined array.
                compare_split_max(proc_id - 1, sub_array);
            }
            // std::cout << "#" << proc_id << ", Even exchange complete, i : " << i << std::endl;
        }
    }

    MPI_Gather( &sub_array[0], ELEMENTS_PER_PROCESS, MPI_INT, 
                &array[0], ELEMENTS_PER_PROCESS, MPI_INT, 
                0, MPI_COMM_WORLD);

    auto end_time = std::chrono::high_resolution_clock::now();

    if(proc_id == ROOT_RANK) {
        // std::cout << "MPI sort: " << std::endl  << array_to_string(array) << std::endl;
        std::cout << "Time for parallel sorting : " << duration_cast<std::chrono::milliseconds>(end_time-start_time).count() << "ms" << std::endl;

        start_time = std::chrono::high_resolution_clock::now();
        sequential_sort(array_copy);
      ///  std::sort(array_copy.begin(), array_copy.end(), std::less<container::value_type>());
        end_time = std::chrono::high_resolution_clock::now();
        std::cout << "Time for standard sorting : " << duration_cast<std::chrono::milliseconds>(end_time-start_time).count() << "ms" << std::endl;
    }
    MPI_Finalize();
}
