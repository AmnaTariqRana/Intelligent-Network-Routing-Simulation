#include <iostream>
#include <fstream>
#include <sstream>
#include <climits>
#include <string>
#include <cctype> 
#include <cstring>  // for strncmp
#include <chrono>
#include <thread>
using namespace std;




const int MAX_DEVICES = 30;
const int MAX = 30;
const int INF = INT_MAX;
class Router;
class Machine;
class Message;
class LinearRoutingTable;
class FIFOQ;
class PRIQ;
struct Device;


int dist[MAX_DEVICES];  
Router* routers[MAX_DEVICES]; 
int routerCount = 0;


struct Message
{
    int ID;
    int priority;
    string srcadd;
    string destAddr;
    string pyl;
    string trc;
    chrono::system_clock::time_point queueTime;

    Message(int id, int pri, string src, string dest, string pl, string tr = "") : ID(id), priority(pri), srcadd(src), destAddr(dest), pyl(pl), trc(tr),
        queueTime(chrono::system_clock::now())
    {
    }




    Message(const string& line)
    {
        const char* ptr = line.c_str();
        char field[256];
        int field_idx = 0;

        while (*ptr)
        {


            if (*ptr == '|' || *ptr == '\0')
            {
                field[field_idx] = '\0';
                processField(field);
                field_idx = 0;
                ptr++;
                continue;
            }

            field[field_idx++] = *ptr++;



        }


        if (field_idx > 0)
        {
            field[field_idx] = '\0';
            processField(field);
        }
        queueTime = chrono::system_clock::now();



    }



    void processField(const char* field)
    {
        if (strncmp(field, "ID:", 3) == 0)
        {
            ID = atoi(field + 3);
        }
        else if (strncmp(field, "PRIORITY:", 9) == 0)
        {
            priority = atoi(field + 9);
        }
        else if (strncmp(field, "SRC:", 4) == 0)
        {
            srcadd = string(field + 4);
        }
        else if (strncmp(field, "DEST:", 5) == 0)
        {
            destAddr = string(field + 5);
        }
        else if (strncmp(field, "pyl:", 8) == 0)
        {
            pyl = string(field + 8);
        }
        else if (strncmp(field, "trc:", 6) == 0)
        {
            trc = string(field + 6);
        }
    }


    bool operator<(const Message& other) const
    {
        return priority < other.priority;
    }



    bool operator>(const Message& other) const
    {
        return priority > other.priority;
    }

};




class PRIQ {
public:



    Message* queue[MAX];
    int size;

    PRIQ()
    {
        size = 0;
    }


    void swap(Message*& n1, Message*& n2)
    {
        Message* temp = n1;
        n1 = n2;
        n2 = temp;
    }


    void heapifyup(int idx)
    {
        while (idx > 0)
        {
            int parent = (idx - 1) / 2;
            if (queue[idx]->priority < queue[parent]->priority)
            {
                swap(queue[idx], queue[parent]);
                idx = parent;
            }
            else {
                break;
            }

        }

    }



    void heapifydown(int idx)
    {
        while (2 * idx + 1 < size)
        {
            int left = 2 * idx + 1;
            int right = 2 * idx + 2;

            int min = idx;

            if (left < size && queue[left] < queue[min])
            {
                min = left;
            }

            if (right < size && queue[right] < queue[min])
            {
                min = right;
            }

            if (min != idx)
            {
                swap(queue[idx], queue[min]);
                idx = min;

            }
            else {
                break;
            }


        }
    }




    void enQ(Message* msg)
    {
        if (size >= MAX)
        {
            cout << "Queue is FULL" << endl;
            return;
        }

        queue[size] = msg;
        heapifyup(size);
        size++;



    }


    Message* deQ()
    {
        if (size <= 0)
        {
            cout << "Queue is Empty:" << endl;
            return nullptr;
        }


        Message* top = queue[0];
        queue[0] = queue[size - 1];
        size--;
        heapifydown(0);
        return top;
    }






    Message* peek() {
        if (size <= 0) {
            cout << "Queue is EMPTY" << endl;
            return nullptr;
        }
        return queue[0];
    }


    bool isEmpty() const
    {
        return size == 0;
    }








};








class FIFOQ {
private:


    Message* queue[MAX];
    int front, rear;
    chrono::system_clock::time_point lastForwardTime;




public:
    FIFOQ() : front(-1), rear(-1) {}

    void enQ(Message* msg)
    {
        if (rear == MAX - 1)
        {
            cout << "Queue is full\n";
            return;
        }


        if (front == -1)
        {
            front = 0;
        }
        rear++;
        queue[rear] = msg;



    }

    Message* timedDequeue() {


        if (front == -1 || front > rear)
        {
            return nullptr;
        }

        auto now = chrono::system_clock::now();
        Message* frontMsg = queue[front];

        // Check if 1 second has passed since this message was enqueued
        if (now - frontMsg->queueTime >= 1s)
        {



            front++;
            if (front > rear)
            {
                front = rear = -1;
            }


            return frontMsg;


        }

        return nullptr;
    }

    bool isEmpty() const
    {
        return front == -1 || front > rear;
    }







};








struct RoutingEntry
{
    string destination;
    string outgoingQueue;

    RoutingEntry(string dest, string outQ) : destination(dest), outgoingQueue(outQ) {}



};

