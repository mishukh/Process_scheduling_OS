#include <bits/stdc++.h>
#include "parser.h"
#include "visualizer.h"

#define all(v) v.begin(), v.end()

using namespace std;

/** Global Constants **/
const string TRACE = "trace";
const string SHOW_STATISTICS = "stats";
const string ALGORITHMS[9] = {"", "FCFS", "RR-", "SPN", "SRT", "HRRN", "FB-1", "FB-2i", "AGING"};

bool sortByServiceTime(const tuple<string, int, int> &a, const tuple<string, int, int> &b)
{
    return (get<2>(a) < get<2>(b));
}
bool sortByArrivalTime(const tuple<string, int, int> &a, const tuple<string, int, int> &b)
{
    return (get<1>(a) < get<1>(b));
}

bool descendingly_by_response_ratio(tuple<string, double, int> a, tuple<string, double, int> b)
{
    return get<1>(a) > get<1>(b);
}

bool byPriorityLevel (const tuple<int,int,int>&a,const tuple<int,int,int>&b){
    if(get<0>(a)==get<0>(b))
        return get<2>(a)> get<2>(b);
    return get<0>(a) > get<0>(b);
}

void clear_timeline()
{
    for(int i=0; i<last_instant; i++)
        for(int j=0; j<process_count; j++)
            timeline[i][j] = ' ';
}

string getProcessName(tuple<string, int, int> &a)
{
    return get<0>(a);
}

int getArrivalTime(tuple<string, int, int> &a)
{
    return get<1>(a);
}

int getServiceTime(tuple<string, int, int> &a)
{
    return get<2>(a);
}

int getPriorityLevel(tuple<string, int, int> &a)
{
    return get<2>(a);
}

double calculate_response_ratio(int wait_time, int service_time)
{
    return (wait_time + service_time)*1.0 / service_time;
}

struct PerformanceMetrics
{
    double avg_waiting_time;
    double cpu_utilization;
    double throughput;
};

map<string, PerformanceMetrics> algorithmMetrics; // Stores results for each algorithm



void fillInWaitTime(){
    for (int i = 0; i < process_count; i++)
    {
        int arrivalTime = getArrivalTime(processes[i]);
        for (int k = arrivalTime; k < finishTime[i]; k++)
        {
            if (timeline[k][i] != '*')
                timeline[k][i] = '.';
        }
    }
}



void firstComeFirstServe()
{
    sort(all(processes),sortByArrivalTime);

    int time = 0;  // Start from time = 0
    for (int i = 0; i < process_count; i++)
    {
        int processIndex = i;
        int arrivalTime = getArrivalTime(processes[i]);
        int serviceTime = getServiceTime(processes[i]);

        // If CPU is idle, jump to the next process's arrival time
        if (time < arrivalTime)
            time = arrivalTime;

        finishTime[processIndex] = time + serviceTime;
        turnAroundTime[processIndex] = finishTime[processIndex] - arrivalTime;
        normTurn[processIndex] = (turnAroundTime[processIndex] * 1.0 / serviceTime);

        for (int j = time; j < finishTime[processIndex]; j++)
            timeline[j][processIndex] = '*';
        for (int j = arrivalTime; j < time; j++)
            timeline[j][processIndex] = '.';

        time += serviceTime;
    }
}


