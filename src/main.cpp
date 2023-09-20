/*
Author: Amaan Rahman
Class: ECE6122
Last Date Modified: 09/13/2023
Description:
*/

#include <chrono>
#include <cmath>
#include <condition_variable>
#include <fstream>
#include <functional>
#include <iomanip>
#include <memory>
#include <mutex>
#include <string.h>
#include <thread>
#include "include/auxiliary.h"
#include "include/ECE_ElectricField.h"

// --- Define Aliases --- //
template <typename T>
using coord3D_t = std::tuple<T, T, T>;
template <typename T>
using matrix = std::vector<std::vector<T>>;
using chrono_clock_t = std::chrono::high_resolution_clock;

// --- Function declarations --- //
std::vector<std::string> strsplit(std::string, const char);
template <typename T>
bool validInput(std::vector<std::string> &, std::vector<T> &, std::size_t);
template <typename T>
bool validStringList(std::vector<std::string> &, std::vector<T> &);

// void initQMatrix(int, int, matrix<ECE_ElectricField> &, std::vector<double> &, double);
bool checkOverlap(int, int, matrix<ECE_ElectricField> &, std::vector<double> &);
void computeElectricField(int, int, matrix<ECE_ElectricField> &, std::vector<double> &, std::vector<double> &);
int orderOfMag(float);
void printQMatrix(int, int, matrix<ECE_ElectricField> &, std::ostream &);

// --- Function Definitions --- //
std::vector<std::string> strsplit(std::string str, const char delim)
{
    std::vector<std::string> results;
    if (!str.empty())
    {
        std::string token = "";
        std::istringstream tokenStream(str);

        while (std::getline(tokenStream, token, delim))
        {
            results.push_back(token);
        }
    }
    return results;
}

template <typename T>
bool validInput(std::vector<std::string> &input, std::vector<T> &result, std::size_t sizeLimit)
{
    bool valid = true;

    if (!(std::is_same<T, int>::value || std::is_same<T, double>::value))
    {
        std::cerr << "[FATAL ERROR] Unsupported data type passed to vector container for `result` variable. Expected `int` or `double`.";
        exit(1);
    }
    if (input.size() != sizeLimit)
    {
        std::cerr << "[ERROR] Expected only " << sizeLimit << " inputs delimited by space!" << std::endl;
        valid = false;
    }
    if (!validStringList(input, result))
    {
        std::cerr << "[ERROR] Failed to convert string to appropriate value" << std::endl;
        valid = false;
    }

    return valid;
}

template <typename T>
bool validStringList(std::vector<std::string> &strList, std::vector<T> &result)
{
    bool valid = true;
    T value = 0;

    for (std::string str : strList)
    {
        if (!strToValue(str, value))
        {
            valid = false;
            break;
        }
        result.push_back(value);
    }
    return valid;
}

// void initQMatrix(int N, int M, matrix<ECE_ElectricField> &qMatrix, std::vector<double> &xyDelta, double q)
// {
//     double xDelta = xyDelta.at(0);
//     double yDelta = xyDelta.at(1);
//     double xOffset = (xDelta * (M - 1)) / 2;
//     double yOffset = (yDelta * (N - 1)) / 2;
//     double x = 0, y = 0, z = 0;

//     // iterate rows
//     for (int i = 0; i < N; ++i)
//     {
//         y = -1 * (i * yDelta) + yOffset;

//         // iterate columns
//         std::vector<std::unique_ptr<ECE_ElectricField>> row;
//         row.reserve(M);
//         for (int j = 0; j < M; ++j)
//         {
//             x = (j * xDelta) - xOffset;
//             row.push_back(std::make_unique<ECE_ElectricField>(x, y, z, q));
//         }
//         qMatrix.push_back(std::move(row));
//     }
//     return;
// }

void initQMatrix(int N, int M, matrix<std::tuple<double, double, double>> &qMatrix, std::vector<double> &xyDelta)
{
    double xDelta = xyDelta.at(0);
    double yDelta = xyDelta.at(1);
    double xOffset = (xDelta * (M - 1)) / 2;
    double yOffset = (yDelta * (N - 1)) / 2;
    double x = 0, y = 0, z = 0;

    // iterate rows
    for (int i = 0; i < N; ++i)
    {
        y = -1 * (i * yDelta) + yOffset;

        // iterate columns
        std::vector<std::tuple<double, double, double>> row;
        row.reserve(M);
        for (int j = 0; j < M; ++j)
        {
            x = (j * xDelta) - xOffset;
            std::tuple<double, double, double> loc = std::make_tuple(x, y, z);
            row.push_back(loc);
        }
        qMatrix.push_back(row);
    }
    return;
}

