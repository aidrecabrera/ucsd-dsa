#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;
namespace fs = std::filesystem;

string get_program_name() {
  for (const auto& entry : fs::directory_iterator(".")) {
    string file_name = entry.path().filename().string();
    if (file_name.find("program_") == 0 &&
        file_name.find(".exe") == file_name.size() - 4) {
      return file_name;
    }
  }
  return "";
}

vector<pair<string, string>> get_test_cases() {
  vector<pair<string, string>> test_cases;
  for (const auto& entry : fs::directory_iterator("./tests")) {
    string file_name = entry.path().filename().string();
    if (file_name.find(".") == string::npos) {
      ifstream input_file("./tests/" + file_name);
      ifstream output_file("./tests/" + file_name + ".a");
      if (input_file && output_file) {
        string input, output;
        getline(input_file, input);
        getline(output_file, output);
        input_file.close();
        output_file.close();
        test_cases.push_back({input, output});
      }
    }
  }
  return test_cases;
}

string run_program(string program_name, string input) {
  string command = program_name + " < temp.txt";
  ofstream temp_file("temp.txt");
  if (temp_file) {
    temp_file << input << endl;
    temp_file.close();
    char buffer[128];
    string output = "";
    FILE* pipe = _popen(command.c_str(), "r");
    if (pipe) {
      while (fgets(buffer, 128, pipe) != NULL) {
        output += buffer;
      }
      _pclose(pipe);
    }
    remove("temp.txt");
    return output;
  }
  return "";
}

string compare_output(string actual, string expected) {
  while (actual.back() == '\n') {
    actual.pop_back();
  }
  while (expected.back() == '\n') {
    expected.pop_back();
  }
  if (actual == expected) {
    return "Success";
  } else {
    return "Failure";
  }
}

int main() {
  string program_name = get_program_name();
  if (program_name != "") {
    vector<pair<string, string>> test_cases = get_test_cases();
    if (!test_cases.empty()) {
      fs::create_directory("./results");
      ofstream result_file("./results/results.txt");
      if (result_file) {
        for (int i = 0; i < test_cases.size(); i++) {
          string input = test_cases[i].first;
          string expected = test_cases[i].second;
          string actual = run_program(program_name, input);
          string result = compare_output(actual, expected);
          result_file << "Test Case " << i + 1 << " --> Input: \"" << input
                      << "\" Output: \"" << result << "\"" << endl;
        }
        result_file.close();
        cout << "The testing process is done. Please check the results.txt "
                "file in the results folder."
             << endl;
      } else {
        cerr << "Error: Cannot open the results.txt file for writing." << endl;
      }
    } else {
      cerr << "Error: No test cases found in the tests folder." << endl;
    }
  } else {
    cerr << "Error: No program to be tested found in the current directory."
         << endl;
  }
  return 0;
}