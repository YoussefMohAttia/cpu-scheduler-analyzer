#include <iostream>
#include <bits/stdc++.h>
#include <string>
#include <algorithm>
#include <queue>
#include <iomanip>
using namespace std;

// Global variables
int simulationend;
int numofprocess;

// Process struct
struct Process{
    int startT;       
    int finishT;       
    int arrivalT;      
    int serviceT;      
    int turnaroundT;   
    float nturnaround; 
    int waitingT=0;    
    int checkingT;
    int remainingT;    
    int workingT;      
    char name;         
    int id;            
    int Priority;      
    int Done;          
    int quantum;       
    float resRatio;    
};

// Print statistics
void PrintS(Process p[]){
    cout << left;
    cout << setw(11) << "Process" << "|";
    for(int i = 0; i < numofprocess; i++) {
        printf("%3c  |", p[i].name);
    }
    printf("\n");    
    // arriv. times
    cout << setw(11) << "Arrival" << "|";
    for(int i = 0; i < numofprocess; i++)
        printf("%3d  |", p[i].arrivalT);
    printf("\n");    
    // serv. times
    cout << left << setw(11) << "Service" << "|";
    for(int i = 0; i < numofprocess; i++)
        printf("%3d  |", p[i].serviceT);
    printf(" Mean|\n");   
    // finish times
    cout << left << setw(11) << "Finish" << "|";
    for(int i = 0; i < numofprocess; i++)
        printf("%3d  |", p[i].finishT);
    printf("-----|\n");    
    // turna. time and mean
    cout << left << setw(11) << "Turnaround" << "|";
    int sum = 0;
    for(int i = 0; i < numofprocess; i++) {
        printf("%3d  |", p[i].turnaroundT);
        sum += p[i].turnaroundT;
    }
    cout << fixed << setprecision(2) << right << setw(5) << (sum * 1.0 / numofprocess) << "|\n";    
    // Print norm. times and mean
    cout << left << setw(11) << "NormTurn" << "|";
    float summ = 0;
    for(int i = 0; i < numofprocess; i++) {
        printf("%5.2f|", p[i].nturnaround);
        summ += p[i].nturnaround;
    }
    cout << fixed << setprecision(2) << right << setw(5) << (summ / numofprocess) << "|\n\n";
}

// Print trace
void PrintT(Process p[], char** arr){
     for(int i=0 ; i<numofprocess ; i++)
    {   
        cout<<p[i].name<<"     ";
        cout<<"|";
        for(int j = 0 ; j<simulationend ; j++)
            cout<<arr[i][j]<<"|";
        cout<<" "<<"\n";
    }
}

// function used in Aging algorithm to sort queue
queue<Process> Ag_sortQueue(queue<Process> &q)
{
   vector<Process> temp;
   while(!q.empty())
   {
       temp.push_back(q.front());
       q.pop();
   }
    sort(temp.begin(), temp.end(), [](Process a, Process b) {
        return (a.Priority != b.Priority) ? (a.Priority > b.Priority) : (a.waitingT > b.waitingT);
    });
   for(int i=0;i<temp.size();i++)
   {
       q.push(temp[i]);
   }
   return q;
}

// function used in Aging algorithm to increment priority and time
queue<Process> Ag_incrementQueue(queue<Process> &q,int quantum)
{
   vector<Process> temp;
   while(!q.empty())
   {
       Process t = q.front();
       t.Priority++;
       t.waitingT+=quantum;
       temp.push_back(t);
       q.pop();
   }
   
   for(int i=0;i<temp.size();i++)
   {
       q.push(temp[i]);
   }
   return q;
}

