
#ifndef PSYDAPT_BASE_HPP
#define PSYDAPT_BASE_HPP

class Base
{
public:
    virtual double next() = 0;
    virtual bool update(int response, double intensity) = 0;
};

#endif
