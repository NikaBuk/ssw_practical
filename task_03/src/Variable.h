//
// Created by vovan on 29.09.2019.
//

#ifndef LECS_PARS_VARIABLE_H
#define LECS_PARS_VARIABLE_H

#include <string>

class Variable {
public:
    explicit Variable(std::string t_type, std::string t_value,
                      bool t_flagArray) : type(t_type),
        value(t_value), flagArray(t_flagArray) {};
    std::string type;
    std::string value;
    bool flagArray{ false };
    bool hasValue{false};
    int beginArrayValue;
    int endArrayValue;
};


#endif //LECS_PARS_VARIABLE_H