class LinearRoutingTable {
private:
    RoutingEntry* entries[MAX];
    int count;




public:
    LinearRoutingTable() : count(0) {}







    void addEntry(string dest, string outQ)
    {
        if (count >= MAX)
        {
            cout << "Routing table is full\n";
            return;
        }
        entries[count++] = new RoutingEntry(dest, outQ);
    }



    int getCount() const {
        return count;
    }

    RoutingEntry* getEntry(int i) const
    {
        if (i >= 0 && i < count)
        {
            return entries[i];
        }
        return nullptr;
    }




    bool removeEntry(string dest)
    {


        for (int i = 0; i < count; i++)
        {

            if (entries[i]->destination == dest)
            {

                delete entries[i];
                for (int j = i; j < count - 1; j++)
                {
                    entries[j] = entries[j + 1];
                }
                count--;
                return true;
            }
        }
        return false;


    }

    void clearEntries()
    {

        for (int m = 0; m < count; m++)
        {
            delete entries[m];
            entries[m] = nullptr;
        }
        count = 0;
    }



    string getNextHop(string dest)
    {
        for (int i = 0; i < count; i++)
        {
            if (entries[i]->destination == dest)
            {
                return entries[i]->outgoingQueue;
            }
        }
        return ""; // Not found
    }














    void printTable()
    {
        cout << "Routing Table (Linear List):\n";
        cout << "DESTINATION\tOUTGOING QUEUE\n";


        for (int i = 0; i < count; i++)
        {
            cout << entries[i]->destination << "\t\t"
                << entries[i]->outgoingQueue << endl;
        }




    }






};







void saveRoutingTable(const LinearRoutingTable& rt, const string& filename)
{


    ofstream file(filename);



    if (!file.is_open())
    {
        cout << "Error saving routing table to file: " << filename << endl;
        return;
    }

    for (int i = 0; i < rt.getCount(); ++i)
    {


        RoutingEntry* entry = rt.getEntry(i);
        if (entry)
        {
            file << entry->destination << ":" << entry->outgoingQueue << endl;


        }



    }

    file.close();
    cout << "Routing table saved to file: " << filename << endl;
}









void recordPath(Message* msg)
{
    string path = "C:\\Users\\Taiba\\source\\repos\\Data St. Project\\Data St. Project\\path.txt";
    ofstream pathFile(path, ios::app);
    if (!pathFile.is_open())
    {
        cout << "Error: Could not open path file!" << endl;
        return;
    }

    if (!msg->trc.empty())
    {
        pathFile << msg->ID << ":" << msg->srcadd << ":"
            << msg->trc << ":" << msg->destAddr << "\n";
    }
    else {
        pathFile << msg->ID << ":" << msg->srcadd << ":"
            << msg->destAddr << "\n";
    }
    cout << "Message ID " << msg->ID << " path recorded." << endl;
    pathFile.close();
}








struct Device {
    string id;
    bool isRouter;
};

Device devices[MAX_DEVICES];
int adjacency[MAX_DEVICES][MAX_DEVICES] = { 0 };
int deviceCount = 0;
class Machine {
private:
    string id;
    FIFOQ outgoingQueue;
    FIFOQ incomingQueue;
    Router* connectedRouter;

public:
    Machine(const string& machineId) : id(machineId), connectedRouter(nullptr) {}

    void connectToRouter(Router* router) {
        connectedRouter = router;
    }

    void sendMessage(Message* msg)
    {
        cout << "Machine " << id << ": Adding message ID " << msg->ID
            << " to outgoing queue" << endl;
        outgoingQueue.enQ(msg);
    }

    void processoutQ()
    {
        Message* msg = outgoingQueue.timedDequeue();
        if (msg && connectedRouter) {
            cout << "Machine " << id << ": Forwarding message ID " << msg->ID
                << " to router " << endl;// connectedRouter->getId() << endl;
            // connectedRouter->receiveMessage(msg);
        }
    }

    void receiveMessage(Message* msg)
    {
        if (msg->destAddr == id)
        {
            cout << "Machine " << id << ": Received message ID " << msg->ID << endl;
            recordPath(msg);
            delete msg;
        }
        else
        {
            incomingQueue.enQ(msg);
        }
    }

    void processINQ()
    {
        Message* msg = incomingQueue.timedDequeue();
        if (msg) {
            cout << "Machine " << id << ": Processing received message ID " << msg->ID << endl;
            if (msg->destAddr == id) {
                cout << "Machine " << id << ": Received message ID " << msg->ID << endl;
                recordPath(msg);
                delete msg;
            }
            else {
                // Shouldn't happen as we check destination before enqueueing
                cout << "Error: Message not for this machine" << endl;
            }
        }
    }
    bool isIdle() const
    {
        return incomingQueue.isEmpty() && outgoingQueue.isEmpty();
    }

    string getId() const { return id; }
};


void delayOneSecond()
{
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
}




class Router {
public:
    string id;
    PRIQ incomingQueue;
    FIFOQ* outQ[MAX_DEVICES];
    string outgoingQueueTargets[MAX_DEVICES];
    int outQcount;
    LinearRoutingTable routingTable;


