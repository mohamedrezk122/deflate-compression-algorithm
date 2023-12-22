#include "lz77.h"

using namespace std;

int main(int argc, char *argv[]) {
    string input_file(argv[1]);
    string base = input_file.substr(0, input_file.length() - 4);
    string output_file = base + ".dat";
    cout << input_file << endl;
    cout << output_file << endl;
    compress_with_lz77(input_file, output_file);
    decompress_with_lz77(output_file, base + "-recovered.txt");
    return 0;
}