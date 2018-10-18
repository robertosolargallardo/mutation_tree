#ifndef _DIPLOID_INDIVIDUAL_HH_
#define _DIPLOID_INDIVIDUAL_HH_
#include <glob.hh>
#include <gene.hh>
#define N_CHROMOSOMES 2
namespace diploid{
class individual
{
private:
    uint32_t _number_of_genes;
    std::unique_ptr<std::array<allele_t,N_CHROMOSOMES>[]> _genes;

public:
    individual(void);
    individual(const uint32_t&);
    individual(const individual&);
    individual& operator=(const individual&);
    ~individual(void);

    void set(const uint32_t&,const allele_t&,const allele_t&);
    void set(const uint32_t&,const uint32_t&,const allele_t&);

    std::array<allele_t,2>& get(const uint32_t&) const;
    void increase(void);

    void flush(void);
};
};
#endif