    Router(const string& routerId) : id(routerId), outQcount(0)
    {
        for (int i = 0; i < MAX_DEVICES; i++)
        {
            outQ[i] = nullptr;
            outgoingQueueTargets[i] = "";
        }
    }

    ~Router() {
        for (int i = 0; i < outQcount; i++)
        {
            if (outQ[i] != nullptr)
            {
                delete outQ[i];
            }
        }
    }

    void addConnection(const string& destidx)
    {
        if (outQcount >= MAX_DEVICES)
        {
            cout << "Maximum outgoing connections reached for router " << id << endl;
            return;
        }

        for (int i = 0; i < outQcount; i++)
        {
            if (outgoingQueueTargets[i] == destidx)
            {
                return;
            }
        }

        outQ[outQcount] = new FIFOQ();
        outgoingQueueTargets[outQcount] = destidx;
        outQcount++;
    }




    void receiveMessage(Message* msg)
    {
        cout << "Router " << id << ": Received message ID " << msg->ID << endl;
        incomingQueue.enQ(msg);
    }

    void processMSG()
    {
        while (!incomingQueue.isEmpty())
        {
            Message* msg = incomingQueue.deQ();

            if (!msg->trc.empty())
            {
                msg->trc += ":";
            }
            msg->trc += id;

            string nextHop = routingTable.getNextHop(msg->destAddr);

            if (nextHop.empty()) {
                cout << "Router " << id << ": No route found for destination "
                    << msg->destAddr << endl;
                continue;
            }

            bool forwarded = false;
            for (int i = 0; i < outQcount; i++) {
                if (outgoingQueueTargets[i] == nextHop) {
                    msg->queueTime = chrono::system_clock::now(); // Reset queue time
                    outQ[i]->enQ(msg);
                    cout << "Router " << id << ": Forwarded message ID " << msg->ID
                        << " to " << nextHop << endl;
                    forwarded = true;
                    break;
                }
            }

            if (!forwarded) {
                cout << "Router " << id << ": No outgoing queue for " << nextHop << endl;
            }
        }
    }

    void forwardMessages(Router* rts[], int noofrt, Machine* machines[], int noofmatchine) {
        for (int i = 0; i < outQcount; i++) {
            Message* msg = outQ[i]->timedDequeue();
            if (msg) {
                string targetId = outgoingQueueTargets[i];

                // Check if destination is a router
                if (targetId[0] == 'R') {
                    bool delivered = false;
                    for (int j = 0; j < noofrt; j++) {
                        if (rts[j]->getId() == targetId) {
                            rts[j]->receiveMessage(msg);
                            cout << "Message ID " << msg->ID << " forwarded from "
                                << id << " to " << targetId << endl;
                            delivered = true;
                            break;
                        }
                    }
                    if (!delivered) {
                        cout << "Error: Target router " << targetId << " not found" << endl;
                    }
                }
                else {
                    // Destination is a machine
                    bool delivered = false;
                    for (int j = 0; j < noofmatchine; j++) {
                        if (machines[j]->getId() == targetId) {
                            machines[j]->receiveMessage(msg);
                            cout << "Message ID " << msg->ID << " delivered to machine " << targetId << endl;
                            delivered = true;
                            break;
                        }
                    }
                    if (!delivered) {
                        cout << "Error: Target machine " << targetId << " not found" << endl;
                    }
                }
            }
        }
    }

    string getId() const { return id; }

    void addRoutingEntry(const string& dest, const string& outQ) {
        routingTable.addEntry(dest, outQ);
    }

    bool removeRoutingEntry(const string& dest) {
        return routingTable.removeEntry(dest);
    }

    void printRoutingTable() {
        routingTable.printTable();
    }
};



















void getting_routes(LinearRoutingTable& rt, const string& filename)
{
    ifstream file(filename);


    if (!file.is_open())
    {
        cout << "Error opening file: " << filename << endl;
        return;
    }

    string line;
    while (getline(file, line))
    {
        if (line.empty()) continue;

        bool found = false;
        for (size_t i = 0; i < line.length(); i++)
        {
            if (line[i] == ':')
            {
                string dest = line.substr(0, i);
                string router = line.substr(i + 1);
                rt.addEntry(dest, router);
                found = true;
                break;
            }
        }
        if (!found)
        {
            cout << "Invalid routing table entry: " << line << endl;
        }



    }


}

Message* parseMessageFromString(const string& str)
{
    const char* ptr = str.c_str();
    char field[256];
    int field_idx = 0;
    int id = 0, priority = 0;
    string srcadd, destAddr, pyl;

    while (*ptr) {
        if (*ptr == ':') {
            field[field_idx] = '\0';
            if (strncmp(field, "ID", 2) == 0)
            {
                id = atoi(field + 3);
            }
            else if (strncmp(field, "PRIORITY", 8) == 0)
            {
                priority = atoi(field + 9);
            }
            else if (strncmp(field, "SRC", 3) == 0)
            {
                srcadd = string(field + 4);
            }
            else if (strncmp(field, "DEST", 4) == 0)
            {
                destAddr = string(field + 5);
            }
            else if (strncmp(field, "pyl", 7) == 0)
            {
                pyl = string(field + 8);
            }
            field_idx = 0;
            ptr++;
            continue;
        }



        field[field_idx++] = *ptr++;





    }


    if (field_idx > 0)
    {
        field[field_idx] = '\0';
        if (strncmp(field, "pyl", 7) == 0)
        {
            pyl = string(field + 8);
        }
    }

    if (id != 0 && !srcadd.empty() && !destAddr.empty())
    {
        return new Message(id, priority, srcadd, destAddr, pyl);
    }
    return nullptr;
}







