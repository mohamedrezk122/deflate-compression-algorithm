#include "lz77.h"

using namespace std;

vector<unsigned char> convert_int_to_bytes(int value, int num) {
    vector<unsigned char> bytes(num);
    for (int i = 0; i < num; i++)
        bytes.at(num - 1 - i) = (value >> (i * 8));
    return bytes;
}

uint16_t convert_bytes_to_int(unsigned char *bytes) {
    return uint16_t(bytes[0] << 8 | bytes[1]);
}

uint8_t convert_bytes_to_int8(unsigned char *bytes) {
    return uint8_t(bytes[0]);
}

void write_vector_to_file(ofstream &file, vector<unsigned char> values) {
    for (const auto &element : values)
        file << element;
}

Tuple longest_matching_substring(string search_buffer,
                                 string look_ahead_buffer) {
    Tuple tuple;
    uint16_t idx,m;
    uint8_t matching_length, max_length_so_far = 0;
    for (uint16_t i = 0; i < search_buffer.length(); i++) {
        if (search_buffer[i] == look_ahead_buffer[0]) {
            matching_length = 1;
            for (uint16_t j = 1; j < look_ahead_buffer.length(); j++) {
                if (search_buffer[j + i] == look_ahead_buffer[j])
                    matching_length++;
                else
                    break;
            }
            if (matching_length >= max_length_so_far) {
                max_length_so_far = matching_length;
                idx = i;
            }
            // allow overlapping matches 
            // m = 0 ;
            // for(uint16_t k = max_length_so_far ; k < look_ahead_buffer.length() ; k++){
            //     if (look_ahead_buffer[k] == look_ahead_buffer[m++])
            //         max_length_so_far ++ ;
            //     else
            //         break ;
            // }
        }
    }
    tuple.offset = max_length_so_far ? (search_buffer.length() - idx) : 0;
    tuple.length = max_length_so_far;
    tuple.next = look_ahead_buffer[max_length_so_far];
    return tuple;
}

void write_tuple_to_file(ofstream &file, Tuple tuple) {
    write_vector_to_file(file, convert_int_to_bytes(tuple.offset , 2));
    write_vector_to_file(file, convert_int_to_bytes(tuple.length , 1));
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
    window.SEARCH_BUFF_SIZE = 11500; // hard coded values
    window.LOOK_AHEAD_BUFF_SIZE = 255;

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
        // cout << tuple.offset << " " << tuple.length << " " << tuple.next << endl;
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
    unsigned char length[1];
    unsigned char offset[2];
    while (!input_file_stream.eof()) {
        Tuple tuple;
        for (int i = 0; i < 2; i++) {
            input_file_stream.get(c);
            offset[i] = c;
        }
        for (int j = 0; j < 1; j++) {
            input_file_stream.get(c);
            length[j] = c;
        }
        input_file_stream.get(next);
        tuple.offset = convert_bytes_to_int(offset);
        tuple.length = convert_bytes_to_int8(length);
        tuple.next = next;
        if (input_file_stream.eof())
            break;
        encoded_tuples.push_back(tuple);
        // cout << tuple.offset << " " << tuple.length << " " << tuple.next << endl;
    }
    input_file_stream.close();
    return encoded_tuples;
}
void decompress_with_lz77(string input_file, string output_file) {
    vector<Tuple> encoded_tuples = read_compressed_file_lz77(input_file);
    string decoded = "";
    // cout << "out size:" << int(encoded_tuples.size()) * 4 << endl ; 
    uint8_t length ;
    for (auto tuple : encoded_tuples) {
        length = tuple.length ;
        if(length){

            // there is an overlap 
            // if(length > tuple.offset){
            //     while(length){
            //         decoded.push_back(decoded[decoded.length() - tuple.offset]);
            //         length--;
            //     }
            // }
            // else 
                decoded.append(
                    decoded.substr(static_cast <int>(decoded.length()) - static_cast <int>(tuple.offset), static_cast <int>(length)));
        }
        decoded.push_back(tuple.next);
    }
    // decoded.pop_back();
    ofstream output_file_stream(output_file);
    output_file_stream << decoded;
    output_file_stream.close();
}
