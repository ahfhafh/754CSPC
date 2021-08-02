#include <iostream>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <string>
#include <utility>
#include <algorithm>
#include <deque>

std::string scheduler;
std::vector<std::string> p_id;
std::vector<std::string> p_order;
bool record_idle = false;

struct Process
{
    int id;
    int64_t arrival;
    int64_t burst;

    bool operator<(Process const &p) { return burst < p.burst; }
};
std::vector<Process> processes;
std::deque<Process> RQ;
struct Process OnCPU;

int64_t timeslice;
int64_t jobs_remaining;
bool cpu_idle = true;

void simulate_SJF()
{
    int64_t curr_time = 0;
    jobs_remaining = processes.size();
    std::cout << "Seq: ";
    while (1)
    {
        if (jobs_remaining == 0)
            break;
        //if process in cpuis done
        if (OnCPU.burst == 0 && cpu_idle == false)
        {
            std::cout << "A\n";
            //mark process done
            OnCPU = {0};
            //set CPU idle
            cpu_idle = true;
            //jobs_remaining--
            jobs_remaining--;
            continue;
        }

        //if a new process arriving
        for (int64_t i = 0; i < (int)processes.size(); i++)
        {
            if (processes[i].arrival == curr_time)
            {
                std::cout << "B\n";
                //add new process to RQ
                RQ.push_back(processes[i]);
                //std::cout << "Processes arriving--> " << processes[i].id << " arrival time ---> " << processes[i].arrival << "\n";
            }
        }
        //if cpuis idle and RQ not empty
        if (cpu_idle == true && !RQ.empty())
        {
            std::cout << "C\n";
            //move process from RQ to CPU
            std::sort(RQ.begin(), RQ.end());
            OnCPU = RQ.front();
            cpu_idle = false;
            RQ.pop_front();
            //record process being worked on
            p_order.push_back("P" + std::to_string(OnCPU.id) + ",");
            record_idle = false;
        }
        else if (cpu_idle == true && RQ.empty() && record_idle == false)
        {
            record_idle = true;
            p_order.push_back("-,");
        }
        //execute one burst of job on CPU
        OnCPU.burst--;
        //std::cout << "OnCPU burst = " << OnCPU.burst;
        curr_time++;
        //std::cout << "curr time = " << curr_time;
    }

    // print processes sequence
    for (const auto &p : p_order)
    {
        std::cout << p.c_str();
    }
    std::cout << "\n";
}

void simulate_RR(int timeslice)
{
    int64_t curr_time = 0;
    int64_t curr_timeslice = 0;
    std::string p_name;
    jobs_remaining = processes.size();
    std::cout << "Seq: ";
    while (1)
    {
        if (jobs_remaining == 0)
            break;
        //if process in cpuis done
        if (OnCPU.burst == 0 && cpu_idle == false)
        {
            //mark process done
            OnCPU = {0};
            //set CPU idle
            cpu_idle = true;
            //jobs_remaining--
            jobs_remaining--;
            continue;
        }
        else if (OnCPU.burst != 0 && cpu_idle == false && curr_timeslice == timeslice)
        {
            //move processes back to RQ
            RQ.push_back(OnCPU);
            cpu_idle = true;
        }

        //if a new process arriving
        for (int64_t i = 0; i < (int)processes.size(); i++)
        {
            if (processes[i].arrival == curr_time)
            {
                //add new process to RQ
                RQ.push_back(processes[i]);
                //std::cout << "Processes arriving--> " << processes[i].id << " arrival time ---> " << processes[i].arrival << "\n";
            }
        }
        //if cpuis idle and RQ not empty
        if (cpu_idle == true && !RQ.empty())
        {
            //move process from RQ to CPU
            OnCPU = RQ.front();
            cpu_idle = false;
            RQ.pop_front();
            curr_timeslice = 0;
            //record process being worked on
            p_name = "P" + std::to_string(OnCPU.id) + ",";
            if (p_order.back() != p_name)
                p_order.push_back(p_name);
            record_idle = false;
        }
        else if (cpu_idle == true && RQ.empty() && record_idle == false)
        {
            record_idle = true;
            p_order.push_back("-,");
        }
        //execute one burst of job on CPU
        OnCPU.burst--;
        //std::cout << "OnCPU burst = " << OnCPU.burst;
        curr_time++;
        //std::cout << "curr time = " << curr_time;
        curr_timeslice++;
    }

    // print processes sequence
    for (const auto &p : p_order)
    {
        std::cout << p.c_str();
    }
    std::cout << "\n";
}

int main(int argc, char **argv)
{
    if (argc != 3 && argc != 4)
    {
        std::cout << "Usage: " << argv[0] << " [config.txt] [RR or SJF] [timeslice]\n";
        exit(-1);
    }

    scheduler = argv[2];
    if (scheduler != "RR" && scheduler != "rr" && scheduler != "SJF" && scheduler != "sjf")
    {
        std::cout << "scheduler should be either [RR] or [SJF]\n";
        exit(-1);
    }

    if (scheduler == "SJF" && argc == 4)
    {
        std::cout << "SJF does not accept [timeslice]\n";
        exit(-1);
    }
    else if (scheduler == "RR" && argc == 4)
    {
        timeslice = atoi(argv[3]);
        if (timeslice == 0)
        {
            std::cout << "[timeslice] needs to be an interger greater than 0\n";
            exit(-1);
        }
    }
    else if (scheduler == "RR" && argc != 4)
    {
        std::cout << "RR needs [timeslice]\n";
        exit(-1);
    }

    // file i/o
    std::ifstream infile(argv[1]);
    int64_t a, b;
    int i = 0;
    while (infile >> a >> b)
    {
        processes.push_back({i, a, b});
        i++;
    }

    if (scheduler == "SJF" || scheduler == "sjf")
        simulate_SJF();

    if (scheduler == "RR" || scheduler == "rr")
        simulate_RR(timeslice);
}
