#include <cassert>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <ctime>

/// <summary>
/// Encrypts or decrypts a string using XOR with the given key.
/// XOR is symmetric, so the same logic applies for both operations.
/// </summary>
/// <param name="source">The string to be transformed (either plaintext or ciphertext)</param>
/// <param name="key">The key used to encrypt or decrypt</param>
/// <returns>The resulting transformed string</returns>
std::string encrypt_decrypt(const std::string& source, const std::string& key)
{
    const auto key_length = key.length();
    const auto source_length = source.length();

    // Ensure both key and source strings are not empty
    assert(key_length > 0 && source_length > 0);

    std::string output = source;

    // Perform XOR operation between source characters and key (key repeats if shorter)
    for (size_t i = 0; i < source_length; ++i)
    {
        output[i] = source[i] ^ key[i % key_length];
    }

    return output;
}

/// <summary>
/// Reads the entire contents of a file into a single string.
/// Supports binary mode for handling any type of data.
/// </summary>
/// <param name="filename">Name of the file to read</param>
/// <returns>File contents as a string, or empty string if reading fails</returns>
std::string read_file(const std::string& filename)
{
    std::ifstream input_file_stream(filename, std::ios::in | std::ios::binary);
    if (!input_file_stream)
    {
        std::cerr << "Unable to open file: " << filename << std::endl;
        return "";
    }

    std::ostringstream ss;
    ss << input_file_stream.rdbuf();
    return ss.str();
}

/// <summary>
/// Writes a string to a file in binary mode.
/// Overwrites existing content if the file already exists.
/// </summary>
/// <param name="filename">Name of the file to write</param>
/// <param name="content">The content to write to the file</param>
void write_file(const std::string& filename, const std::string& content)
{
    std::ofstream output_file_stream(filename, std::ios::out | std::ios::binary);
    if (!output_file_stream)
    {
        std::cerr << "Unable to open file for writing: " << filename << std::endl;
        return;
    }

    output_file_stream << content;
}

/// <summary>
/// Compares the contents of two files and checks for exact byte-by-byte match.
/// Used to verify the decrypted file matches the original input file.
/// </summary>
/// <param name="file1">Path to the original file</param>
/// <param name="file2">Path to the decrypted file</param>
/// <returns>True if files are identical, false otherwise</returns>
bool compare_files(const std::string& file1, const std::string& file2)
{
    std::string content1 = read_file(file1);
    std::string content2 = read_file(file2);

    // Check if either file failed to read
    if (content1.empty() || content2.empty())
    {
        std::cerr << "Error: One or both files could not be read for comparison.\n";
        return false;
    }

    // Compare content
    if (content1 == content2)
    {
        std::cout << "SUCCESS: Decrypted file matches original input.\n";
        return true;
    }
    else
    {
        std::cout << "ERROR: Decrypted file does NOT match the original input.\n";
        return false;
    }
}

/// <summary>
/// Main program function that:
/// 1. Reads input data from a file
/// 2. Encrypts the data using XOR
/// 3. Saves the encrypted result
/// 4. Decrypts the encrypted string
/// 5. Saves the decrypted result
/// 6. Verifies the decrypted output matches the original input
/// </summary>
int main()
{
    std::cout << "Encryption and Decryption Program\n";

    // File paths and secret key
    const std::string input_filename = "inputdatafile.txt";
    const std::string encrypted_filename = "encrypted_output.txt";
    const std::string decrypted_filename = "decrypted_output.txt";
    const std::string key = "password";

    // Step 1: Read input from file
    std::string original_content = read_file(input_filename);
    if (original_content.empty())
    {
        std::cerr << "No content read from input file. Exiting." << std::endl;
        return 1;
    }

    // Step 2: Encrypt the input
    std::string encrypted_content = encrypt_decrypt(original_content, key);
    write_file(encrypted_filename, encrypted_content);
    std::cout << "Encrypted file saved as: " << encrypted_filename << std::endl;

    // Step 3: Decrypt the encrypted content
    std::string decrypted_content = encrypt_decrypt(encrypted_content, key);
    write_file(decrypted_filename, decrypted_content);
    std::cout << "Decrypted file saved as: " << decrypted_filename << std::endl;

    // Step 4: Compare decrypted file with original
    compare_files(input_filename, decrypted_filename);

    return 0;
}