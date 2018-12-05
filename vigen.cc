#include <vector>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <fstream>
#include <iostream>


static const std::string Alphabet = "abcdefghijklmnopqrstuvwxyz";
static const size_t kLenAlphabet = Alphabet.length();


void usage()
{
    std::cout << "Usage: vigen [OPTION]... [FILE]...\n"
                 "  -k,  create secret key\n"
                 "  -e,  encrypt file\n"
                 "  -d,  decrypt file\n\n"
                 "Example: vigen -k secret_key -e file\n"
                 "         vigen -k secret_key -d file\n";
}

void error_message()
{
    std::cerr << "Try 'vigen -h' for more information.\n" << std::endl;
}

void removeNotAlphabetChars(std::string& key)
{
    if (key.empty()) return;
    key.erase(std::remove_if(key.begin(), key.end(), [](char ch) {
        return !std::isalpha(ch);
    }), key.end());
}

void genKey(std::string& key)
{
    if (key == "-e" || key == "-d") {
        std::cerr << "Enter the secret key\n";
        exit(1);
    }

    removeNotAlphabetChars(key);
    if (key.empty()) {
        std::cout << "    (warning) Press a key on the keyboard!\n";
        exit(1);
    }
}

const std::string encrypt(std::string& raw_text, std::string& key)
{
    std::string encrypted_text;
    if (raw_text.empty() || key.empty()) {
        return encrypted_text;
    }

    std::transform(raw_text.begin(), raw_text.end(), raw_text.begin(), ::tolower);
    std::transform(key.begin(), key.end(), key.begin(), ::tolower);

    const char letter_A = Alphabet[0];
    size_t key_length = key.length();

    for (size_t i = 0, len = raw_text.length(); i < len; ++i) {
        if (!std::isalpha(raw_text[i])) {
            encrypted_text += raw_text[i];
            continue;
        }
        auto pos = ((raw_text[i] - letter_A) + (key[i % key_length] - letter_A)) % kLenAlphabet;
        encrypted_text += Alphabet[pos];
    }

    return encrypted_text + '\n';
}

const std::string decrypt(const std::string& enc_text, const std::string& key)
{
    std::string clear_text;
    const char letter_A = Alphabet[0];
    size_t key_length = key.length();

    for (size_t i = 0, len = enc_text.length(); i < len; ++i) {
        if (!std::isalpha(enc_text[i])) {
            clear_text += enc_text[i];
            continue;
        }
        auto pos = (((enc_text[i] - letter_A) - (key[i % key_length] - letter_A)) + kLenAlphabet) % kLenAlphabet;
        clear_text += Alphabet[pos];
    }

    return clear_text + '\n';
}

void encryptToFile(const std::string& filename, std::string& sec_key)
{
    std::ifstream fromFile(filename);
    if (!fromFile) {
        std::cerr << "Unable to open '" << filename << "': File not found\n";
        exit(1);
    }

    const std::string encrypted_file = filename + ".vig";
    std::ofstream toFile(encrypted_file);

    std::string line;
    while (std::getline(fromFile, line)) {
        toFile << encrypt(line, sec_key);
    }

    toFile.close();
    fromFile.close();
}

void decryptToFile(const std::string& filename, const std::string& sec_key)
{
    std::ifstream fromFile(filename);
    if (!fromFile) {
        std::cerr << "Unable to open '" << filename << "': File not found\n";
        exit(1);
    }

    const std::string decrypted_file = "clear_text.txt";
    std::ofstream toFile(decrypted_file);

    std::string line;
    while (std::getline(fromFile, line)) {
        toFile << decrypt(line, sec_key);
    }

    toFile.close();
    fromFile.close();
}

void setFilename(int argc, char** argv, std::string& filename)
{
    if (argc == 4) {
        std::cout << "> ";
        std::getline(std::cin, filename);
    }
    else if (argc == 5) {
        filename = argv[4];
    }
}

int main(int argc, char** argv)
{
    std::vector<std::string> all_args;
    for (int i = 0; i < argc; ++i)
        all_args.push_back(argv[i]);

    if (argc == 2 && all_args[1] == "-h") {
        usage();
        return 0;
    }
    if (argc < 4 || argc > 5) {
        error_message();
        return 1;
    }

    if (all_args[1] != "-k" && (all_args[3] != "-e" || all_args[3] != "-d"))
        error_message();

    std::string sec_key;
    if (all_args[1] == "-k") {
        sec_key = all_args[2];
        genKey(sec_key);
    }

    std::string filename;
    setFilename(argc, argv, filename);

    if (all_args[3] == "-e") {
        encryptToFile(filename, sec_key);
    } else if (all_args[3] == "-d") {
        decryptToFile(filename, sec_key);
    }

    return 0;
}