bool checkOverlap(int N, int M, matrix<std::tuple<double, double, double>> &qMatrix, std::vector<double> &pXYZ)
{
    double x, y, z;
    bool valid = false;
    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < M; ++j)
        {
            std::tuple<double, double, double> loc = qMatrix[i][j];
            x = std::get<0>(loc), y = std::get<1>(loc), z = std::get<1>(loc);
            if (pXYZ[0] == x && pXYZ[1] == y && pXYZ[2] == z)
            {
                valid = true;
                goto jump;
            }
        }
    }
jump:
    return valid;
}

// void computeElectricField(int N, int M, matrix<ECE_ElectricField> &qMatrix, std::vector<double> &pXYZ, std::vector<double> &vecE)
// {
//     double Ex = vecE[0], Ey = vecE[1], Ez = vecE[2];
//     double Px = pXYZ[0], Py = pXYZ[1], Pz = pXYZ[2];
//     for (int i = 0; i < M; ++i)
//     {
//         for (int j = 0; j < N; ++j)
//         {
//             qMatrix[i][j]->computeFieldAt(Px, Py, Pz);
//             qMatrix[i][j]->getElectricField(Ex, Ey, Ez);
//             vecE[0] += Ex;
//             vecE[1] += Ey;
//             vecE[2] += Ez;
//         }
//     }
//     return;
// }
std::mutex m;
std::condition_variable cv;
bool ready = false, processed = false, exiting = true;
void workerThread(std::functio)
{
    while ()
        std::unique_lock<std::mutex> lk(m);
    cv.wait(lk, []
            { return ready; });
}

void thread_computeEField(int T, int M, int N, matrix<ECE_ElectricField> &qMatrix, std::vector<double> &pXYZ, std::vector<double> &vecE)
{
    double Ex, Ey, Ez;
    double Px = pXYZ[0], Py = pXYZ[1], Pz = pXYZ[2];
    std::vector<std::thread> threads;
    threads.reserve(T);
    for (int i = 0; i < (M * N); ++i)
    {
    }
}

int orderOfMag(float val)
{
    int order = floor(log10(val));
    return order;
}

// void printQMatrix(int N, int M, matrix<ECE_ElectricField> &qMatrix, std::ostream &file = std::cout)
// {
//     double x, y, z;

//     // (-0.00,-0.00,-0.00)     (-0.00,-0.00,-0.00)
//     //          o --------------------- o
//     //          |                       |
//     // (0.00,0.00,0.00)     (0.00,0.00,0.00)
//     //        o ------------------ o
//     //        |                    |
//     // (0.00,0.00,0.00)     (0.00,0.00,0.00)
//     //        o ------------------ o
//     for (int i = 0; i < M; ++i)
//     {
//         for (int j = 0; j < N; ++j)
//         {
//             qMatrix[i][j]->getQLocation(x, y, z);
//             std::string pos = "(" + std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z) + ")";
//             file << std::left << std::setw(19) << std::fixed << std::setprecision(2) << pos;
//             if (j < (M - 1))
//             {
//                 file << "     ";
//                 continue;
//             }
//             file << std::endl;
//         }
//         for (int j = 0; j < M; ++j)
//         {
//             if (j == 0)
//             {
//                 file << "         ";
//             }
//             file << "o";
//             if (j < (M - 1))
//             {
//                 file << " --------------------- ";
//                 continue;
//             }
//             file << std::endl;
//         }
//         if (i < (N - 1))
//         {
//             for (int j = 0; j < M; ++j)
//             {
//                 if (j == 0)
//                 {
//                     file << "         ";
//                 }
//                 file << "|";
//                 if (j < (M - 1))
//                 {
//                     file << "                       ";
//                     continue;
//                 }
//                 file << std::endl;
//             }
//         }
//     }
//     return;
// }

