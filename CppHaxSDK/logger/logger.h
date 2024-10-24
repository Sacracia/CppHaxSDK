#pragma once

#include <stdint.h>
#include <iostream>
#include <sstream>
#include <fstream>

class Logger : public std::ostream {
public:
    bool m_newLine = true;
    std::ostringstream ss;
public:
    template <class T>
    Logger& operator<<(const T& v)
    {
        std::cout << " << ";
        if (m_newLine) {
            ss << "[STARTOFLINE] ";
            m_newLine = false;
        }
        ss << v;
        return *this;
    }
};

extern Logger logger;

inline std::ostringstream& end(std::ostringstream& o) {
    std::cout << "ENDL\n";
    logger.m_newLine = true;
    logger.ss << " [ENDOFLINE]\n";
    /*std::ofstream file("tmp.txt", std::ios_base::app);
    file << logger.ss.str();
    file.close();*/
    std::cout << logger.ss.str();
    logger.ss.str("");
    return logger.ss;
}

#define LOG logger
#define ENDL logger.endl