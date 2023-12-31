#ifndef LZ77_H
#define LZ77_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>


struct Tuple {
    uint16_t offset ;
    uint8_t length ;
    char next  ;
};


struct Sliding_Window {
    uint16_t SEARCH_BUFF_SIZE ;
    uint8_t LOOK_AHEAD_BUFF_SIZE ;
    std::string search_buffer  ;
    std::string look_ahead_buffer ;
};

std::vector<unsigned char> convert_int_to_bytes(int value, int num);

uint16_t convert_bytes_to_int(unsigned char * bytes);
uint8_t convert_bytes_to_int8(unsigned char * bytes);


Tuple longest_matching_substring(std::string search_buffer, std::string look_ahead_buffer);

void write_vector_to_file(std::ofstream& file, std::vector<unsigned char> values);
void write_tuple_to_file(std::ofstream& file, Tuple tuple);

std::vector<Tuple> read_compressed_file_lz77(std::string input_file) ;
void write_compressed_file(std::string output_file, std::vector<Tuple> tuples);

void compress_with_lz77(std::string input_file, std::string output_file);
void decompress_with_lz77(std::string input_file, std::string output_file);

#endif
