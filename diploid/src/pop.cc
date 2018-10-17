#include <pop.hh>
namespace diploid{
pop::pop(void)
{
    this->_id=0U;
    this->_pool=nullptr;
    this->_population_size=0U;
    this->_number_of_genes=0U;
}
pop::pop(const pop &_p)
{
    this->_id=_p._id;
    this->_pool=_p._pool;
    this->_src=_p._src;
    this->_dst=_p._dst;
    this->_population_size=_p._population_size;
    this->_number_of_genes=_p._number_of_genes;
}
pop& pop::operator=(const pop &_p)
{
    this->_id=_p._id;
    this->_pool=_p._pool;
    this->_src=_p._src;
    this->_dst=_p._dst;
    this->_population_size=_p._population_size;
    this->_number_of_genes=_p._number_of_genes;
    return(*this);
}
pop::~pop(void)
{
    this->_src.clear();
    this->_dst.clear();
}
pop::pop(const uint32_t &_id,const uint32_t &_population_size,const uint32_t &_number_of_genes,const std::shared_ptr<std::unique_ptr<gene[]>> &_pool)
{
    this->_id=_id;
    this->_pool=_pool;
    this->_population_size=_population_size;
    this->_number_of_genes=_number_of_genes;

    this->_src.reserve(this->_population_size);
    this->_dst.reserve(this->_population_size);

    for(uint32_t i=0U; i<this->_population_size; ++i)
        {
            individual offspring(this->_number_of_genes);
            for(uint32_t position=0U; position<this->_number_of_genes; ++position)
                offspring.set(position,(*this->_pool)[position].create(),(*this->_pool)[position].create());
            this->_src.push_back(offspring);
            this->_dst.push_back(individual(this->_number_of_genes));
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
    std::uniform_int_distribution<int> coin(0,1);

    uint32_t number_of_mutations=0U;
    uint32_t *mutations_per_gene=nullptr;

    std::tie<uint32_t,uint32_t*>(number_of_mutations,mutations_per_gene)=this->mutations();

    uint32_t mutations=0U,position=0U;

    uint32_t pa=0U,pb=0U;

    std::vector<std::array<uint32_t,N_CHROMOSOMES>> p(this->_population_size);
    std::generate(p.begin(),p.end(),[&uniform](void)->std::array<uint32_t,N_CHROMOSOMES> {return(std::array<uint32_t,N_CHROMOSOMES>{uniform(rng),uniform(rng)});});
    std::sort(p.begin(),p.end(),[](const std::array<uint32_t,N_CHROMOSOMES> &a,const std::array<uint32_t,N_CHROMOSOMES> &b)->bool{return(a[0]<b[0]);});

    std::vector<std::array<int,N_CHROMOSOMES>> c(this->_population_size);//TODO AJUSTAR AL NUMERO DE GENES
    std::generate(c.begin(),c.end(),[&coin](void)->std::array<int,N_CHROMOSOMES> {return(std::array<int,N_CHROMOSOMES>{coin(rng),coin(rng)});});

    std::vector<int> m(number_of_mutations);
    std::generate(m.begin(),m.end(),[&coin](void)->int{return(coin(rng));});

    for(uint32_t i=0U; i<this->_population_size; ++i)
        {
            auto start=std::chrono::system_clock::now();

            for(uint32_t j=0U; j<this->_number_of_genes; ++j)
                this->_dst[i].set(j,this->_src[p[i][0]].get(j)[c[i][0]],this->_src[p[i][1]].get(j)[c[i][1]]);

            auto end=std::chrono::system_clock::now();
            pa+=(end-start).count();

            start=std::chrono::system_clock::now();
            if(i<number_of_mutations)
                {
                    allele_t allele=std::make_shared<node>();
                    allele->parent(this->_dst[i].get(position)[m[i]]);
                    this->_dst[i].get(position)[m[i]]->child(allele);
                    allele->mutate();
                    this->_dst[i].set(position,m[i],allele);
                    (*this->_pool)[position].insert(allele);

                    ++mutations;
                    if(mutations==mutations_per_gene[position])
                        {
                            ++position;
                            mutations=0U;
                        }
                }

            end=std::chrono::system_clock::now();
            pb+=(end-start).count();

            this->_dst[i].increase();
        }
    std::cout << pa << " " << pb << std::endl;
    std::swap(this->_src,this->_dst);
    delete[] mutations_per_gene;
}
/*void pop::rebuild(void)
{
    uint32_t prev=0U;
    uint32_t current=this->_index[0];
    uint32_t diff=current-prev;

    for(auto &i : this->_index)
        {
            if(i!=current)
                {
                    current=i;
                    diff=((i-prev)>1U)?diff+(i-prev)-1:diff;
                }
            prev=i;
            i=current-diff;
        }
    this->_last=*this->_index.rbegin();
}
void pop::increment(const double &_percentage)
{
    uint32_t _increment=uint32_t(double(this->_population_size)*_percentage);

    this->_index.reserve(this->_population_size+_increment);
    this->_individuals.reserve(this->_population_size+_increment);

    std::vector<individual>::iterator it=this->_individuals.begin();
    std::advance(it,this->_last);
    for(uint32_t i=0U; i<_increment; ++i)
        {
            individual offspring(this->_number_of_genes);
            for(uint32_t position=0U; position<this->_number_of_genes; ++position)
                offspring.set(position,(*this->_pool)[position].random(),(*this->_pool)[position].random());
            this->_individuals.insert(it++,offspring);
            this->_index.push_back(this->_last++);
        }

    this->_population_size+=_increment;
}
void pop::decrement(const double &_percentage)
{
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

    this->_individuals=individuals;
    this->_population_size-=_decrement;

    this->rebuild();
}

std::vector<pop> pop::split(const uint32_t &_partitions)
{
    static thread_local std::mt19937 rng(time(0));

    std::vector<pop> populations;
    std::vector<std::vector<uint32_t>> index(_partitions);

    std::shuffle(this->_index.begin(),this->_index.end(),rng);

    while(!this->_index.empty())
        {
            index[this->_index.size()%_partitions].push_back(*this->_index.begin());
            this->_index.erase(this->_index.begin());
        }

    for(size_t i=0; i<_partitions; ++i)
        {
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
}*/
std::vector<individual> pop::sample(void)
{
    static thread_local std::mt19937 rng(time(0));
    std::uniform_int_distribution<uint32_t> uniform(0U,this->_population_size-1U);

    std::vector<individual> individuals;

    for(uint32_t i=0U; i<uint32_t(double(this->_population_size)*0.05); ++i)
        individuals.push_back(this->_src[uniform(rng)]);

    return(individuals);
}
void pop::serialize(const std::string &_directory)
{
    /*std::ofstream output;
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
    output.close();*/
}
};
