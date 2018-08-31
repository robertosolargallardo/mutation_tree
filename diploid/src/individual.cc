#include <individual.hh>
individual::individual(void)
{
    this->_number_of_genes=0U;
}
individual::individual(const uint32_t &_number_of_genes)
{
    this->_number_of_genes=_number_of_genes;
    this->_genes=std::make_unique<std::array<allele_t,2>[]>(this->_number_of_genes);
}
individual::individual(const individual &_i)
{
    this->_number_of_genes=_i._number_of_genes;
    this->_genes=std::make_unique<std::array<allele_t,2>[]>(this->_number_of_genes);
    for(uint32_t i=0U; i<this->_number_of_genes; ++i)
        this->_genes[i]=_i._genes[i];
}
individual& individual::operator=(const individual &_i)
{
    this->_number_of_genes=_i._number_of_genes;
    this->_genes=std::make_unique<std::array<allele_t,2>[]>(this->_number_of_genes);
    for(uint32_t i=0U; i<this->_number_of_genes; ++i)
        this->_genes[i]=_i._genes[i];
    return(*this);
}
individual::~individual(void)
{
    ;
}
void individual::increase(void)
{
    for(uint32_t i=0U; i<this->_number_of_genes; ++i)
        {
            this->_genes[i][0]->increase();
            this->_genes[i][1]->increase();
        }
}
void individual::set(const uint32_t &_position,const uint32_t &_chromosome,const allele_t &_a)
{
    this->_genes[_position][_chromosome]=_a;
}
void individual::set(const uint32_t &_position,const allele_t &_a,const allele_t &_b)
{
    this->_genes[_position][0]=_a;
    this->_genes[_position][1]=_b;
}
void individual::flush(void)
{
    for(uint32_t i=0U; i<this->_number_of_genes; ++i)
        {
            this->_genes[i][0]->references(0U);
            this->_genes[i][1]->references(0U);
        }
}
std::array<allele_t,N_CHROMOSOMES>& individual::get(const uint32_t &_position) const
{
    return(this->_genes[_position]);
}
