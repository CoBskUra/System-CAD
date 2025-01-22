#pragma once
#include <vector>
#include <string>
#include <filesystem>
#include <glm/fwd.hpp>


static class FileManager {
public:
    bool static doesFolderExist(const char* folderPath) {
        return std::filesystem::exists(folderPath) && std::filesystem::is_directory(folderPath);
    }

    std::vector<std::string> static getFilesNamesInDirectory(const std::string& directoryPath) {
        std::vector<std::string> fileNames;

        for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
            if (entry.is_regular_file()) { 
                fileNames.push_back(entry.path().filename().string());
            }
        }

        return fileNames;
    }

    template <typename T>
    static void  saveVectorToFile(const std::vector<T>& vec, const char* filename) {
        std::ofstream outFile(filename, std::ios::binary);
        if (!outFile) {
            std::cerr << "Nie uda³o siê otworzyæ pliku do zapisu: " << filename << std::endl;
            return;
        }

        // Zapisujemy rozmiar wektora
        size_t size = vec.size();
        outFile.write(reinterpret_cast<const char*>(&size), sizeof(size));

        // Zapisujemy dane wektora
        outFile.write(reinterpret_cast<const char*>(vec.data()), size * sizeof(T));
        outFile.close();
    }

    template <typename T>
    static std::vector<T> loadVectorFromFile(const std::string& filename) {
        std::ifstream inFile(filename, std::ios::binary);
        if (!inFile) {
            std::cerr << "Nie uda³o siê otworzyæ pliku do odczytu: " << filename << std::endl;
            return {};
        }

        // Odczytujemy rozmiar wektora
        size_t size;
        inFile.read(reinterpret_cast<char*>(&size), sizeof(size));

        // Tworzymy wektor i odczytujemy dane
        std::vector<T> vec(size);
        inFile.read(reinterpret_cast<char*>(vec.data()), size * sizeof(T));
        inFile.close();

        return vec;
    }

    // Funkcja do zapisu wektora glm::vec2
    static void saveVec2VectorToFile(const std::vector<glm::vec2>& vec, const std::string& filename) {
        std::ofstream outFile(filename, std::ios::binary);
        if (!outFile) {
            std::cerr << "Nie uda³o siê otworzyæ pliku do zapisu: " << filename << std::endl;
            return;
        }

        // Zapisujemy rozmiar wektora
        size_t size = vec.size();
        outFile.write(reinterpret_cast<const char*>(&size), sizeof(size));

        // Zapisujemy ka¿dy element glm::vec2
        for (const auto& v : vec) {
            outFile.write(reinterpret_cast<const char*>(&v.x), sizeof(float));
            outFile.write(reinterpret_cast<const char*>(&v.y), sizeof(float));
        }

        outFile.close();
    }

    // Funkcja do odczytu wektora glm::vec2
    static std::vector<glm::vec2> loadVec2VectorFromFile(const std::string& filename) {
        std::ifstream inFile(filename, std::ios::binary);
        if (!inFile) {
            std::cerr << "Nie uda³o siê otworzyæ pliku do odczytu: " << filename << std::endl;
            return {};
        }

        // Odczytujemy rozmiar wektora
        size_t size;
        inFile.read(reinterpret_cast<char*>(&size), sizeof(size));

        // Odczytujemy ka¿dy element glm::vec2
        std::vector<glm::vec2> vec(size);
        for (auto& v : vec) {
            inFile.read(reinterpret_cast<char*>(&v.x), sizeof(float));
            inFile.read(reinterpret_cast<char*>(&v.y), sizeof(float));
        }

        inFile.close();
        return vec;
    }
};