void roundRobin(int originalQuantum)
{
    sort(all(processes),sortByArrivalTime);
    queue<pair<int,int>>q;
    int j=0;
    if(getArrivalTime(processes[j])==0){
        q.push(make_pair(j,getServiceTime(processes[j])));
        j++;
    }
    int currentQuantum = originalQuantum;
    for(int time =0;time<last_instant;time++){
        if(!q.empty()){
            int processIndex = q.front().first;
            q.front().second = q.front().second-1;
            int remainingServiceTime = q.front().second;
            int arrivalTime = getArrivalTime(processes[processIndex]);
            int serviceTime = getServiceTime(processes[processIndex]);
            currentQuantum--;
            timeline[time][processIndex]='*';
            while(j<process_count && getArrivalTime(processes[j])==time+1){
                q.push(make_pair(j,getServiceTime(processes[j])));
                j++;
            }

            if(currentQuantum==0 && remainingServiceTime==0){
                finishTime[processIndex]=time+1;
                turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
                normTurn[processIndex] = (turnAroundTime[processIndex] * 1.0 / serviceTime);
                currentQuantum=originalQuantum;
                q.pop();
            }else if(currentQuantum==0 && remainingServiceTime!=0){
                q.pop();
                q.push(make_pair(processIndex,remainingServiceTime));
                currentQuantum=originalQuantum;
            }else if(currentQuantum!=0 && remainingServiceTime==0){
                finishTime[processIndex]=time+1;
                turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
                normTurn[processIndex] = (turnAroundTime[processIndex] * 1.0 / serviceTime);
                q.pop();
                currentQuantum=originalQuantum;
            }
        }
        while(j<process_count && getArrivalTime(processes[j])==time+1){
            q.push(make_pair(j,getServiceTime(processes[j])));
            j++;
        }
    }
    fillInWaitTime();
}

void shortestProcessNext()
{
    sort(all(processes),sortByArrivalTime);
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq; // (service time, index)
    int j = 0, currentTime = 0;

    while (j < process_count || !pq.empty())  // Run until all processes are scheduled
    {
        // Add all processes that have arrived by currentTime
        while (j < process_count && getArrivalTime(processes[j]) <= currentTime)
        {
            pq.push(make_pair(getServiceTime(processes[j]), j)); // Push available processes
            j++;
        }

        if (!pq.empty())
        {
            // Pick the process with the shortest service time
            int processIndex = pq.top().second;
            pq.pop();

            int arrivalTime = getArrivalTime(processes[processIndex]);
            int serviceTime = getServiceTime(processes[processIndex]);

            // If CPU is idle, jump to the next available process
            if (currentTime < arrivalTime)
                currentTime = arrivalTime;

            int startTime = currentTime;
            int finish = currentTime + serviceTime;

            // Execution trace
            for (int t = startTime; t < finish; t++)
                timeline[t][processIndex] = '*'; // Mark execution in timeline
            for (int t = arrivalTime; t < startTime; t++)
                timeline[t][processIndex] = '.'; // Mark waiting

            finishTime[processIndex] = finish;
            turnAroundTime[processIndex] = finish - arrivalTime;
            normTurn[processIndex] = (turnAroundTime[processIndex] * 1.0 / serviceTime);

            currentTime = finish; // Move to process completion time
        }
        else
        {
            currentTime++; // If queue is empty, move to the next arrival time
        }
    }
}


void shortestRemainingTime()
{
    sort(all(processes),sortByArrivalTime);
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    int j = 0;
    for (int i = 0; i < last_instant; i++)
    {
        while(j<process_count &&getArrivalTime(processes[j]) == i){
            pq.push(make_pair(getServiceTime(processes[j]), j));
            j++;
        }
        if (!pq.empty())
        {
            int processIndex = pq.top().second;
            int remainingTime = pq.top().first;
            pq.pop();
            int serviceTime = getServiceTime(processes[processIndex]);
            int arrivalTime = getArrivalTime(processes[processIndex]);
            timeline[i][processIndex] = '*';

            if (remainingTime == 1) // process finished
            {
                finishTime[processIndex] = i + 1;
                turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
                normTurn[processIndex] = (turnAroundTime[processIndex] * 1.0 / serviceTime);
            }
            else
            {
                pq.push(make_pair(remainingTime - 1, processIndex));
            }
        }
    }
    fillInWaitTime();
}