void Hrrn(Process p[], char** arr){
    int runningT=0;    
    float maxR;        
    int Selected;     
    // Init. process states
    for (int i= 0 ; i<numofprocess ; i++){
        p[i].id=i;
        p[i].Done = 0;
        p[i].resRatio = 0;
    }
    runningT = p[0].arrivalT;
    while(runningT<simulationend){
        maxR = INT16_MIN;
        for(int i=0;i<numofprocess;i++){
            if(p[i].arrivalT<=runningT && p[i].Done==0){
                int wait = runningT - p[i].arrivalT;
                p[i].resRatio = (1.0*(wait + p[i].serviceT)) /p[i].serviceT;
                for(int j =1;j<=wait;j++)
                    arr[p[i].id][runningT-j] = '.';
                maxR = max(maxR,p[i].resRatio);
                if(p[i].resRatio==maxR)
                    Selected = i;
            }
        }
        
        for(int i=0;i<p[Selected].serviceT;i++){
            arr[p[Selected].id][runningT+i] = '*';
        }
        runningT += p[Selected].serviceT;
        p[Selected].Done=1;
        p[Selected].finishT = runningT;
        p[Selected].turnaroundT = p[Selected].finishT -p[Selected].arrivalT;
        p[Selected].nturnaround = (1.0*p[Selected].turnaroundT)/p[Selected].serviceT; 
    }
    
}

void Spn(Process p[], char** arr) {
    // Create priority queues and sort them
    priority_queue<Process, vector<Process>, function<bool(Process,Process)>> readyQueue(
        [](Process p1, Process p2) { return p1.serviceT > p2.serviceT; }
    );
    priority_queue<Process, vector<Process>, function<bool(Process,Process)>> arrivalQueue(
        [](Process p1, Process p2) { return p1.arrivalT > p2.arrivalT; }
    );
    // Initi. process states
    for(int i = 0; i < numofprocess; i++) {
        p[i].id = i;
        p[i].waitingT = 0;
        p[i].workingT = 0;
        p[i].remainingT = p[i].serviceT;
        p[i].checkingT = 0;
        arrivalQueue.push(p[i]);
    }
    int tNow = 0;
    bool firstProcess = true;
    Process temp;
    int current = 0;

    while(tNow < simulationend) {
        // Check arriving processes
        while(!arrivalQueue.empty() && arrivalQueue.top().arrivalT <= tNow) {
            readyQueue.push(arrivalQueue.top());
            arrivalQueue.pop();
        }
        if(!readyQueue.empty()) {
            temp = readyQueue.top();
            readyQueue.pop();

            if(firstProcess) {
                temp.startT = temp.arrivalT;
                firstProcess = false;
            } else {
                temp.startT = tNow;
            }
            // Calc times
            temp.finishT = temp.startT + temp.serviceT;
            temp.turnaroundT = temp.finishT - temp.arrivalT;
            temp.nturnaround = (float)temp.turnaroundT / temp.serviceT;
            temp.waitingT = temp.startT - temp.arrivalT;
            temp.workingT = temp.serviceT;
            // Update trace arr
            for(int i = temp.startT; i < temp.finishT; i++) {
                arr[temp.id][i] = '*';
            }
            for(int i = temp.arrivalT; i < temp.startT; i++) {
                arr[temp.id][i] = '.';
            }
            p[temp.id] = temp;
            tNow = temp.finishT;
        } else {
            tNow++;
        }
    }
    sort(p, p + numofprocess, [](Process a, Process b) { return a.arrivalT < b.arrivalT; });
}

