#ifndef PARSER_H
#define PARSER_H
#include <M5StickC.h>

class Parser
{
public:
    Parser();
    bool getCanData(uint8_t id, uint8_t length, uint32_t data);
};

#endif /* PARSER_H */