void highestResponseRatioNext()
{

    sort(all(processes),sortByArrivalTime);
    // Vector of tuple <process_name, process_response_ratio, time_in_service> for processes that are in the ready queue
    vector<tuple<string, double, int>> present_processes;
    int j=0;
    for (int current_instant = 0; current_instant < last_instant; current_instant++)
    {
        while(j<process_count && getArrivalTime(processes[j])<=current_instant){
            present_processes.push_back(make_tuple(getProcessName(processes[j]), 1.0, 0));
            j++;
        }
        // Calculate response ratio for every process
        for (auto &proc : present_processes)
        {
            string process_name = get<0>(proc);
            int process_index = processToIndex[process_name];
            int wait_time = current_instant - getArrivalTime(processes[process_index]);
            int service_time = getServiceTime(processes[process_index]);
            get<1>(proc) = calculate_response_ratio(wait_time, service_time);
        }

        // Sort present processes by highest to lowest response ratio
        sort(all(present_processes), descendingly_by_response_ratio);

        if (!present_processes.empty())
        {
            int process_index = processToIndex[get<0>(present_processes[0])];
            while(current_instant<last_instant && get<2>(present_processes[0]) != getServiceTime(processes[process_index])){
                timeline[current_instant][process_index]='*';
                current_instant++;
                get<2>(present_processes[0])++;
            }
            current_instant--;
            present_processes.erase(present_processes.begin());
            finishTime[process_index] = current_instant + 1;
            turnAroundTime[process_index] = finishTime[process_index] - getArrivalTime(processes[process_index]);
            normTurn[process_index] = (turnAroundTime[process_index] * 1.0 / getServiceTime(processes[process_index]));
        }
    }
    fillInWaitTime();
}

void feedbackQ1()
{
    sort(all(processes),sortByArrivalTime);
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq; //pair of priority level and process index
    unordered_map<int,int>remainingServiceTime; //map from process index to the remaining service time
    int j=0;
    if(getArrivalTime(processes[0])==0){
        pq.push(make_pair(0,j));
        remainingServiceTime[j]=getServiceTime(processes[j]);
        j++;
    }
    for(int time =0;time<last_instant;time++){
        if(!pq.empty()){
            int priorityLevel = pq.top().first;
            int processIndex =pq.top().second;
            int arrivalTime = getArrivalTime(processes[processIndex]);
            int serviceTime = getServiceTime(processes[processIndex]);
            pq.pop();
            while(j<process_count && getArrivalTime(processes[j])==time+1){
                    pq.push(make_pair(0,j));
                    remainingServiceTime[j]=getServiceTime(processes[j]);
                    j++;
            }
            remainingServiceTime[processIndex]--;
            timeline[time][processIndex]='*';
            if(remainingServiceTime[processIndex]==0){
                finishTime[processIndex]=time+1;
                turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
                normTurn[processIndex] = (turnAroundTime[processIndex] * 1.0 / serviceTime);
            }else{
                if(pq.size()>=1)
                    pq.push(make_pair(priorityLevel+1,processIndex));
                else
                    pq.push(make_pair(priorityLevel,processIndex));
            }
        }
        while(j<process_count && getArrivalTime(processes[j])==time+1){
                pq.push(make_pair(0,j));
                remainingServiceTime[j]=getServiceTime(processes[j]);
                j++;
        }
    }
    fillInWaitTime();
}

void feedbackQ2i()
{
    sort(all(processes),sortByArrivalTime);
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq; //pair of priority level and process index
    unordered_map<int,int>remainingServiceTime; //map from process index to the remaining service time
    int j=0;
    if(getArrivalTime(processes[0])==0){
        pq.push(make_pair(0,j));
        remainingServiceTime[j]=getServiceTime(processes[j]);
        j++;
    }
    for(int time =0;time<last_instant;time++){
        if(!pq.empty()){
            int priorityLevel = pq.top().first;
            int processIndex =pq.top().second;
            int arrivalTime = getArrivalTime(processes[processIndex]);
            int serviceTime = getServiceTime(processes[processIndex]);
            pq.pop();
            while(j<process_count && getArrivalTime(processes[j])<=time+1){
                    pq.push(make_pair(0,j));
                    remainingServiceTime[j]=getServiceTime(processes[j]);
                    j++;
            }

            int currentQuantum = pow(2,priorityLevel);
            int temp = time;
            while(currentQuantum && remainingServiceTime[processIndex]){
                currentQuantum--;
                remainingServiceTime[processIndex]--;
                timeline[temp++][processIndex]='*';
            }

            if(remainingServiceTime[processIndex]==0){
                finishTime[processIndex]=temp;
                turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
                normTurn[processIndex] = (turnAroundTime[processIndex] * 1.0 / serviceTime);
            }else{
                if(pq.size()>=1)
                    pq.push(make_pair(priorityLevel+1,processIndex));
                else
                    pq.push(make_pair(priorityLevel,processIndex));
            }
            time = temp-1;
        }
        while(j<process_count && getArrivalTime(processes[j])<=time+1){
                pq.push(make_pair(0,j));
                remainingServiceTime[j]=getServiceTime(processes[j]);
                j++;
        }
    }
    fillInWaitTime();
}

