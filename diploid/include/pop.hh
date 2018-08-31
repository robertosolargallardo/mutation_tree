#ifndef _POP_HH_
#define _POP_HH_
#include <glob.hh>
#include <node.hh>
#include <gene.hh>
#include <individual.hh>

class pop
{
private:
    std::shared_ptr<std::unique_ptr<gene[]>> 	_pool;

    std::vector<individual> 	_src;
    std::vector<individual> 	_dst;
    uint32_t 						_population_size;
    uint32_t						_number_of_genes;
    uint32_t						_id;

public:
    pop(void);
    pop(const pop&);
    pop& operator=(const pop&);
    pop(const uint32_t&,const uint32_t&,const uint32_t&,const std::shared_ptr<std::unique_ptr<gene[]>>&);
    //pop(const uint32_t&,const uint32_t&,const uint32_t&,const std::vector<individual>&,const std::vector<uint32_t>&,const std::shared_ptr<std::unique_ptr<gene[]>>&);
    ~pop(void);


    void increment(const double&);
    void decrement(const double&);
    std::vector<pop> split(const uint32_t&);

    void drift(void);
    void rebuild(void);
    std::vector<individual> sample(void);

    std::tuple<uint32_t,uint32_t*> mutations(void);
    uint32_t id(void) const;

    void serialize(const std::string&);
};
#endif
