#include <pop.hh>
pop::pop(void) {
    this->_id=0U;
    this->_pool=nullptr;
    this->_population_size=0U;
    this->_number_of_genes=0U;
    this->_last=0U;
}
pop::pop(const pop &_p) {
    this->_id=_p._id;
    this->_pool=_p._pool;
    this->_index=_p._index;
    this->_individuals=_p._individuals;
    this->_population_size=_p._population_size;
    this->_number_of_genes=_p._number_of_genes;
    this->_last=_p._last;
}
pop& pop::operator=(const pop &_p) {
    this->_id=_p._id;
    this->_pool=_p._pool;
    this->_index=_p._index;
    this->_individuals=_p._individuals;
    this->_population_size=_p._population_size;
    this->_number_of_genes=_p._number_of_genes;
    this->_last=_p._last;
    return(*this);
}
pop::~pop(void) {
    this->_index.clear();
    this->_individuals.clear();
}
pop::pop(const uint32_t &_id,const uint32_t &_population_size,const uint32_t &_number_of_genes,const std::vector<individual> &_individuals,const std::vector<uint32_t> &_index,const std::shared_ptr<std::unique_ptr<gene[]>> &_pool) {
    this->_id=_id;
    this->_pool=_pool;
    this->_index=_index;
    this->_individuals=_individuals;
    this->_population_size=_population_size;
    this->_number_of_genes=_number_of_genes;
    this->_last=_individuals.size();
}
pop::pop(const uint32_t &_id,const uint32_t &_population_size,const uint32_t &_number_of_genes,const std::shared_ptr<std::unique_ptr<gene[]>> &_pool) {
    this->_id=_id;
    this->_pool=_pool;
    this->_population_size=_population_size;
    this->_number_of_genes=_number_of_genes;
    this->_last=_population_size;

    this->_index.reserve(this->_population_size);
    this->_individuals.reserve(this->_population_size);

    for(uint32_t i=0U; i<this->_population_size; ++i) {
        individual offspring(this->_number_of_genes);
        for(uint32_t position=0U; position<this->_number_of_genes; ++position)
            offspring.set(position,(*this->_pool)[position].create());
        this->_individuals.push_back(offspring);
        this->_index.push_back(i);
    }
}
uint32_t pop::id(void) const {
    return(this->_id);
}
std::tuple<uint32_t,uint32_t*> pop::mutations(void) {
    static thread_local std::mt19937 rng(time(0));

    uint32_t number_of_mutations=0U;
    uint32_t *mutations_per_gene=new uint32_t[this->_number_of_genes];
    uint32_t t=this->_population_size;

    for(uint32_t i=0U; i<this->_number_of_genes; ++i) {
        switch((*this->_pool)[i].type()) {
        case SNP: {
            t=this->_population_size*(*this->_pool)[i].length();
            break;
        }
        case STR: {
            break;
        }
        default: {
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
void pop::drift(void) {
    static thread_local std::mt19937 rng(time(0));
    std::uniform_int_distribution<uint32_t> uniform(0U,this->_population_size-1U);

    uint32_t number_of_mutations=0U;
    uint32_t *mutations_per_gene=nullptr;

    std::tie<uint32_t,uint32_t*>(number_of_mutations,mutations_per_gene)=this->mutations();

    uint32_t id=0U,position=0U,mutations=0U;

    for(uint32_t i=0U; i<this->_population_size; ++i) {
        id=this->_index[uniform(rng)];

        if(i<number_of_mutations) {
            if(this->_last>=this->_population_size) this->_individuals.push_back(individual(this->_individuals[id]));
            else this->_individuals[this->_last]=this->_individuals[id];

            this->_individuals[this->_last].references(0U);
            std::shared_ptr<node> allele=std::make_shared<node>();
            allele->parent(this->_individuals[this->_last].get(position));
            this->_individuals[this->_last].get(position)->child(allele);

            allele->mutate();
            this->_individuals[this->_last].set(position,allele);
            this->_individuals[this->_last].increase();
            (*this->_pool)[position].insert(allele);

            ++this->_last;
            ++mutations;
            if(mutations==mutations_per_gene[position]) {
                ++position;
                mutations=0U;
            }
        } else
            this->_individuals[id].increase();
    }
    auto partition=std::partition(this->_individuals.begin(),this->_individuals.end(),[](const individual &i) {
        return(i.references()!=0U);
    });
    this->_last=std::distance(this->_individuals.begin(),partition);

    if(this->_individuals.size()>this->_population_size) {
        auto end=this->_individuals.begin();
        std::advance(end,this->_population_size);
        this->_individuals.erase(end,this->_individuals.end());
    }

    delete[] mutations_per_gene;
}
void pop::flush(void) {
    this->_index.clear();
    for(uint32_t i=0U; i<this->_individuals.size(); ++i) {
        if(this->_individuals[i].references()==0U) break;

        for(uint32_t j=0U; j<this->_individuals[i].references(); ++j)
            this->_index.push_back(i);
        this->_individuals[i].references(0U);
    }
}
void pop::rebuild(void) {
    uint32_t prev=0U;
    uint32_t current=this->_index[0];
    uint32_t diff=current-prev;

    for(auto &i : this->_index) {
        if(i!=current) {
            current=i;
            diff=((i-prev)>1U)?diff+(i-prev)-1:diff;
        }
        prev=i;
        i=current-diff;
    }
}
void pop::serialize(const std::string &_directory) {
    std::ofstream output;
    size_t number_of_individuals=this->_individuals.size();
    uint32_t references=0U;
    std::string filename=_directory+"/"+"pop_"+std::to_string(this->id())+".bin";

    output.open(filename,std::ios::binary | std::ios::out);
    output.write((char*)&number_of_individuals,sizeof(size_t));
    output.write((char*)&this->_number_of_genes,sizeof(uint32_t));

    for(uint32_t i=0U; i<this->_individuals.size(); ++i) {
        references=this->_individuals[i].references();
        output.write((char*)&references,sizeof(uint32_t));
        for(uint32_t gid=0; gid<this->_number_of_genes; ++gid) {
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
void pop::increment(const double &_percentage) {
    uint32_t _increment=uint32_t(double(this->_population_size)*_percentage);

    this->_index.reserve(this->_population_size+_increment);
    this->_individuals.reserve(this->_population_size+_increment);

    for(uint32_t i=0U; i<_increment; ++i) {
        individual offspring(this->_number_of_genes);
        for(uint32_t position=0U; position<this->_number_of_genes; ++position)
            offspring.set(position,(*this->_pool)[position].random());
        this->_individuals.push_back(offspring);
        this->_index.push_back(this->_population_size+i);
    }

    this->_population_size+=_increment;
}
void pop::decrement(const double &_percentage) {
    static thread_local std::mt19937 rng(time(0));
    uint32_t _decrement=uint32_t(double(this->_population_size)*_percentage);

    std::shuffle(this->_index.begin(),this->_index.end(),rng);
    for(uint32_t i=0U; i<_decrement; ++i)
        this->_index.erase(this->_index.begin());

    std::sort(this->_index.begin(),this->_index.end());

    std::vector<uint32_t> uniq=this->_index;
    std::vector<uint32_t>::iterator it=std::unique(uniq.begin(),uniq.end());
    uniq.resize(std::distance(uniq.begin(),it));

    std::vector<individual> individuals(this->_index.size());
    for(size_t i=0; i<uniq.size(); ++i)
        individuals[i]=this->_individuals[uniq[i]];

    this->_last=individuals.size();
    this->_individuals=individuals;
    this->_population_size-=_decrement;

    this->rebuild();
}

std::vector<pop> pop::split(const uint32_t &_partitions) {
    static thread_local std::mt19937 rng(time(0));

    std::vector<pop> populations;
    std::vector<std::vector<uint32_t>> index(_partitions);

    std::shuffle(this->_index.begin(),this->_index.end(),rng);

    while(!this->_index.empty()) {
        index[this->_index.size()%_partitions].push_back(*this->_index.begin());
        this->_index.erase(this->_index.begin());
    }

    for(size_t i=0; i<_partitions; ++i) {
        std::sort(index[i].begin(),index[i].end());

        std::vector<uint32_t> uniq=index[i];
        std::vector<uint32_t>::iterator it=std::unique(uniq.begin(),uniq.end());
        uniq.resize(std::distance(uniq.begin(),it));

        std::vector<individual> individuals(index[i].size());
        for(size_t j=0; j<uniq.size(); ++j)
            individuals[j]=this->_individuals[uniq[j]];

        pop p=pop(i,index[i].size(),this->_number_of_genes,individuals,index[i],this->_pool);
        p.rebuild();
        populations.push_back(p);
    }
    return(populations);
}
