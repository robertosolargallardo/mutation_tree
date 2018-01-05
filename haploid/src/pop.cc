#include <pop.hh>
pop::pop(void)
{
    this->_id=0U;
    this->_pool=nullptr;
    this->_population_size=0U;
    this->_number_of_genes=0U;
	 this->_mutant=0U;
}
pop::pop(const pop &_p)
{
    this->_id=_p._id;
    this->_pool=_p._pool;
    this->_index=_p._index;
    this->_individuals=_p._individuals;
    this->_population_size=_p._population_size;
    this->_number_of_genes=_p._number_of_genes;
	 this->_mutant=_p._mutant;
}
pop& pop::operator=(const pop &_p)
{
    this->_id=_p._id;
    this->_pool=_p._pool;
    this->_index=_p._index;
    this->_individuals=_p._individuals;
    this->_population_size=_p._population_size;
    this->_number_of_genes=_p._number_of_genes;
	 this->_mutant=_p._mutant;
    return(*this);
}
pop::~pop(void)
{
    this->_index.clear();
    this->_individuals.clear();
}
pop::pop(const uint32_t &_id,const uint32_t &_population_size,const uint32_t &_number_of_genes,const std::shared_ptr<std::unique_ptr<gene[]>> &_pool)
{
    this->_id=_id;
    this->_pool=_pool;
    this->_population_size=_population_size;
    this->_number_of_genes=_number_of_genes;
	 this->_mutant=_population_size;

    this->_index.reserve(this->_population_size);
    this->_individuals.reserve(this->_population_size);

    for(uint32_t i=0U; i<this->_population_size; ++i)
        {
            individual offspring(this->_number_of_genes);
            for(uint32_t position=0U; position<this->_number_of_genes; ++position)
                offspring.set(position,(*this->_pool)[position].create());
            this->_individuals.push_back(offspring);
            this->_index.push_back(i);
        }
}
uint32_t pop::id(void) const
{
    return(this->_id);
}
std::tuple<uint32_t,uint32_t*> pop::mutations(void)
{
    static thread_local std::mt19937 rng(time(0));

    uint32_t number_of_mutations=0U;
    uint32_t *mutations_per_gene=new uint32_t[this->_number_of_genes];
    uint32_t t=this->_population_size;

    for(uint32_t i=0U; i<this->_number_of_genes; ++i)
        {
            switch((*this->_pool)[i].type())
                {
                case SNP:
                {
                    t=this->_population_size*(*this->_pool)[i].length();
                    break;
                }
                case STR:
                {
                    break;
                }
                default:
                {
                    std::cerr << "error::unknown marker type: " << (*this->_pool)[i].type() << std::endl;
                    exit(EXIT_FAILURE);
                }
                }
            std::binomial_distribution<uint32_t> binomial(t,(*this->_pool)[i].rate());
            mutations_per_gene[i]=binomial(rng);
            number_of_mutations+=mutations_per_gene[i];
        }
    return(std::tuple<uint32_t,uint32_t*>(number_of_mutations,mutations_per_gene));
}
void pop::drift(void)
{
    static thread_local std::mt19937 rng(time(0));
    std::uniform_int_distribution<uint32_t> uniform(0U,this->_population_size-1U);

    uint32_t number_of_mutations=0U;
    uint32_t *mutations_per_gene=nullptr;

    std::tie<uint32_t,uint32_t*>(number_of_mutations,mutations_per_gene)=this->mutations();

    uint32_t id=0U,position=0U,mutations=0U;

    for(uint32_t i=0U; i<this->_population_size; ++i)
        {
            id=this->_index[uniform(rng)];

            if(i<number_of_mutations)
                {
						  if(this->_mutant>=this->_population_size) this->_individuals.push_back(individual(this->_individuals[id]));
						  else this->_individuals[this->_mutant]=this->_individuals[id];

						  this->_individuals[this->_mutant].references(0U);
                    std::shared_ptr<node> allele=std::make_shared<node>();
                    allele->parent(this->_individuals[this->_mutant].get(position));
                    this->_individuals[this->_mutant].get(position)->child(allele);

                    allele->mutate();
                    this->_individuals[this->_mutant].set(position,allele);
                    this->_individuals[this->_mutant].increase();
                    (*this->_pool)[position].insert(allele);

						  ++this->_mutant;
                    ++mutations;
                    if(mutations==mutations_per_gene[position])
                        {
                            ++position;
                            mutations=0U;
                        }
                }
            else
                this->_individuals[id].increase();
        }
	 auto partition=std::partition(this->_individuals.begin(),this->_individuals.end(),[](const individual &i){return(i.references()!=0U);});
	 this->_mutant=std::distance(this->_individuals.begin(),partition);

	 if(this->_individuals.size()>this->_population_size){
       auto end=this->_individuals.begin();
		 std::advance(end,this->_population_size);
		 this->_individuals.erase(end,this->_individuals.end());
	 }

    delete[] mutations_per_gene;
}
void pop::flush(void)
{
    this->_index.clear();
    for(uint32_t i=0U; i<this->_individuals.size(); ++i)
        {
				if(this->_individuals[i].references()==0U) break;

            for(uint32_t j=0U; j<this->_individuals[i].references(); ++j)
                this->_index.push_back(i);
            this->_individuals[i].references(0U);
        }
}
void pop::serialize(const std::string &_directory)
{
    std::ofstream output;
    size_t number_of_individuals=this->_individuals.size();
    uint32_t references=0U;
    std::string filename=_directory+"/"+"pop_"+std::to_string(this->id())+".bin";

    output.open(filename,std::ios::binary | std::ios::out);
    output.write((char*)&number_of_individuals,sizeof(size_t));
    output.write((char*)&this->_number_of_genes,sizeof(uint32_t));

    for(uint32_t i=0U; i<this->_individuals.size(); ++i)
        {
            references=this->_individuals[i].references();
            output.write((char*)&references,sizeof(uint32_t));
            for(uint32_t gid=0; gid<this->_number_of_genes; ++gid)
                {
                    std::vector<uint32_t> path;
                    this->_individuals[i].get(gid)->path(path);

                    output.write((char*)&gid,sizeof(uint32_t));
                    uint32_t number_of_levels=path.size();
                    output.write((char*)&number_of_levels,sizeof(size_t));
                    for(auto nid : path) output.write((char*)&nid,sizeof(uint32_t));
                }
        }
    output.close();
}
