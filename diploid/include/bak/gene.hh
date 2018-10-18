#ifndef _DIPLOID_GENE_HH_
#define _DIPLOID_GENE_HH_
#include <glob.hh>
#include <node.hh>

namespace diploid{
typedef std::shared_ptr<node> allele_t;
enum dna_t {SNP=0U,STR=1U,UNKNOWN};

class gene
{
private:
    dna_t      _type;
    double 		_rate;
    uint32_t 	_length;

    allele_t 				  _root;
    std::vector<allele_t> _alleles;

public:
    gene(void);
    gene(const gene&);
    gene& operator=(const gene&);
    ~gene(void);

    void rate(const double&);
    void length(const uint32_t&);
    void type(const dna_t&);

    double rate(void) const;
    uint32_t length(void) const;
    dna_t type(void) const;

    void insert(const allele_t&);
    void contract(void);
    void flush(void);
    void mutate(void);

    allele_t create(void);
    allele_t random(void);

    void serialize(const std::string&) const;

    json save(void);
};
};
#endif