void Fb2i(Process p[], char** arr) {
    const int Maxtime = 1000; // prevent infinite loops
    int tNow = 0;
    // Queues to handle processes
    queue<Process> inputQueue;
    priority_queue<Process, vector<Process>, function<bool(Process,Process)>> readyQueue(
        [](Process p1, Process p2) { return p1.Priority > p2.Priority; }
    );
    priority_queue<Process, vector<Process>, function<bool(Process,Process)>> tempQueue(
        [](Process p1, Process p2) { return p1.arrivalT > p2.arrivalT; }
    );
    // Init.
    for (int i = 0; i < numofprocess; i++) {
        p[i].id=i;
        p[i].waitingT=0;
        p[i].workingT=0;
        p[i].remainingT=p[i].serviceT;
        p[i].checkingT=0;
        p[i].Priority=0;
        p[i].quantum=1;
        inputQueue.push(p[i]);
    }
    int completedCount = 0;
    while((!inputQueue.empty() || !readyQueue.empty()) && tNow < Maxtime) {
        // push arrived processes to ready
        while(!inputQueue.empty() && inputQueue.front().arrivalT <= tNow) {
            readyQueue.push(inputQueue.front());
            inputQueue.pop();
        }
        // Sort by arrival time if Priority is equal
        while(!readyQueue.empty()) {
            tempQueue.push(readyQueue.top());
            readyQueue.pop();
        }
        while(!tempQueue.empty()) {
            readyQueue.push(tempQueue.top());
            tempQueue.pop();
        }

        if(readyQueue.empty()) {
            tNow++;
            continue;
        }
        Process currentProc = readyQueue.top();
        readyQueue.pop();
        // first exec.
        if(currentProc.workingT == 0) {
            currentProc.startT = tNow;
            currentProc.checkingT = tNow;
        }
        // quantum Adjusting
        currentProc.quantum = min(currentProc.quantum, currentProc.remainingT);
        // Execute
        for(int i = tNow; i < tNow + currentProc.quantum; i++) {
            arr[currentProc.id][i] = '*';
        }
        // Update Times & Check for preemption
        currentProc.workingT += currentProc.quantum;
        tNow += currentProc.quantum;
        currentProc.checkingT += currentProc.quantum;
        if(tNow != currentProc.checkingT) {
            currentProc.waitingT += (tNow - currentProc.checkingT);
            currentProc.checkingT = tNow;
        }
        currentProc.remainingT = currentProc.serviceT - currentProc.workingT;
        // Process completed
        if(currentProc.remainingT == 0) {
            currentProc.finishT = currentProc.startT + currentProc.waitingT + currentProc.serviceT;
            currentProc.turnaroundT = currentProc.finishT - currentProc.arrivalT;
            currentProc.nturnaround = currentProc.turnaroundT * 1.0 / currentProc.serviceT;
            for(int i = currentProc.arrivalT; i < currentProc.finishT; i++) {
                if(arr[currentProc.id][i] != '*') {
                    arr[currentProc.id][i] = '.';
                }
            }
            p[completedCount++] = currentProc;
        } else {
            while(!inputQueue.empty() && inputQueue.front().arrivalT <= tNow) {
                readyQueue.push(inputQueue.front());
                inputQueue.pop();
            }
            // Update Priority and quantum
            if(!readyQueue.empty()) {
                currentProc.Priority++;
                currentProc.quantum *= 2;
            }
            readyQueue.push(currentProc);
        }
    }

    sort(p, p + numofprocess, [](Process a, Process b) { return a.arrivalT < b.arrivalT; });
}

void Aging(Process p[], int q, char** arr){
    queue<Process> ready;    // Queues init.
    queue<Process> copy;
    int runningT = p[0].arrivalT;
    int Admit[numofprocess] = {0};

    for(int i=0; i<numofprocess;i++){
        p[i].Priority = p[i].serviceT;
        p[i].id = i;
        p[i].waitingT = 0;
    }
    ready.push(p[0]);
    Admit[0]=1;
    
    while(runningT<simulationend){
        for(int i=0;i<numofprocess;i++){
            if(p[i].arrivalT<=runningT && Admit[i]!=1){
                p[i].waitingT = runningT-p[i].arrivalT;
                p[i].Priority++;
                ready.push(p[i]);
                Admit[i]=1; 
                
            }
        }
        ready = Ag_sortQueue(ready);
        Process temp = ready.front();
        ready.pop();
        temp.Priority = temp.serviceT;
        temp.waitingT=0;
        
        copy = ready;
        for(int i=0;i<q;i++){
            while(!copy.empty()){
            Process t = copy.front();
            copy.pop();
            arr[t.id][runningT+i] = '.';
        }
        }
        for(int i = 0;i<q;i++){
            arr[temp.id][runningT+i] = '*';
        }

        ready = Ag_incrementQueue(ready,q);
        ready.push(temp);
        runningT += q;
    }

    
}