void aging(int originalQuantum)
{
    sort(all(processes),sortByArrivalTime);
    vector<tuple<int,int,int>>v; //tuple of priority level, process index and total waiting time
    int j=0,currentProcess=-1;
    for(int time =0;time<last_instant;time++){
        while(j<process_count && getArrivalTime(processes[j])<=time){
            v.push_back(make_tuple(getPriorityLevel(processes[j]),j,0));
            j++;
        }

        for(int i=0;i<v.size();i++){
            if(get<1>(v[i])==currentProcess){
                get<2>(v[i])=0;
                get<0>(v[i])=getPriorityLevel(processes[currentProcess]);
            }
            else{
                get<0>(v[i])++;
                get<2>(v[i])++;
            }
        }
        sort(v.begin(),v.end(),byPriorityLevel);
        currentProcess=get<1>(v[0]);
        int currentQuantum = originalQuantum;
        while(currentQuantum-- && time<last_instant){
            timeline[time][currentProcess]='*';
            time++;
        }
        time--;
    }
    fillInWaitTime();
}

void printAlgorithm(int algorithm_index)
{
    int algorithm_id = algorithms[algorithm_index].first - '0';
    if(algorithm_id==2)
        cout << ALGORITHMS[algorithm_id] <<algorithms[algorithm_index].second <<endl;
    else
        cout << ALGORITHMS[algorithm_id] << endl;
}

void printProcesses()
{
    cout << "Process    ";
    for (int i = 0; i < process_count; i++)
        cout << "|  " << getProcessName(processes[i]) << "  ";
    cout << "|\n";
}
void printArrivalTime()
{
    cout << "Arrival    ";
    for (int i = 0; i < process_count; i++)
        printf("|%3d  ",getArrivalTime(processes[i]));
    cout<<"|\n";
}
void printServiceTime()
{
    cout << "Service    |";
    for (int i = 0; i < process_count; i++)
        printf("%3d  |",getServiceTime(processes[i]));
    cout << " Mean|\n";
}
void printFinishTime()
{
    cout << "Finish     ";
    for (int i = 0; i < process_count; i++)
        printf("|%3d  ",finishTime[i]);
    cout << "|-----|\n";
}
void printTurnAroundTime()
{
    cout << "Turnaround |";
    int sum = 0;
    for (int i = 0; i < process_count; i++)
    {
        printf("%3d  |",turnAroundTime[i]);
        sum += turnAroundTime[i];
    }
    if((1.0 * sum / turnAroundTime.size())>=10)
		printf("%2.2f|\n",(1.0 * sum / turnAroundTime.size()));
    else
	 	printf(" %2.2f|\n",(1.0 * sum / turnAroundTime.size()));
}

void printNormTurn()
{
    cout << "NormTurn   |";
    float sum = 0;
    for (int i = 0; i < process_count; i++)
    {
        if( normTurn[i]>=10 )
            printf("%2.2f|",normTurn[i]);
        else
            printf(" %2.2f|",normTurn[i]);
        sum += normTurn[i];
    }

    if( (1.0 * sum / normTurn.size()) >=10 )
        printf("%2.2f|\n",(1.0 * sum / normTurn.size()));
	else
        printf(" %2.2f|\n",(1.0 * sum / normTurn.size()));
}