void getting_messages(PRIQ& q, const string& filename)
{

    ifstream file(filename);
    string line;


    if (!file.is_open()) {
        cout << "Error opening file!: " << filename << endl;
        return;

    }


    while (getline(file, line)) {

        if (!line.empty())
        {

            Message* m = new Message(line);
            q.enQ(m);
        }

    }



}


int getsrcidx(const string& id) {
    for (int i = 0; i < deviceCount; i++)

    {
        if (devices[i].id == id)
        {
            return i;
        }
    }
    return -1;
}


void printPaths(const string& filePath = "C:\\Users\\Taiba\\source\\repos\\Data St. Project\\Data St. Project\\path.txt")
{
    ifstream file(filePath);
    if (!file.is_open())
    {
        cout << "UNABLE TO OPEN FILE" << filePath << endl;
        return;
    }

    cout << "All Message Paths" << endl;
    string line;
    while (getline(file, line))
    {
        cout << line << endl;
    }
    file.close();




}



void buildRoutingTablesFromPaths(Router* rts[], int noofrt, const string& pathFile = "C:\\Users\\Taiba\\source\\repos\\Data St. Project\\Data St. Project\\path.txt") {
    ifstream file(pathFile);
    if (!file.is_open()) {
        cout << "Error: Could not open path file!" << endl;
        return;
    }

    const int MAX_PATH_LENGTH = 20;

    string pathParts[MAX_PATH_LENGTH];
    string line;

    while (getline(file, line))
    {
        if (line.empty()) continue;




        int partCount = 0;
        size_t start = 0;
        size_t end = line.find(':');

        // Split into parts
        while (end != string::npos && partCount < MAX_PATH_LENGTH)
        {
            pathParts[partCount++] = line.substr(start, end - start);
            start = end + 1;
            end = line.find(':', start);
        }
        if (start < line.length() && partCount < MAX_PATH_LENGTH)
        {
            pathParts[partCount++] = line.substr(start);
        }

        // Should have at least ID, SRC, DEST (trc may be empty)
        if (partCount < 3) continue;

        string dest = pathParts[partCount - 1];
        string src = pathParts[1];
        string trc = (partCount > 3) ? pathParts[2] : "";

        // Split trc into individual rts
        string routersInPath[MAX_PATH_LENGTH];
        int routerPathCount = 0;
        start = 0;
        end = trc.find(':');

        while (end != string::npos && routerPathCount < MAX_PATH_LENGTH) {
            routersInPath[routerPathCount++] = trc.substr(start, end - start);
            start = end + 1;
            end = trc.find(':', start);
        }
        if (start < trc.length() && routerPathCount < MAX_PATH_LENGTH)
        {
            routersInPath[routerPathCount++] = trc.substr(start);
        }

        // Build routing entries
        for (int i = 0; i < routerPathCount; i++)
        {
            string currentRouter = routersInPath[i];

            // Find this router in our router list
            for (int r = 0; r < noofrt; r++)
            {
                if (rts[r]->getId() == currentRouter)
                {
                    // The next hop is either:
                    // 1. The next router in path, or
                    // 2. The destination if this is the last router
                    string nextHop;
                    if (i < routerPathCount - 1)
                    {
                        nextHop = routersInPath[i + 1];
                    }
                    else
                    {
                        nextHop = dest;
                    }

                    // Add to routing table
                    rts[r]->addRoutingEntry(dest, nextHop);
                    break;
                }
            }
        }
    }
    file.close();
}







string extract(const string& str, char delimiter, int& position)
{
    int start = position;
    while (position < str.length() && str[position] != delimiter)
    {
        position++;
    }
    return str.substr(start, position - start);
}
int parse(const string& str)
{
    int output = 0;
    for (char a : str)
    {
        if (isdigit(a))
        {
            output = output * 10 + (a - '0');
        }
    }
    return output;
}
void changeEdgeW(const string& from, const string& to, int newW)
{
    int a = getsrcidx(from);
    int b = getsrcidx(to);
    if (a == -1 || b == -1)
    {
        cout << "devices not found" << endl;
        return;
    }
    adjacency[a][b] = adjacency[b][a];
    adjacency[b][a] = newW;
    cout << " Edge weight changed between " << from << " and " << to << " to " << newW << endl;
}
void EdgeParse(const string& line)
{
    int position = 0;
    string from = extract(line, ',', position);

    position++;
    string to = extract(line, ',', position);

    position++;
    string weight = line.substr(position);

    if (!from.empty() && !to.empty() && !weight.empty())
    {
        int W = parse(weight);
        changeEdgeW(from, to, W);
    }
    else
    {
        cout << "Invalid " << endl;
    }
}
void weightCSV(const string& fileN)
{
    ifstream file(fileN);
    if (!file.is_open())
    {
        cout << "Error" << endl;
        return;
    }

    char l[100];

    while (file.getline(l, sizeof(l)))
    {
        if (strlen(l) > 0)
        {
            EdgeParse(l);
        }
    }
}



