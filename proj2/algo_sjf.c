#include "scheduler.h"

static int pick_next_sjf(Process *p, int n, int now) {
    int best = -1;
    for (int i = 0; i < n; i++) {
        if (p[i].remaining_time <= 0) continue;          // finished
        if (p[i].arrival_time > now) continue;           // not arrive

        if (best == -1) {
            best = i;
            continue;
        }

        // SJF：Processes with less remaining time will be given priority.
        if (p[i].remaining_time < p[best].remaining_time) {
            best = i;
        } else if (p[i].remaining_time == p[best].remaining_time) {
            // tie-break : First-Come First-Served, then sort by id.
            if (p[i].arrival_time < p[best].arrival_time) best = i;
            else if (p[i].arrival_time == p[best].arrival_time && p[i].id < p[best].id) best = i;
        }
    }
    return best;
}

void run_SJF(Process *processes, int process_count) {
    int t = 0; // current time quantum

    while (t < TOTAL_QUANTA) {
        int idx = pick_next_sjf(processes, process_count, t);

        // no ready process : CPU idle, jump to next arrival time
        if (idx == -1) {
            int next_arrival = 1e9;
            for (int i = 0; i < process_count; i++) {
                if (processes[i].remaining_time > 0 && processes[i].arrival_time < next_arrival) {
                    next_arrival = processes[i].arrival_time;
                }
            }
            if (next_arrival == 1e9 || next_arrival >= TOTAL_QUANTA) break;
            if (next_arrival <= t) { t++; }  // Prevent infinite loops
            else t = next_arrival;
            continue;
        }

        Process *cur = &processes[idx];

        // first get cpu : record start_time
        if (cur->start_time < 0) cur->start_time = t;

        // Non-preemptive : run until completion, or reach TOTAL_QUANTA
        while (t < TOTAL_QUANTA && cur->remaining_time > 0) {
            cur->history[t] = true;      // run in this quantum
            cur->remaining_time--;       // consume one quantum
            t++;

            if (cur->remaining_time == 0) {
                cur->finish_time = t;    
                break;
            }
        }
    }
}
