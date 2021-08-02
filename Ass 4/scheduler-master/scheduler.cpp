/**********************************************
 * Last Name:   Yang
 * First Name:  Jack
 * Student ID:  30062393
 * Course:      CPSC 457
 * Tutorial:    T05
 * Assignment:  4
 *
 * File name: scheduler.cpp
 *********************************************/
#include <iostream>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <string>
#include <utility>
#include <algorithm>
#include <deque>
#include <stdlib.h>

struct Process
{
    int id;
    int64_t arrival;
    int64_t burst;

    bool operator<(Process const &p) { return burst < p.burst; }
};

std::string scheduler;
std::vector<Process> processes;
int64_t timeslice;

void simulate_SJF()
{
    int64_t curr_time = 0;
    int64_t last_curr_time = curr_time;
    int64_t jobs_remaining = processes.size();
    bool cpu_idle = true;
    std::deque<Process> RQ;
    std::vector<std::string> p_order;
    struct Process OnCPU = {0};

    std::cout << "Seq: ";
    while (1)
    {
        if (jobs_remaining == 0 || p_order.size() >= 100)
            break;

        //if process in cpuis done
        if (OnCPU.burst == 0 && cpu_idle == false)
        {
            //std::cout << "A\n";
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
            if (last_curr_time == curr_time && processes[i].arrival == 0)
            {
                RQ.push_back(processes[i]);
                //std::cout << "Process arriving--> " << processes[i].id << " arrival time ---> " << processes[i].arrival << "\n";
            }
            else if (last_curr_time < processes[i].arrival && processes[i].arrival <= curr_time)
            {
                //std::cout << "B\n";
                //add new process to RQ
                RQ.push_back(processes[i]);
                //std::cout << "Process arriving--> " << processes[i].id << " arrival time ---> " << processes[i].arrival << "\n";
            }
        }

        //if cpuis idle and RQ not empty
        if (cpu_idle == true && !RQ.empty())
        {
            //std::cout << "C\n";
            //move process from RQ to CPU
            std::sort(RQ.begin(), RQ.end());
            OnCPU = RQ.front();
            cpu_idle = false;
            RQ.pop_front();
            //record process being worked on
            p_order.push_back("P" + std::to_string(OnCPU.id) + ",");
        }
        else if (cpu_idle == true && RQ.empty())
        {
            p_order.push_back("-,");

            for (int i = 0; i < (int)processes.size(); i++)
            {
                if (processes[i].arrival > curr_time)
                {
                    // increment curr_time to next arrival time
                    curr_time += processes[i].arrival - curr_time - 1;
                    break;
                }
            }
        }

        //execute current process n burst of job on CPU (increment by burst)
        if (cpu_idle == false)
        {
            // record last curr_time before increment
            last_curr_time = curr_time;
            curr_time += OnCPU.burst;
            OnCPU.burst -= OnCPU.burst;
        }
        else // increment by 1
            curr_time++;

        /*
        std::cout << "curr time = " << curr_time << "\n";
        std::cout << "OnCPU burst = " << OnCPU.burst << "\n";
        */
    }

    // print processes sequence
    for (const auto &p : p_order)
    {
        std::cout << p.c_str();
    }
    std::cout << "\n";
}