int calshortestpath(int dist[], bool visited[], int deviceCount)
{
    int min = INF, min_index = -1;
    for (int v = 0; v < deviceCount; v++)
    {
        if (!visited[v] && dist[v] <= min)
        {
            min = dist[v];
            min_index = v;
        }
    }
    return min_index;
}


void rebuildRoutingTablesEdge(Router* routers[], int routerCount)
{
    cout << "Rebuilding routing tables..." << endl;

    for (int r = 0; r < routerCount; r++) {
        routers[r]->routingTable.clearEntries();  // Changed from clearEntries()

        int routerIndex = getsrcidx(routers[r]->getId());
        if (routerIndex == -1) continue;

        int parent[MAX_DEVICES];
        bool visited[MAX_DEVICES] = { false };

        for (int i = 0; i < deviceCount; i++) {
            dist[i] = INF;
            parent[i] = -1;
        }
        dist[routerIndex] = 0;

        for (int count = 0; count < deviceCount - 1; count++) {
            int u = calshortestpath(dist, visited, deviceCount);
            if (u == -1) break;
            visited[u] = true;

            for (int v = 0; v < deviceCount; v++) {
                if (!visited[v] && adjacency[u][v] && dist[u] != INF
                    && dist[u] + adjacency[u][v] < dist[v]) {
                    dist[v] = dist[u] + adjacency[u][v];
                    parent[v] = u;
                }
            }
        }

        for (int i = 0; i < deviceCount; i++) {
            if (!devices[i].isRouter && dist[i] != INF) {
                int current = i;
                while (parent[current] != -1 && parent[current] != routerIndex) {
                    current = parent[current];
                }

                if (parent[current] != -1) {
                    string nextHop = devices[current].id;
                    routers[r]->addRoutingEntry(devices[i].id, nextHop);
                }
            }
        }
    }
}





void processEdgeChangeCommand(const string& arg, Router* routers[], int routerCount)
{
    if (arg.length() > 4 && arg.find(".csv") == arg.length() - 4)
    {
        weightCSV(arg);
    }
    else {
        int pos = 0;
        string from = extract(arg, ',', pos);
        pos++;
        string to = extract(arg, ',', pos);
        pos++;
        string weightStr = arg.substr(pos);

        if (!from.empty() && !to.empty() && !weightStr.empty())
        {
            int weight = parse(weightStr);
            changeEdgeW(from, to, weight);
            rebuildRoutingTablesEdge(routers, routerCount);
        }
        else {
            cout << "Invalid format. Use: change edge from,to,weight" << endl;
        }
    }
}






void processCommands(PRIQ& msgQueue, LinearRoutingTable& rt) {
    string choice;
    cout << "Enter commands (send msg, change RT, print path, print RT,change edge, exit):" << endl;
    while (true) {
        cout << "> ";
        getline(cin, choice);

        if (choice.empty()) continue;

        if (choice == "exit")
        {
            break;
        }
        else if (strncmp(choice.c_str(), "send msg", 8) == 0)
        {
            // Handle send message choice
            if (choice.length() > 9 && choice[8] == ' ')
            {
                string arg = choice.substr(9);
                if (arg == "file") {
                    string filename;
                    cout << "Enter message file: ";
                    getline(cin, filename);
                    getting_messages(msgQueue, filename);
                }
                else
                {
                    // Parse message from choice line
                    Message* msg = parseMessageFromString(arg);
                    if (msg)
                    {
                        msgQueue.enQ(msg);
                    }
                    else
                    {
                        cout << "Invalid message format" << endl;
                    }
                }
            }
        }
        else if (strncmp(choice.c_str(), "change RT", 9) == 0)
        {

            if (choice.length() > 10 && choice[9] == ' ')
            {
                string arg = choice.substr(10);
                if (arg == "file")
                {
                    string filename;
                    cout << "Enter routing table file: ";

                    //getline(cin, filename);
                    getting_routes(rt, "C:\\Users\\Taiba\\source\\repos\\Data St. Project\\Data St. Project\\routing.txt");
                }
                else {

                    if (!arg.empty())
                    {
                        if (arg[0] == '+')
                        {
                            bool found = false;
                            for (size_t i = 1; i < arg.length(); i++)
                            {
                                if (arg[i] == ':')
                                {
                                    string dest = arg.substr(1, i - 1);
                                    string router = arg.substr(i + 1);
                                    rt.addEntry(dest, router);
                                    saveRoutingTable(rt, "C:\\Users\\Taiba\\source\\repos\\Data St. Project\\Data St. Project\\routing.txt");
                                    found = true;
                                    break;
                                }
                            }
                        }
                        else if (arg[0] == '-') {
                            string dest = arg.substr(1);
                            if (rt.removeEntry(dest)) {
                                saveRoutingTable(rt, "C:\\Users\\Taiba\\source\\repos\\Data St. Project\\Data St. Project\\routing.txt");
                            }
                        }


                    }

                }
            }
        }
        else if (choice == "print path") {
            printPaths();
        }
        else if (choice == "print RT")
        {
            rt.printTable();




        }
        else if (strncmp(choice.c_str(), "change edge", 11) == 0)
        {
            if (choice.length() > 12)
            {
                string arg = choice.substr(12);
                processEdgeChangeCommand(arg, routers, routerCount);
            }
        }
        else
        {
            cout << "Unknown choice" << endl;
        }
    }




}








