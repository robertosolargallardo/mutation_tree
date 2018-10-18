#ifndef _HAPLOID_SIMULATOR_HH_
#define _HAPLOID_SIMULATOR_HH_
#include <glob.hh>
#include <popset.hh>
#include <evlist.hh>
#include <sys/stat.h>

namespace haploid{
class simulator
{
private:
    std::shared_ptr<popset> _popset;
    std::shared_ptr<evlist> _evlist;

public:
    simulator(void);
    simulator(const json&);
    simulator(const simulator&);
    simulator& operator=(const simulator&);
    ~simulator(void);

    void run(const std::string&);
};
};
#endif