void simulate_RR(int64_t timeslice)
{
    int64_t curr_time = 0;
    int64_t last_curr_time = curr_time;
    int64_t curr_timeslice = 0;
    int64_t jobs_remaining = processes.size();
    std::string p_name;
    bool cpu_idle = true;
    std::deque<Process> RQ;
    std::vector<std::string> p_order;
    struct Process OnCPU;

    std::cout << "Seq: ";
    while (1)
    {
        if (jobs_remaining == 0 || p_order.size() >= 100)
            break;
        //if process in cpuis done
        if (OnCPU.burst == 0 && cpu_idle == false)
        {
            //std::cout << "A\n";
            //mark process done
            OnCPU = {0};
            //set CPU idle
            cpu_idle = true;
            //jobs_remaining--
            jobs_remaining--;
            continue;
        }

        int i = 0;
        //if a new process arriving (last_curr_time < process's arrival time < curr_time)
        for (; i < (int)processes.size(); i++)
        {
            // for arrivals at time 0
            if (last_curr_time == curr_time && processes[i].arrival == 0)
            {
                //std::cout << "B(1)\n";
                RQ.push_back(processes[i]);
                //std::cout << "Process arriving--> " << processes[i].id << " arrival time ---> " << processes[i].arrival << "\n";
            }
            // for arrivals > time 0
            else if (last_curr_time < processes[i].arrival && processes[i].arrival < curr_time)
            {
                //std::cout << "B(2)\n";
                //add new process to RQ
                RQ.push_back(processes[i]);
                //std::cout << "Process arriving--> " << processes[i].id << " arrival time ---> " << processes[i].arrival << "\n";
            }
            else if (processes[i].arrival == curr_time)
                break;
        }
        // timeslice burst time up, move current process back onto RQ
        if (OnCPU.burst != 0 && cpu_idle == false && curr_timeslice == timeslice)
        {
            //move processes back to RQ
            RQ.push_back(OnCPU);
            //set CPU idle
            cpu_idle = true;
            //std::cout << "Process back onto RQ--> " << OnCPU.id << "\n";
        }
        //if a new process arriving (process's arrival time = curr_time)
        for (; i < (int)processes.size(); i++)
        {
            if (processes[i].arrival == curr_time)
            {
                RQ.push_back(processes[i]);
                //std::cout << "Process arriving--> " << processes[i].id << " arrival time ---> " << processes[i].arrival << "\n";
            }
        }

        //if cpuis idle and RQ not empty
        if (cpu_idle == true && !RQ.empty())
        {
            //std::cout << "C\n";
            //move process from RQ to CPU
            OnCPU = RQ.front();
            cpu_idle = false;
            RQ.pop_front();
            curr_timeslice = 0;
            //record process being worked on
            p_name = "P" + std::to_string(OnCPU.id) + ",";
            if (!p_order.empty())
            {
                if (p_order.back() != p_name)
                    p_order.push_back(p_name);
            }
            else
                p_order.push_back(p_name);
        }
        else if (cpu_idle == true && RQ.empty())
        {
            //std::cout << "D\n";
            p_order.push_back("-,");

            // increment by n to nearest arrival time
            for (int y = 0; y < (int)processes.size(); y++)
            {
                if (processes[y].arrival > curr_time)
                {
                    curr_time += processes[y].arrival - curr_time - 1;
                    curr_timeslice += (processes[y].arrival % timeslice) - 1;
                    break;
                }
            }
        }
        //execute current process n burst of job on CPU < timeslice
        if (cpu_idle == false)
        {
            // record last curr_time before increment
            last_curr_time = curr_time;
            // increment by burst instead of timeslice
            if (OnCPU.burst <= timeslice)
            {
                //std::cout << "E\n";
                curr_time += OnCPU.burst;
                OnCPU.burst -= OnCPU.burst;
                curr_timeslice += OnCPU.burst;
            }
            // increment by timeslice instead of burst
            else
            {
                //std::cout << "F\n";
                curr_time += timeslice;
                OnCPU.burst -= timeslice;
                curr_timeslice += timeslice;
            }
        }
        else // increment by 1
        {
            //std::cout << "G\n";
            curr_time++;
            curr_timeslice++;
        }

        //std::cout << "curr time = " << curr_time << "\n";
        //std::cout << "OnCPU burst = " << OnCPU.burst << "\n";
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
        timeslice = strtoll(argv[3], NULL, 10);
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

    // do SJF
    if (scheduler == "SJF" || scheduler == "sjf")
        simulate_SJF();

    // do RR
    if (scheduler == "RR" || scheduler == "rr")
        simulate_RR(timeslice);
}