void initArray(char** arr){     // Init. array
    for(int i=0 ; i<numofprocess ; i++)
    {
        for(int j = 0 ; j<simulationend ; j++)
        arr[i][j]=' ';
    }

}

void printTopNumbers(int totalTime) {   // Print time scale
    int j = 0;
    for(int i = 0; i < totalTime + 1; i++) {
        cout << j << " ";
        j++;
        if(j % 10 == 0)
            j = 0;
    }
    cout << "\n";
}

// Print separator line for trace
void printSeparatorLine(int totalTime) {
    for(int i = 0; i < (2*totalTime + 8); i++) {
        cout << "-";
    }
    cout << "\n";
}

int main(){
    string Algorithm; 
    char Runmode [10]; 
    vector<string> Algorithms; 
    char del = ',';
    bool stats=0;
    cin>>Runmode;
    cin>>Algorithm;
    cin>>simulationend;
    cin>>numofprocess;
    Process p[numofprocess];
    // Allocate memory
    char** trace = new char*[numofprocess];
    for(int i = 0; i < numofprocess; i++) {
        trace[i] = new char[simulationend];
    }

    if (strcmp("stats",Runmode)==0)
    stats=1;
    for(int i=0;i<numofprocess;i++){
        cin>>p[i].name>>del>>p[i].arrivalT>>del>>p[i].serviceT;
    }
    sort(p,p+numofprocess,[](Process a, Process b) { return a.arrivalT < b.arrivalT; });
    stringstream ss(Algorithm);
    while(ss.good())
    {
        string substr;
        getline(ss, substr, del);
        Algorithms.push_back(substr);
    }

    if(stats){          // stats functionality
        for(int i = 0; i<Algorithms.size();i++){
            if(Algorithms[i][0] == '3'){
                Spn(p,trace);
                cout<<"SPN"<<"\n";
                PrintS(p);
            } else if(Algorithms[i][0] == '5'){
                Hrrn(p,trace);
                cout<<"HRRN"<<"\n";
                PrintS(p);
            } else if(Algorithms[i][0] == '7'){
                cout<<"FB-2i"<<"\n";
                Fb2i(p,trace);
                PrintS(p);
            }else {
                cout << "u should test Algorithms (3,5,7) - No Stats for aging\n";
            }
        }
    } else {            // trace functionality
        for(int i = 0; i<Algorithms.size();i++){
            initArray(trace);
            if(Algorithms[i][0] == '3'){
                cout<<"SPN   ";
                printTopNumbers(simulationend);
                printSeparatorLine(simulationend);
                Spn(p,trace);
                PrintT(p,trace);
                printSeparatorLine(simulationend);
                cout<<"\n";
            } else if(Algorithms[i][0] == '5'){
                cout<<"HRRN  ";
                printTopNumbers(simulationend);
                printSeparatorLine(simulationend);
                Hrrn(p,trace);
                PrintT(p,trace);
                printSeparatorLine(simulationend);
                cout<<"\n";
            } else if(Algorithms[i][0] == '7'){
                cout<<"FB-2i ";
                printTopNumbers(simulationend);
                printSeparatorLine(simulationend);
                Fb2i(p,trace);
                PrintT(p,trace);
                printSeparatorLine(simulationend);
                cout<<"\n";
            } else if(Algorithms[i][0] == '8'){
                cout<<"Aging ";
                printTopNumbers(simulationend);
                printSeparatorLine(simulationend);
                int q = int(Algorithms[i][2])-'0';
                Aging(p,q,trace);
                PrintT(p,trace);
                printSeparatorLine(simulationend);
                cout<<"\n";
            } else {
                cout << "You should test Algorithms (3,5,7,8)\n";
            }
        }
    }
    // Deallocate memory
    for(int i = 0; i < numofprocess; i++) {
        delete[] trace[i];
    }
    delete[] trace;

    return 0;
}