void calculate_performance_metrics(string algorithm_name)
{
    double total_waiting_time = 0;
    int total_service_time = 0;
    int completed_processes = 0;

    for (int i = 0; i < process_count; i++)
    {
        int arrivalTime = getArrivalTime(processes[i]);
        int serviceTime = getServiceTime(processes[i]);

        int waitingTime = turnAroundTime[i] - serviceTime; // Waiting Time = Turnaround Time - Service Time
        total_waiting_time += waitingTime;
        total_service_time += serviceTime;
        completed_processes++;
    }

    // Compute Metrics
    double avg_waiting_time = total_waiting_time / process_count;
    double cpu_utilization = (total_service_time * 100.0) / last_instant;
    double throughput = completed_processes * 1.0 / last_instant;

    // Store results
    algorithmMetrics[algorithm_name] = {avg_waiting_time, cpu_utilization, throughput};

    // Print individual algorithm's results
    cout << "\n==== Performance Metrics for " << algorithm_name << " ====\n";
    cout << "Average Waiting Time   : " << fixed << setprecision(2) << avg_waiting_time << " units\n";
    cout << "CPU Utilization        : " << fixed << setprecision(2) << cpu_utilization << "%\n";
    cout << "Throughput            : " << fixed << setprecision(2) << throughput << " processes/unit time\n";
    cout << "====================================\n\n";
}




void parse_compare()
{
    if (operation == "compare")
    {
        cout << "\n========================================\n";
        cout << "  Comparing Performance of Algorithms \n";
        cout << "========================================\n";
        cout << "Algorithms Selected for Comparison:\n";

        for (auto &algo : algorithms)
        {
            int algorithm_id = algo.first - '0';
            if (algorithm_id == 2)
                cout << "- " << ALGORITHMS[algorithm_id] << algo.second << endl;
            else
                cout << "- " << ALGORITHMS[algorithm_id] << endl;
        }
        cout << "========================================\n";
    }
}


void printStats(int algorithm_index)
{
    printAlgorithm(algorithm_index);
    printProcesses();
    printArrivalTime();
    printServiceTime();
    printFinishTime();
    printTurnAroundTime();
    printNormTurn();


    string algorithm_name;
    int algorithm_id = algorithms[algorithm_index].first - '0';

    if (algorithm_id == 2)
        algorithm_name = ALGORITHMS[algorithm_id] + to_string(algorithms[algorithm_index].second);
    else
        algorithm_name = ALGORITHMS[algorithm_id];

    // Compute and store metrics
    calculate_performance_metrics(algorithm_name);
}

void printTimeline(int algorithm_index)
{   
    if (coloredVisualization) {
        printColoredTimeline(algorithm_index);
        return;
    }
    
    for (int i = 0; i <= last_instant; i++)
        cout << i % 10<<" ";
    cout <<"\n";
    cout << "------------------------------------------------\n";
    for (int i = 0; i < process_count; i++)
    {
        cout << getProcessName(processes[i]) << "     |";
        for (int j = 0; j < last_instant; j++)
        {
            cout << timeline[j][i]<<"|";
        }
        cout << " \n";
    }
    cout << "------------------------------------------------\n";
}

void execute_algorithm(char algorithm_id, int quantum,string operation)
{
    switch (algorithm_id)
    {
    case '1':
        if(operation==TRACE)cout<<"FCFS  ";
        firstComeFirstServe();
        break;
    case '2':
        if(operation==TRACE)cout<<"RR-"<<quantum<<"  ";
        roundRobin(quantum);
        break;
    case '3':
        if(operation==TRACE)cout<<"SPN   ";
        shortestProcessNext();
        break;
    case '4':
        if(operation==TRACE)cout<<"SRT   ";
        shortestRemainingTime();
        break;
    case '5':
        if(operation==TRACE)cout<<"HRRN  ";
        highestResponseRatioNext();
        break;
    case '6':
        if(operation==TRACE)cout<<"FB-1  ";
        feedbackQ1();
        break;
    case '7':
        if(operation==TRACE)cout<<"FB-2i ";
        feedbackQ2i();
        break;
    case '8':
        if(operation==TRACE)cout<<"Aging ";
        aging(quantum);
        break;
    default:
        break;
    }
}