int main()
{
    const char delim = ' ';
    std::string input = "";
    std::vector<int> matrixDims;
    std::vector<double> xyDelta;
    double q = 0;
    bool display = false;

    // get total # of concurrent threads
    unsigned int numThreads = std::thread::hardware_concurrency();
    std::cout << "Your computer supports " << numThreads << " concurrent threads." << std::endl;

    // process N x M matrix
    while (true)
    {
        std::cout << "Please enter the number of rows and columns in the N x M array: ";
        std::getline(std::cin, input);
        std::vector<std::string> split_input = strsplit(input, delim);
        if (validInput(split_input, matrixDims, 2))
        {
            break;
        }
        matrixDims.clear();
    }

    // process (x,y) separation distance
    while (true)
    {
        std::cout << "Please enter the x and y speraration distances in meters: ";
        std::getline(std::cin, input);
        std::vector<std::string> split_input = strsplit(input, delim);
        if (validInput(split_input, xyDelta, 2))
        {
            if (xyDelta.at(0) <= 0 && xyDelta.at(1) <= 0)
            {
                std::cerr << "[ERROR] (x,y) separation distance values must be > 0.0!" << std::endl;
                xyDelta.clear();
                continue;
            }
            break;
        }
    }

    // process charge, q, value in Coulombs
    while (true)
    {
        std::cout << "Please enter the common charge on the points in micro C: ";
        std::cin >> input;
        if (strToValue(input, q))
        {
            break;
        }
        input = "";
    }

    q *= 1e-6; // C -> uC
    int N = matrixDims.at(0);
    int M = matrixDims.at(1);
    matrix<std::tuple<double, double, double>> qMatrix;
    qMatrix.reserve(N);
    initQMatrix(N, M, qMatrix, xyDelta, q);
    // if (display)
    // {
    //     std::ofstream displayFile;
    //     displayFile.open("qGrid.txt");
    //     printQMatrix(N, M, qMatrix, displayFile);
    //     displayFile.close();
    // }

    // compute electric field at point P from plane of charges
    char flag = 'Y';
    std::cout << std::endl;
    while (flag == 'Y')
    {
        int orderEx, orderEy, orderEz, orderEMag;
        std::vector<double> pXYZ, vecE(3);

        std::getline(std::cin, input); // clear input buffer stream
        while (true)
        {
            std::cout << "Please enter the location in space to determine the electric field (x y z) in meters: ";
            std::getline(std::cin, input);
            std::cout << input << std::endl;
            std::vector<std::string> split_input = strsplit(input, delim);
            if (validInput(split_input, pXYZ, 3))
            {
                if (checkOverlap(N, M, qMatrix, pXYZ))
                {
                    std::cerr << "[ERROR] Invalid point provided: point overlaps with one of the charge locations!" << std::endl;
                    pXYZ.clear();
                    continue;
                }
                break;
            }
        }

        // instantiate threads
        std::vector<std::thread> threads;
        threads.reserve(numThreads);
        std::vector<int> ids;
        // int maxDim = std::max(M, N);

        // compute electric field
        const auto start = chrono_clock_t::now();
        // Emag = computeElectricField(N, M, qMatrix, pXYZ, vecE);
        for (unsigned int i = 0; i < (numThreads - 1); ++i)
        {
            std::thread t(thread_computeEField, i, numThreads, M, N, std::ref(qMatrix), std::ref(pXYZ), std::ref(vecE));
            threads.push_back(std::move(t));
        }
        for (std::thread &t : threads)
        {
            t.join();
        }
        threads.clear();
        double Emag = sqrt(pow(vecE[0], 2) + pow(vecE[1], 2) + pow(vecE[2], 2));
        const auto end = chrono_clock_t::now();
        double duration = std::chrono::duration<double>(end - start).count() * 1.0e6;

        // print results
        orderEx = orderOfMag(vecE[0]);
        orderEy = orderOfMag(vecE[1]);
        orderEz = orderOfMag(vecE[2]);
        orderEMag = orderOfMag(Emag);
        std::cout << "The electric field at (" << pXYZ[0] << "," << pXYZ[1] << "," << pXYZ[2] << ") in V/m is" << std::endl;

        std::cout << std::fixed << std::setprecision(4);
        std::cout << "Ex = " << vecE[0] / (pow(10, orderEx)) << " * 10^" << orderEx << std::endl;
        std::cout << "Ey = " << vecE[1] / (pow(10, orderEy)) << " * 10^" << orderEy << std::endl;
        std::cout << "Ez = " << vecE[2] / (pow(10, orderEz)) << " * 10^" << orderEz << std::endl;
        std::cout << "|E| = " << Emag / (pow(10, orderEMag)) << " * 10^" << orderEMag << std::endl;

        std::cout << "The calculation took " << duration << " microsec!" << std::endl;

        std::cout << "Do you want to enter a new location (Y/N)? ";
        std::cin >> flag;
    }
    std::cout << "Bye!" << std::endl;

    return 0;
}
