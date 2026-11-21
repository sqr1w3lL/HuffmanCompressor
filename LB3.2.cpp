#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <queue>
#include <fstream>
#include <windows.h>
using namespace std;

struct Node {
    char ch;
    int freq;
    Node* left;
    Node* right;
    Node(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
};

struct Compare {
    bool operator()(Node* a, Node* b) {
        return a->freq > b->freq;
    }
};

class HuffmanCompressor {
private:
    string inputFile;
    string text;
    map<char, string> huffmanCode;
    map<char, int> freq;
    string encodedText;
    string decodedText;

public:
    void setFile(string filename) {
        inputFile = filename;
        text.clear();
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Не вдалося відкрити файл!\n";
            return;
        }
        char c;
        while (file.get(c)) text += c;
        file.close();
        cout << "Файл успішно зчитано! (" << text.size() << " символів)\n";
    }

    void buildFrequencyTable() {
        freq.clear();
        for (char c : text) freq[c]++;
    }

    void generateCodes(Node* root, string str) {
        if (!root) return;
        if (!root->left && !root->right)
            huffmanCode[root->ch] = str;
        generateCodes(root->left, str + "0");
        generateCodes(root->right, str + "1");
    }

    void compress() {
        if (text.empty()) {
            cout << "Немає даних для стискання!\n";
            return;
        }

        buildFrequencyTable();
        priority_queue<Node*, vector<Node*>, Compare> pq;

        for (auto pair : freq) pq.push(new Node(pair.first, pair.second));

        while (pq.size() > 1) {
            Node* left = pq.top(); pq.pop();
            Node* right = pq.top(); pq.pop();
            Node* node = new Node('\0', left->freq + right->freq);
            node->left = left;
            node->right = right;
            pq.push(node);
        }

        Node* root = pq.top();
        generateCodes(root, "");

        encodedText = "";
        for (char c : text) encodedText += huffmanCode[c];

        ofstream out("encoded.txt");
        out << encodedText;
        out.close();

        double originalSize = text.size() * 8;
        double compressedSize = encodedText.size();
        double ratio = (1 - compressedSize / originalSize) * 100;

        cout << "\nРезультати стискання\n";
        cout << "Початковий розмір: " << originalSize << " біт\n";
        cout << "Стиснутий розмір: " << compressedSize << " біт\n";
        cout << "Коефіцієнт стискання: " << fixed << setprecision(2) << ratio << "%\n";

        cout << "\nКоди символів:\n";
        for (auto pair : huffmanCode)
            cout << "'" << pair.first << "' : " << pair.second << endl;

        cout << "\nСтиснення завершено! Дані збережено у 'encoded.txt'\n";
    }

    void decompress() {
        if (encodedText.empty()) {
            ifstream in("encoded.txt");
            if (!in.is_open()) {
                cout << "Немає закодованих даних для декодування!\n";
                return;
            }
            in >> encodedText;
            in.close();
        }

        map<string, char> reverseCode;
        for (auto pair : huffmanCode)
            reverseCode[pair.second] = pair.first;

        decodedText = "";
        string code = "";
        for (char bit : encodedText) {
            code += bit;
            if (reverseCode.find(code) != reverseCode.end()) {
                decodedText += reverseCode[code];
                code = "";
            }
        }

        ofstream out("decoded.txt");
        out << decodedText;
        out.close();

        cout << "\nДекодування завершено! Результат у файлі 'decoded.txt'\n";
        cout << "Відновлений текст:\n" << decodedText.substr(0, 200)
            << (decodedText.size() > 200 ? "..." : "") << endl;
    }
};

int main() {
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    setlocale(LC_ALL, "ukr");

    HuffmanCompressor compressor;
    int choice;
    string filename;

    do {
        cout << "\nМеню Хаффмана\n"
            << "1. Вказати файл для стискання\n"
            << "2. Виконати стискання\n"
            << "3. Виконати декодування\n"
            << "0. Вихід\n"
            << "Ваш вибір: ";
        cin >> choice;

        switch (choice) {
        case 1:
            cout << "Введіть назву файлу: ";
            cin >> filename;
            compressor.setFile(filename);
            break;
        case 2:
            compressor.compress();
            break;
        case 3:
            compressor.decompress();
            break;
        case 0:
            cout << "Вихід із програми.\n";
            break;
        default:
            cout << "Невірний вибір!\n";
        }
    } while (choice != 0);

    return 0;
}