bool isnummmm(const string& str)
{
    for (char c : str) {
        if (!isdigit(c)) {
            return false;
        }
    }
    return !str.empty();
}




void addDevice(const string& id, bool isRouter)
{
    if (deviceCount >= MAX_DEVICES)
    {
        cout << "Maximum device limit reached!" << endl;
        return;
    }


    devices[deviceCount].id = id;
    devices[deviceCount].isRouter = isRouter;
    deviceCount++;



}

void parseNetworkCSV(const string& filename)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cout << "Failed to open file: " << filename << endl;
        return;
    }

    string line;
    int row = 0;

    // First pass: read header row
    if (getline(file, line)) {
        stringstream ss(line);
        string cell;
        int col = 0;

        while (getline(ss, cell, ',')) {
            if (col == 0) { // Skip first empty cell
                col++;
                continue;
            }
            bool isRouter = (cell[0] == 'R');
            addDevice(cell, isRouter);
            col++;
        }
    }

    // Second pass: read connection data
    while (getline(file, line) && row < deviceCount) {
        stringstream ss(line);
        string cell;
        int col = 0;

        string sourceDevice;
        getline(ss, sourceDevice, ',');
        if (sourceDevice.empty()) continue;

        int srcidx = getsrcidx(sourceDevice);
        if (srcidx == -1) {
            cout << "Error: Source device not found: " << sourceDevice << endl;
            continue;
        }

        while (getline(ss, cell, ',') && col < deviceCount) {
            if (cell == "?")
            {
                adjacency[srcidx][col] = 0;
            }
            else if (isnummmm(cell)) {
                adjacency[srcidx][col] = stoi(cell);
            }
            else {
                adjacency[srcidx][col] = 0;
                cout << "Warning: Invalid weight '" << cell << "' treated as 0" << endl;
            }
            col++;
        }
        row++;
    }
    file.close();
}






void networkprintingggg() {
    cout << "Network Connections:" << endl;

    cout << "Source\t\tDestinations..." << endl;




    for (int i = 0; i < deviceCount; i++)
    {
        cout << devices[i].id << "\t";
        for (int j = 0; j < deviceCount; j++)
        {
            if (adjacency[i][j] > 0) {

                cout << devices[j].id << "(" << adjacency[i][j] << ") ";
            }
        }
        cout << endl;
    }
}





void shortpathprintingg(int dist[], int parent[], int srcidx, Device devices[], int deviceCount) {
    cout << "Shortest paths from router " << devices[srcidx].id << ":\n";
    cout << "Destination\tDistance\tPath\n";

    for (int i = 0; i < deviceCount; i++) {


        if (i != srcidx && devices[i].isRouter)
        {
            cout << devices[i].id << "\t\t";
            if (dist[i] == INF) {
                cout << "INF\t\tNo path";
            }
            else {
                cout << dist[i] << "\t\t";
                int path[MAX_DEVICES];
                int pathLength = 0;
                int current = i;

                while (current != -1) {
                    path[pathLength++] = current;
                    current = parent[current];
                }

                for (int j = pathLength - 1; j >= 0; j--) {
                    cout << devices[path[j]].id;
                    if (j > 0) cout << " -> ";
                }
            }
            cout << endl;
        }
    }
}


bool checkfromList(const string list[], int s, const string& v)
{
    bool check = false;
    for (int i = 0; i <= s; i++)
    {
        if (list[i] == v)
        {
            check = true;
            return check;
        }
    }
    return check;
}





void phase2RoutingTable(LinearRoutingTable& rt, const string& msgFile)
{
    for (int j = rt.getCount() - 1; j >= 0; j--)
    {
        RoutingEntry* entry = rt.getEntry(j);
        if (entry)
        {
            rt.removeEntry(entry->destination);
        }
    }
    string dest[MAX_DEVICES];
    int destCounter = 0;
    ifstream file(msgFile);
    if (!file.is_open())
    {
        cout << "cant open file " << msgFile << endl;
        return;
    }
    string line;
    int msgCounter = 0;

    while (getline(file, line))
    {
        if (!line.empty())
        {
            msgCounter++;
            Message msg(line);
            cout << "Message " << msgCounter << ": From " << msg.srcadd << " to " << msg.destAddr << endl;

            if (!msg.destAddr.empty() && !checkfromList(dest, destCounter, msg.destAddr))
            {
                dest[destCounter++] = msg.destAddr;
                cout << "  Added destination: " << msg.destAddr << endl;
            }
        }
    }
    file.close();

    cout << "Found " << destCounter << " unique destinations in " << msgCounter << " messages." << endl;

    cout << "Building routing table for router" << endl;

    for (int d = 0; d < destCounter; d++)
    {
        string destId = dest[d];
        int destIndex = getsrcidx(destId);

        if (destIndex == -1)
        {
            cout << "Destination " << destId << " not found " << endl;
            continue;
        }

        int r1Index = getsrcidx("R1");

        if (r1Index == -1)
        {
            cout << " Routernot found in network!" << endl;
            continue;
        }
        int distance[MAX_DEVICES];
        int parent[MAX_DEVICES];
        bool alreadyvisited[MAX_DEVICES] = { false };

        for (int i = 0; i < deviceCount; i++)
        {
            distance[i] = INF;
            parent[i] = -1;
        }

        distance[r1Index] = 0;

        for (int count = 0; count < deviceCount - 1; count++)
        {
            int u = calshortestpath(distance, alreadyvisited, deviceCount);
            if (u == -1) break;

            alreadyvisited[u] = true;

            for (int v = 0; v < deviceCount; v++)
            {
                if (!alreadyvisited[v] && adjacency[u][v] &&
                    distance[u] != INF && distance[u] + adjacency[u][v] < distance[v])
                {
                    distance[v] = distance[u] + adjacency[u][v];
                    parent[v] = u;
                }
            }
        }
        if (distance[destIndex] < INF)
        {
            if (adjacency[r1Index][destIndex] > 0)
            {

                cout << "  Direct connection to " << destId << endl;
                rt.addEntry(destId, destId);
            }
            else
            {

                int curr = destIndex;
                while (parent[curr] != r1Index && parent[curr] != -1)
                {
                    curr = parent[curr];
                }

                if (parent[curr] == r1Index)
                {
                    cout << "  Adding route: " << destId << " via " << devices[curr].id << endl;
                    rt.addEntry(destId, devices[curr].id);
                }
                else
                {
                    cout << "  Error: Path exists but couldn't determine next hop!" << endl;
                }
            }
        }
        else
        {
            cout << "  No path from R1 to " << destId << endl;
        }
    }

    cout << "Routing table completed." << endl;
}





