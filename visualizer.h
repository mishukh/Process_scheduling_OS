#ifndef COLORED_VISUALIZATION_H_INCLUDED
#define COLORED_VISUALIZATION_H_INCLUDED

#include <bits/stdc++.h>
#include "parser.h"

using namespace std;

// ANSI color codes for terminal output
namespace Color {
    const string RESET   = "\033[0m";
    const string BLACK   = "\033[30m";
    const string RED     = "\033[31m";
    const string GREEN   = "\033[32m";
    const string YELLOW  = "\033[33m";
    const string BLUE    = "\033[34m";
    const string MAGENTA = "\033[35m";
    const string CYAN    = "\033[36m";
    const string WHITE   = "\033[37m";
    
    // Background colors
    const string BG_BLACK   = "\033[40m";
    const string BG_RED     = "\033[41m";
    const string BG_GREEN   = "\033[42m";
    const string BG_YELLOW  = "\033[43m";
    const string BG_BLUE    = "\033[44m";
    const string BG_MAGENTA = "\033[45m";
    const string BG_CYAN    = "\033[46m";
    const string BG_WHITE   = "\033[47m";
}

// Assign unique colors to each process
vector<string> generateProcessColors(int process_count) {
    vector<string> processColors;
    const vector<string> availableColors = {
        Color::BG_RED, Color::BG_GREEN, Color::BG_YELLOW, 
        Color::BG_BLUE, Color::BG_MAGENTA, Color::BG_CYAN
    };
    
    for (int i = 0; i < process_count; i++) {
        processColors.push_back(availableColors[i % availableColors.size()]);
    }
    
    return processColors;
}

void printColoredTimeline(int algorithm_index) {
    vector<string> processColors = generateProcessColors(process_count);
    
    // Print time markers
    cout << "   ";
    for (int i = 0; i <= last_instant; i++) {
        cout << i % 10 << " ";
    }
    cout << "\n";
    
    cout << "------------------------------------------------\n";
    
    // Print process timelines with colors
    for (int i = 0; i < process_count; i++) {
        // Fix: Pass processes[i] as a reference to match the function signature
        tuple<string, int, int>& proc = processes[i];
        cout << get<0>(proc) << "     |";
        
        for (int j = 0; j < last_instant; j++) {
            if (timeline[j][i] == '*') {
                // Process is running (show in color)
                cout << processColors[i] << " " << Color::RESET << "|";
            } else if (timeline[j][i] == '.') {
                // Process is waiting
                cout << Color::BG_WHITE << " " << Color::RESET << "|";
            } else {
                // Empty space
                cout << " |";
            }
        }
        cout << " \n";
    }
    
    // Print color legend
    cout << "------------------------------------------------\n";
    cout << "Indicators: ";
    for (int i = 0; i < process_count; i++) {
        // Fix: Access the process name directly from the tuple
        cout << processColors[i] << "  " << Color::RESET << " = " << get<0>(processes[i]) << "  ";
    }
    cout << Color::BG_WHITE << "  " << Color::RESET << " = Waiting\n";
    cout << "------------------------------------------------\n";
}

#endif // COLORED_VISUALIZATION_H_INCLUDED