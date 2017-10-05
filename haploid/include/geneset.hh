#ifndef _GENESET_HH_
#define _GENESET_HH_
#include <glob.hh>
#include <node.hh>
#define N 2
#define N_SAMPLES 16
#define THRESHOLD 0.01

extern std::mt19937 rng;
class geneset {
private:
    std::shared_ptr<node>              _root;
	 std::vector<std::shared_ptr<node>>	_leafs;
	 std::vector<std::shared_ptr<node>> _index;

    uint32_t _population_size;
    uint32_t _locus_length;
    double   _mutation_rate;

    unsigned short type;

public:
    geneset(void);
    geneset(const uint32_t&,const uint32_t&,const double&);
    ~geneset(void);
    void drift(void);
    void rebuild(void);
    uint32_t size(void) const;

	 void save(const std::string&) const;
};
#endif
