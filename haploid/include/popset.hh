#ifndef _POPSET_HH_
#define _POPSET_HH_
#include <pop.hh>
#include <glob.hh>

class popset
{
private:
    std::shared_ptr<std::unique_ptr<gene[]>>	_pool;

    std::vector<pop> 			_popset;
    uint32_t                _number_of_genes;

public:
    popset(void);
    popset(const json&);
    popset(const popset&);
    popset& operator=(const popset&);
    ~popset(void);

    uint32_t create(const uint32_t&);
    void drift(void);
    void flush(void);

    void serialize(const std::string&);
	 void save(const std::string&);
};
#endif