void algdijkstra(int adjacency[MAX_DEVICES][MAX_DEVICES], int srcidx, Device devices[], int deviceCount) {


    int dist[MAX_DEVICES];
    bool visited[MAX_DEVICES];
    int parent[MAX_DEVICES];

    for (int i = 0; i < deviceCount; i++)
    {

        dist[i] = INF;
        visited[i] = false;
        parent[i] = -1;
    }

    dist[srcidx] = 0;

    for (int count = 0; count < deviceCount - 1; count++)
    {
        int u = calshortestpath(dist, visited, deviceCount);
        if (u == -1)
        {
            break;
        }

        visited[u] = true;

        for (int v = 0; v < deviceCount; v++)
        {
            if (!visited[v] && adjacency[u][v] && dist[u] != INF && dist[u] + adjacency[u][v] < dist[v])
            {
                dist[v] = dist[u] + adjacency[u][v];
                parent[v] = u;
            }
        }
    }
    shortpathprintingg(dist, parent, srcidx, devices, deviceCount);
}



void getlistofpaths()
{
    cout << endl << "ALL ROUTER PATHS:" << endl;

    for (int i = 0; i < deviceCount; i++)
    {
        if (devices[i].isRouter)
        {
            cout << "\n=====================================\n";
            algdijkstra(adjacency, i, devices, deviceCount);
        }
    }


}



void playy(PRIQ& msgQueue, Router* rts[], int noofrt, Machine* machines[], int noofmatchine)
{



    while (true)
    {
        if (!msgQueue.isEmpty())
        {
            Message* msg = msgQueue.deQ();



            for (int i = 0; i < noofmatchine; i++)
            {
                if (machines[i]->getId() == msg->srcadd)
                {
                    machines[i]->sendMessage(msg);
                    break;
                }
            }
        }





        for (int i = 0; i < noofmatchine; i++)
        {
            machines[i]->processoutQ();
            machines[i]->processINQ();
        }

        for (int i = 0; i < noofrt; i++)
        {
            rts[i]->processMSG();
            rts[i]->forwardMessages(rts, noofrt, machines, noofmatchine);
        }

        this_thread::sleep_for(chrono::milliseconds(100));

        bool allEmpty = msgQueue.isEmpty();


        for (int i = 0; i < noofmatchine; i++)
        {
            allEmpty = allEmpty && machines[i]->isIdle();
        }
        for (int i = 0; i < noofrt; i++)
        {
            rts[i]->processMSG();
            rts[i]->forwardMessages(rts, noofrt, machines, noofmatchine);



        }
        if (allEmpty)
            break;
    }
}








void shortpath(const string& src, const string& destination)
{


    int srcidx = getsrcidx(src);
    int destidx = getsrcidx(destination);

    if (srcidx == -1 || destidx == -1)
    {
        cout << "Error: Source or destination device not found!" << endl;
        return;
    }



    int dist[MAX_DEVICES];

    bool visited[MAX_DEVICES];


    int parent[MAX_DEVICES];


    for (int i = 0; i < deviceCount; i++)
    {
        dist[i] = INF;
        visited[i] = false;
        parent[i] = -1;
    }

    dist[srcidx] = 0;



    // algdijkstra's algorithm
    for (int count = 0; count < deviceCount - 1; count++)
    {
        int u = calshortestpath(dist, visited, deviceCount);
        if (u == -1) break;

        visited[u] = true;

        for (int v = 0; v < deviceCount; v++)
        {
            if (!visited[v] && adjacency[u][v] && dist[u] != INF && dist[u] + adjacency[u][v] < dist[v])
            {
                dist[v] = dist[u] + adjacency[u][v];
                parent[v] = u;
            }
        }
    }


    cout << "Shortest path from " << src << " to " << destination << ":\n";
    if (dist[destidx] == INF)
    {
        cout << "No path exists!" << endl;
        return;
    }

    cout << "Distance: " << dist[destidx] << "\nPath: ";

    // (destination to source)
    string path[MAX_DEVICES];
    int pathLength = 0;
    int current = destidx;





    while (current != -1)
    {
        path[pathLength++] = devices[current].id;
        current = parent[current];




    }

    ofstream MyFile("C:\\Users\\Taiba\\source\\repos\\Data St. Project\\Data St. Project\\path.txt", ios::app);





    for (int i = pathLength - 1; i >= 0; i--)
    {
        cout << path[i];
        MyFile << path[i];
        if (i > 0)
        {
            cout << " -> ";
            MyFile << " -> ";
        }

    }




    MyFile << endl;
    MyFile.close();
    cout << endl;





}







