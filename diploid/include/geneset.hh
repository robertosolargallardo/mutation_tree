#ifndef _GENESET_HH_
#define _GENESET_HH_
#include <glob.hh>
#include <node.hh>
#define N 2

extern std::mt19937 rng;
class geneset {
private:
    std::shared_ptr<node>                             _root;
	 std::vector<std::array<std::shared_ptr<node>,N>>	_leafs;

    uint32_t _population_size;
    uint32_t _locus_length;
    double   _mutation_rate;

public:
    geneset(void);
    geneset(const uint32_t&,const uint32_t&,const double&);
    ~geneset(void);
    void drift(void);
    void rebuild(std::vector<std::array<std::shared_ptr<node>,N>>&);
    uint32_t size(void) const;

	 void save(const std::string&) const;

	 void print(void) const{
		 //for(auto leaf : this->_leafs) std::cout << leaf->references() << std::endl;
	 }
};
#endif