int main()
{
    parse();

    
    if (operation == "compare")
    {
        for (int idx = 0; idx < (int)algorithms.size(); idx++)
        {
            clear_timeline();
            execute_algorithm(algorithms[idx].first, algorithms[idx].second, operation);
            calculate_performance_metrics(ALGORITHMS[algorithms[idx].first - '0']); // Collect metrics for comparison
        }
        string best_algo, worst_algo;
        string worst_util_algo, worst_throughput_algo;
    
        double min_waiting_time = DBL_MAX, max_waiting_time = DBL_MIN;
        double max_utilization = DBL_MIN, min_utilization = DBL_MAX;
        double max_throughput = DBL_MIN, min_throughput = DBL_MAX;
    
        // Finding best & worst based on waiting time + efficiency
        for (auto &entry : algorithmMetrics) {
            const string& algo = entry.first;
            double waiting_time = entry.second.avg_waiting_time;
            double utilization = entry.second.cpu_utilization;
            double throughput = entry.second.throughput;
    
            // Best algorithm (lowest waiting time, prefers higher CPU efficiency & throughput in case of tie)
            if (waiting_time < min_waiting_time || 
               (waiting_time == min_waiting_time && utilization > max_utilization) ||
               (waiting_time == min_waiting_time && utilization == max_utilization && throughput > max_throughput)) 
            {
                min_waiting_time = waiting_time;
                max_utilization = utilization;
                max_throughput = throughput;
                best_algo = algo;
            }
    
            // Worst algorithm (highest waiting time, prefers lower CPU efficiency in case of tie)
            if (waiting_time > max_waiting_time || 
               (waiting_time == max_waiting_time && utilization < min_utilization)) 
            {
                max_waiting_time = waiting_time;
                min_utilization = utilization;
                worst_algo = algo;
            }
    
            // Track worst CPU utilization
            if (utilization < min_utilization) {
                min_utilization = utilization;
                worst_util_algo = algo;
            }
    
            // Track worst throughput
            if (throughput < min_throughput) {
                min_throughput = throughput;
                worst_throughput_algo = algo;
            }
        }
    
        // Calculate dynamic threshold for response time classification
        double avg_waiting_threshold = (min_waiting_time + max_waiting_time) / 2;
    
        // Display comparison results
        cout << "\n========================================\n";
        cout << "   Properties & Performance Comparison \n";
        cout << "========================================\n";
    
        for (auto &entry : algorithmMetrics) {
            cout << entry.first << " -> ";
    
            // Response Time Classification
            if (entry.second.avg_waiting_time == min_waiting_time)
                cout << "  Fastest Response, ";
            else if (entry.second.avg_waiting_time < avg_waiting_threshold)
                cout << "  Optimized Response Time, ";
            else
                cout << "  Stable Execution, ";
    
            // CPU Utilization Classification
            if (entry.second.cpu_utilization == max_utilization)
                cout << "  Maximum CPU Efficiency, ";
            else if (entry.second.cpu_utilization > 50)
                cout << "  Balanced CPU Load, ";
            else
                cout << "  Energy Efficient, ";
    
            // Throughput Classification
            if (entry.second.throughput == max_throughput)
                cout << "  Best Throughput!";
            else
                cout << "  Reliable Performance!";
    
            cout << '\n';
        }
    
        // Display the best and worst algorithms
        cout << "\n Best Performing Algorithm: **" << best_algo << "** (Lowest Waiting Time + Efficiency)\n";
        cout << " Slowest Algorithm: **" << worst_algo << "** (Highest Waiting Time)\n";
    
        // Handle equal CPU utilization
        if (min_utilization == max_utilization) {
            cout << " All algorithms have equal CPU Utilization: " << min_utilization << "%\n";
        } else {
            cout << " Lowest CPU Utilization: **" << worst_util_algo << "** (" << min_utilization << "%)\n";
        }
    
        // Handle equal throughput
        if (min_throughput == max_throughput) {
            cout << " All algorithms have equal Throughput: " << min_throughput << " processes/unit time\n";
        } else {
            cout << " Worst Throughput: **" << worst_throughput_algo << "** (" << min_throughput << " processes/unit time)\n";
        }
    
        cout << "========================================\n";
    }
    else
    {
        for (int idx = 0; idx < (int)algorithms.size(); idx++)
        {
            clear_timeline();
            execute_algorithm(algorithms[idx].first, algorithms[idx].second, operation);

            if (operation == TRACE)
                printTimeline(idx);
            else if (operation == SHOW_STATISTICS)
                printStats(idx);

            cout << "\n";
        }
    }

    return 0;
}
