#include "lz77.h"

using namespace std;

vector<unsigned char> convert_int_to_bytes(int value) {
    vector<unsigned char> bytes(4);
    for (int i = 0; i < 4; i++)
        bytes.at(3 - i) = (value >> (i * 8));
    return bytes;
}

uint32_t convert_bytes_to_int(unsigned char *bytes) {
    return uint32_t(bytes[0] << 24 | bytes[1] << 16 | bytes[2] << 8 | bytes[3]);
}

void write_vector_to_file(ofstream &file, vector<unsigned char> values) {
    for (const auto &element : values)
        file << element;
}

Tuple longest_matching_substring(string search_buffer,
                                 string look_ahead_buffer) {
    Tuple tuple;
    int idx, matching_length;
    int max_length_so_far = 0;
    for (int i = 0; i < search_buffer.length(); i++) {
        if (search_buffer[i] == look_ahead_buffer[0]) {
            matching_length = 1;
            for (int j = 1; j < look_ahead_buffer.length(); j++) {
                if (search_buffer[j + i] == look_ahead_buffer[j])
                    matching_length++;
                else
                    break;
            }
            if (matching_length >= max_length_so_far) {
                max_length_so_far = matching_length;
                idx = i;
            }
        }
    }
    tuple.offset = max_length_so_far ? (search_buffer.length() - idx) : 0;
    tuple.length = max_length_so_far;
    tuple.next = look_ahead_buffer[max_length_so_far];
    return tuple;
}

void write_tuple_to_file(ofstream &file, Tuple tuple) {
    write_vector_to_file(file, convert_int_to_bytes(tuple.offset));
    write_vector_to_file(file, convert_int_to_bytes(tuple.length));
    file << tuple.next;
}

void write_compressed_file(string output_file, vector<Tuple> tuples) {
    ofstream output_file_stream;
    output_file_stream.open(output_file);
    for (Tuple tuple : tuples)
        write_tuple_to_file(output_file_stream, tuple);
    output_file_stream.close();
}

void compress_with_lz77(string input_file, string output_file) {
    ifstream input_file_stream;
    input_file_stream.open(input_file);
    vector<Tuple> tuples;

    Sliding_Window window;
    window.SEARCH_BUFF_SIZE = 50000; // hard coded values
    window.LOOK_AHEAD_BUFF_SIZE = 50000;

    // intialize buffers
    window.search_buffer = "";
    window.look_ahead_buffer = "";
    char c;
    int i, extra;
    // populate the look ahead buffer first
    for (int i = 0; i < window.LOOK_AHEAD_BUFF_SIZE; i++) {
        input_file_stream.get(c);
        window.look_ahead_buffer.push_back(c);
    }
    while (window.look_ahead_buffer.length()) {
        Tuple tuple = longest_matching_substring(window.search_buffer,
                                                 window.look_ahead_buffer);
        tuples.push_back(tuple);
        // cout << window.search_buffer << " | " << window.look_ahead_buffer <<
        // endl
        // ;

        // slide search buffer
        window.search_buffer.append(
            window.look_ahead_buffer.substr(0, tuple.length + 1));

        if (window.search_buffer.length() > window.SEARCH_BUFF_SIZE)
            window.search_buffer.erase(0, window.search_buffer.length() -
                                              window.SEARCH_BUFF_SIZE);
        // slide look ahead buffer
        i = 0;
        while (!input_file_stream.eof() && i < tuple.length + 1) {
            input_file_stream.get(c);
            if (input_file_stream.eof())
                break;
            window.look_ahead_buffer.push_back(c);
            i++;
        }
        extra = (+tuple.next != 0) ? 1 : 0;
        window.look_ahead_buffer.erase(0, tuple.length + extra);
        // cout << tuple.offset << " " << tuple.length << " " << tuple.next <<
        // endl;
    }
    // cout << window.search_buffer << " | " << window.look_ahead_buffer << endl
    // ;

    input_file_stream.close();
    write_compressed_file(output_file, tuples);
}

vector<Tuple> read_compressed_file_lz77(string input_file) {
    vector<Tuple> encoded_tuples;
    ifstream input_file_stream;
    input_file_stream.open(input_file);
    // cout << "--------------------" << endl;
    char next, c;
    unsigned char length[4];
    unsigned char offset[4];
    while (!input_file_stream.eof()) {
        Tuple tuple;
        for (int i = 0; i < 4; i++) {
            input_file_stream.get(c);
            offset[i] = c;
        }
        for (int j = 0; j < 4; j++) {
            input_file_stream.get(c);
            length[j] = c;
        }
        input_file_stream.get(next);
        tuple.offset = convert_bytes_to_int(offset);
        tuple.length = convert_bytes_to_int(length);
        tuple.next = next;
        if (input_file_stream.eof())
            break;
        encoded_tuples.push_back(tuple);
        // cout << tuple.offset << " " << tuple.length << " " << tuple.next <<
        // endl;
    }
    input_file_stream.close();
    return encoded_tuples;
}
void decompress_with_lz77(string input_file, string output_file) {
    vector<Tuple> encoded_tuples = read_compressed_file_lz77(input_file);
    string decoded = "";
    for (auto tuple : encoded_tuples) {
        if (tuple.length) {
            decoded.append(
                decoded.substr(decoded.length() - tuple.offset, tuple.length));
        }
        decoded.push_back(tuple.next);
    }
    decoded.pop_back();
    ofstream output_file_stream(output_file);
    output_file_stream << decoded;
    output_file_stream.close();
}