void path_suggest_from_file(const string& filename)
{


    ifstream file(filename);
    if (!file.is_open())
    {
        cout << "Error opening file: " << filename << endl;
        return;
    }





    const int MAX_LINE_LENGTH = 256;
    char line[MAX_LINE_LENGTH];





    while (file.getline(line, MAX_LINE_LENGTH))
    {
        if (strlen(line) == 0) continue;


        if (strchr(line, '|') != nullptr)
        {



            Message msg(line);
            cout << "Message ID: " << msg.ID << endl;
            shortpath(msg.srcadd, msg.destAddr);



        }
        else
        {



            char* delimiter = strchr(line, ',');



            if (delimiter == nullptr)
            {
                cout << "Invalid format in line: " << line << endl;
                continue;
            }

            *delimiter = '\0';
            string source = line;
            string dest = delimiter + 1;
            shortpath(source, dest);
        }
        cout << endl;
    }

    file.close();
}






























int main()
{

    cout << "Hellllooo worldsss" << endl;

    parseNetworkCSV("C:\\Users\\Taiba\\source\\repos\\Data St. Project\\Data St. Project\\network.csv");
    networkprintingggg();


    getlistofpaths();




    Router* rts[MAX_DEVICES];
    Machine* machines[MAX_DEVICES];
    int noofrt = 0, noofmatchine = 0;

    for (int i = 0; i < deviceCount; i++)
    {
        if (devices[i].isRouter)
        {
            rts[noofrt++] = new Router(devices[i].id);
        }
        else
        {
            machines[noofmatchine++] = new Machine(devices[i].id);
        }
    }



    for (int i = 0; i < deviceCount; i++)
    {
        string sourceId = devices[i].id;
        if (devices[i].isRouter)
        {
            Router* srcRouter = nullptr;
            for (int r = 0; r < noofrt; r++)
            {
                if (rts[r]->getId() == sourceId)
                {
                    srcRouter = rts[r];
                    break;
                }
            }
            for (int j = 0; j < deviceCount; j++)
            {
                if (adjacency[i][j] > 0)
                {
                    srcRouter->addConnection(devices[j].id);
                }
            }
        }
        else {
            Machine* srcMachine = nullptr;
            for (int m = 0; m < noofmatchine; m++)
            {
                if (machines[m]->getId() == sourceId)
                {
                    srcMachine = machines[m];
                    break;
                }
            }
            for (int j = 0; j < deviceCount; j++)
            {
                if (adjacency[i][j] > 0 && devices[j].isRouter)
                {
                    for (int r = 0; r < noofrt; r++)
                    {
                        if (rts[r]->getId() == devices[j].id)
                        {
                            srcMachine->connectToRouter(rts[r]);
                            break;
                        }
                    }
                    break;
                }
            }
        }
    }


    PRIQ msgQueue;
    getting_messages(msgQueue, "C:\\Users\\Taiba\\source\\repos\\Data St. Project\\Data St. Project\\messages.txt");


    path_suggest_from_file("C:\\Users\\Taiba\\source\\repos\\Data St. Project\\Data St. Project\\messages.txt");


    LinearRoutingTable rt;
    getting_routes(rt, "C:\\Users\\Taiba\\source\\repos\\Data St. Project\\Data St. Project\\routing.txt");


    getting_messages(msgQueue, "C:\\Users\\Taiba\\source\\repos\\Data St. Project\\Data St. Project\\messages.txt");
    buildRoutingTablesFromPaths(rts, noofrt);

    playy(msgQueue, rts, noofrt, machines, noofmatchine);

    processCommands(msgQueue, rt);


    cout << endl << "printing routing table:" << endl;
    LinearRoutingTable m;

    rt.printTable();




    for (int i = 0; i < noofrt; i++)
    {
        // Load routing table for each router
        string routerId = rts[i]->getId();
        for (int j = 0; j < deviceCount; j++)
        {
            if (adjacency[getsrcidx(routerId)][j] > 0)
            {

                rts[i]->addRoutingEntry(devices[j].id, devices[j].id);
            }
        }
    }





    cout << "PHASE 2" << endl;

    phase2RoutingTable(rt, "C:\\Users\\Taiba\\source\\repos\\Data St. Project\\Data St. Project\\messages.txt");
    rt.printTable();





    for (int i = 0; i < noofrt; i++)
    {
        delete rts[i];
    }
    for (int i = 0; i < noofmatchine; i++)
    {
        delete machines[i];
    }




    return 0;
}
