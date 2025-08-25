#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>
#include <algorithm>

using namespace std;

struct Course {
    string number;              
    string title;               
    vector<string> prereqs;     
};

struct Node {
    Course data;
    Node* left;
    Node* right;
    explicit Node(const Course& c) : data(c), left(nullptr), right(nullptr) {}
};


static inline void rtrim_inplace(string& s) {
    while (!s.empty() && (s.back() == ' ' || s.back() == '\t' || s.back() == '\r' || s.back() == '\n')) s.pop_back();
}
static inline void ltrim_inplace(string& s) {
    size_t i = 0;
    while (i < s.size() && (s[i] == ' ' || s[i] == '\t' || s[i] == '\r' || s[i] == '\n')) ++i;
    if (i) s.erase(0, i);
}
static inline string trim(string s) { ltrim_inplace(s); rtrim_inplace(s); return s; }

static inline string toUpperNoSpaces(const string& s) {
    string out;
    out.reserve(s.size());
    for (char ch : s) {
        if (!isspace(static_cast<unsigned char>(ch))) {
            out.push_back(static_cast<char>(toupper(static_cast<unsigned char>(ch))));
        }
    }
    return out;
}

// split a CSV line by comma
static vector<string> splitCSV(const string& line) {
    vector<string> toks;
    string tok;
    stringstream ss(line);
    while (getline(ss, tok, ',')) {
        toks.push_back(trim(tok));
    }
    
    if (!line.empty() && line.back() == ',') toks.push_back("");
    return toks;
}


Node* insert(Node* root, const Course& c) {
    if (!root) return new Node(c);
    if (c.number < root->data.number) {
        root->left = insert(root->left, c);
    }
    else if (c.number > root->data.number) {
        root->right = insert(root->right, c);
    }
    else {
        // duplicate key: replace data
        root->data = c;
    }
    return root;
}

Node* find(Node* root, const string& numberKey) {
    Node* cur = root;
    while (cur) {
        if (numberKey == cur->data.number) return cur;
        cur = (numberKey < cur->data.number) ? cur->left : cur->right;
    }
    return nullptr;
}

void inOrderPrint(Node* root) {
    if (!root) return;
    inOrderPrint(root->left);
    cout << root->data.number << ", " << root->data.title << "\n\n";
    inOrderPrint(root->right);
}

void destroy(Node* root) {
    if (!root) return;
    destroy(root->left);
    destroy(root->right);
    delete root;
}


bool loadCsvToBST(const string& filename, Node*& root) {
    ifstream in(filename);
    if (!in.is_open()) return false;

    // Replace existing tree if reloading
    destroy(root);
    root = nullptr;

    string line;
    size_t lineNo = 0;
    while (getline(in, line)) {
        ++lineNo;
        
        string raw = line;
        if (trim(raw).empty()) continue;

        vector<string> t = splitCSV(line);
        if (t.size() < 2) {
            
            continue;
        }
        Course c;
        c.number = toUpperNoSpaces(t[0]); // Normalize key for consistent comparisons
        c.title = t[1];

        for (size_t i = 2; i < t.size(); ++i) {
            if (!t[i].empty()) {
                c.prereqs.push_back(toUpperNoSpaces(t[i]));
            }
        }
        root = insert(root, c);
    }
    return true;
}


void printCourse(Node* root, const string& userQuery) {
    if (!root) {
        cout << "Please load the data first (option 1)." << endl;
        return;
    }
    string key = toUpperNoSpaces(userQuery);
    Node* node = find(root, key);
    if (!node) {
        cout << "Course not found." << endl;
        return;
    }

    cout << node->data.number << ", " << node->data.title << "\n\n";

    if (node->data.prereqs.empty()) {
        cout << "Prerequisites: None\n\n";
        return;
    }

    cout << "Prerequisites: ";
    for (size_t i = 0; i < node->data.prereqs.size(); ++i) {
        cout << node->data.prereqs[i];
        if (i + 1 < node->data.prereqs.size()) cout << ", ";
    }
    cout << "\n\n";
}

void printCourseList(Node* root) {
    if (!root) {
        cout << "Please load the data first (option 1)." << endl;
        return;
    }
    cout << "Here is a sample schedule:\n\n";
    inOrderPrint(root);
}


int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    Node* root = nullptr;

    cout << "Welcome to the course planner.\n\n";

    int choice = 0;
    while (true) {
        cout << "1. Load Data Structure.\n";
        cout << "2. Print Course List.\n";
        cout << "3. Print Course.\n";
        cout << "9. Exit\n\n";
        cout << "What would you like to do? ";

        if (!(cin >> choice)) {
            // handle non-integer input
            cin.clear();
            string junk;
            getline(cin, junk);
            cout << "\n";
            cout << "is not a valid option.\n\n"; 
            continue;
        }
        cout << "\n";

        if (choice == 9) {
            cout << "Thank you for using the course planner!\n";
            break;
        }

        switch (choice) {
        case 1: {
            cout << "Enter the file name: ";
            string fname;
            cin >> ws;
            getline(cin, fname);
            cout << "\n";
            if (loadCsvToBST(fname, root)) {
                cout << "Data loaded.\n\n";
            }
            else {
                cout << "Unable to open file: " << fname << "\n\n";
            }
            break;
        }
        case 2:
            printCourseList(root);
            break;
        case 3: {
            if (!root) {
                cout << "Please load the data first (option 1)." << endl;
                break;
            }
            cout << "What course do you want to know about? ";
            string q;
            cin >> ws;
            getline(cin, q);
            cout << "\n";
            printCourse(root, q);
            break;
        }
        default:
            cout << choice << " is not a valid option.\n\n";
            break;
        }
    }

    destroy(root);
    return 0;
}
