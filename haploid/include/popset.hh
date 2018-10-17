#ifndef _POPSET_HH_
#define _POPSET_HH_
#include <pop.hh>
#include <glob.hh>
#include <statistics.hh>

namespace haploid{
class popset
{
private:
    std::shared_ptr<std::unique_ptr<gene[]>>	_pool;

    std::map<std::string,pop>  _popset;
    uint32_t                	 _number_of_genes;

public:
    popset(void);
    popset(const json&);
    popset(const popset&);
    popset& operator=(const popset&);
    ~popset(void);

    void create(const json&);
    void increment(const json&);
    void decrement(const json&);
    void split(const json&);

    void drift(void);
    void flush(void);

    void mutate(void);

    void serialize(const std::string&);
    void save(const std::string&);

    void stats(const std::string&);
};
};
#endif
