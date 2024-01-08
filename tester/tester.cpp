// tester.cpp
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;
namespace fs = std::filesystem;

// A function to get the name of the program to be tested
string get_program_name() {
  // Loop through the files in the current directory
  for (const auto& entry : fs::directory_iterator(".")) {
    // Get the file name as a string
    string file_name = entry.path().filename().string();
    // Check if the file name starts with "program_" and ends with ".exe"
    if (file_name.find("program_") == 0 &&
        file_name.find(".exe") == file_name.size() - 4) {
      // Return the file name
      return file_name;
    }
  }
  // If no such file is found, return an empty string
  return "";
}

// A function to get the test parameters and expected outputs from the "tests"
// folder
vector<pair<string, string>> get_test_cases() {
  // A vector to store the test cases
  vector<pair<string, string>> test_cases;
  // Loop through the files in the "tests" folder
  for (const auto& entry : fs::directory_iterator("./tests")) {
    // Get the file name as a string
    string file_name = entry.path().filename().string();
    // Check if the file name has no extension
    if (file_name.find(".") == string::npos) {
      // Open the file for reading the test parameters
      ifstream input_file("./tests/" + file_name);
      // Open the file for reading the expected outputs
      ifstream output_file("./tests/" + file_name + ".a");
      // Check if both files are opened successfully
      if (input_file && output_file) {
        // Read the test parameters and expected outputs as strings
        string input, output;
        getline(input_file, input);
        getline(output_file, output);
        // Close the files
        input_file.close();
        output_file.close();
        // Add the test case to the vector
        test_cases.push_back({input, output});
      }
    }
  }
  // Return the vector of test cases
  return test_cases;
}

// A function to run the program to be tested with a given test parameter and
// return the output
string run_program(string program_name, string input) {
  // Create a command to run the program with the input redirected from a
  // temporary file
  string command = program_name + " < temp.txt";
  // Open the temporary file for writing the input
  ofstream temp_file("temp.txt");
  // Check if the file is opened successfully
  if (temp_file) {
    // Write the input to the file
    temp_file << input << endl;
    // Close the file
    temp_file.close();
    // Create a buffer to store the output
    char buffer[128];
    // Create a string to store the output
    string output = "";
    // Create a pipe to execute the command and read the output
    FILE* pipe = _popen(command.c_str(), "r");
    // Check if the pipe is opened successfully
    if (pipe) {
      // Read the output from the pipe
      while (fgets(buffer, 128, pipe) != NULL) {
        output += buffer;
      }
      // Close the pipe
      _pclose(pipe);
    }
    // Remove the temporary file
    remove("temp.txt");
    // Return the output
    return output;
  }
  // If the file is not opened successfully, return an empty string
  return "";
}

// A function to compare the actual output and the expected output and return
// the result
string compare_output(string actual, string expected) {
  // Remove the trailing newline characters from both outputs
  while (actual.back() == '\n') {
    actual.pop_back();
  }
  while (expected.back() == '\n') {
    expected.pop_back();
  }
  // Compare the outputs and return the result
  if (actual == expected) {
    return "Success";
  } else {
    return "Failure";
  }
}

// The main function
int main() {
  // Get the name of the program to be tested
  string program_name = get_program_name();
  // Check if the program name is not empty
  if (program_name != "") {
    // Get the test cases
    vector<pair<string, string>> test_cases = get_test_cases();
    // Check if the test cases are not empty
    if (!test_cases.empty()) {
      // Create the "results" folder if it does not exist
      fs::create_directory("./results");
      // Open the file for writing the results
      ofstream result_file("./results/results.txt");
      // Check if the file is opened successfully
      if (result_file) {
        // Print the program name and the number of test cases
        cout << "Test for " << program_name
             << " | N of Cases: " << test_cases.size() << endl;
        // Loop through the test cases
        for (int i = 0; i < test_cases.size(); i++) {
          // Get the test parameter and the expected output
          string input = test_cases[i].first;
          string expected = test_cases[i].second;
          // Run the program with the test parameter and get the actual output
          string actual = run_program(program_name, input);
          // Compare the actual output and the expected output and get the
          // result
          string result = compare_output(actual, expected);
          // Write the result to the file in the requested format
          result_file << "Test Case " << i + 1 << ": [Status: " << result
                      << "] --> Input: [" << input << "] - Expected: ["
                      << expected << "] - Test Output: [" << actual << "]"
                      << endl;
          // Print the result to the terminal in real time
          cout << "Case " << i + 1 << " - " << result << endl;
        }
        // Close the file
        result_file.close();
        // Print a message to indicate the testing process is done
        cout << "The testing process is done. Please check the results.txt "
                "file in the results folder."
             << endl;
      } else {
        // Print an error message if the file is not opened successfully
        cerr << "Error: Cannot open the results.txt file for writing." << endl;
      }
    } else {
      // Print an error message if the test cases are empty
      cerr << "Error: No test cases found in the tests folder." << endl;
    }
  } else {
    // Print an error message if the program name is empty
    cerr << "Error: No program to be tested found in the current directory."
         << endl;
  }
  // Return 0 to indicate the program is terminated
  return 0;
}
