#ifndef _SIMULATOR_HH_
#define _SIMULATOR_HH_
#include <glob.hh>
#include <popset.hh>
#include <evlist.hh>
#include <sys/stat.h>

namespace diploid{
class Simulator
{
private:
    std::shared_ptr<popset> _popset;
    std::shared_ptr<evlist> _evlist;

public:
    Simulator(void);
    Simulator(const json&);
    Simulator(const Simulator&);
    Simulator& operator=(const Simulator&);
    ~Simulator(void);

    void run(const std::string&);
};
};
#endif
