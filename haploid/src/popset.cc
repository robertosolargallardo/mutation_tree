#include <popset.hh>
popset::popset(void)
{
    this->_number_of_genes=0U;
}
popset::popset(const json &_profile)
{
    this->_number_of_genes=_profile["genes"].size();
    this->_pool=std::make_shared<std::unique_ptr<gene[]>>(std::make_unique<gene[]>(this->_number_of_genes));

    for(uint32_t gid=0U; gid<this->_number_of_genes; ++gid)
        {
            (*this->_pool)[gid].type(_profile["genes"][gid]["type"]);
            (*this->_pool)[gid].rate(_profile["genes"][gid]["mutation"]["rate"]);

            switch((*this->_pool)[gid].type())
                {
                case SNP:
                {
                    (*this->_pool)[gid].length(_profile["genes"][gid]["length"].get<uint32_t>());
                    break;
                }
                case STR:
                {
                    (*this->_pool)[gid].length(0U);
                    break;
                }
                default:
                {
                    std::cerr << "error::unknown marker type: " << (*this->_pool)[gid].type() << std::endl;
                    exit(EXIT_FAILURE);
                }
                }
        }
}
popset::popset(const popset &_ps)
{
    this->_number_of_genes=_ps._number_of_genes;
    this->_popset=_ps._popset;
    this->_pool=_ps._pool;
}
popset& popset::operator=(const popset &_ps)
{
    this->_number_of_genes=_ps._number_of_genes;
    this->_popset=_ps._popset;
    this->_pool=_ps._pool;
    return(*this);
}
popset::~popset(void)
{
    this->_popset.clear();
}
void popset::drift(void)
{
    for(auto& p : this->_popset)
        p.second.drift();

    for(uint32_t position=0U; position<this->_number_of_genes; ++position)
        (*this->_pool)[position].contract();
}
void popset::flush(void)
{
    for(auto& p : this->_popset)
        p.second.flush();

    for(uint32_t position=0U; position<this->_number_of_genes; ++position)
        (*this->_pool)[position].flush();
}
void popset::save(const std::string &_directory)
{
    for(uint32_t i=0; i<this->_number_of_genes; ++i)
        {
            std::string filename=_directory+"/"+"gene_"+std::to_string(i)+".json";
            json document=(*this->_pool)[i].save();
            std::ofstream output(filename);
            output << std::setw(4) << document << std::endl;
        }

}
void popset::serialize(const std::string &_directory)
{
    for(uint32_t i=0; i<this->_number_of_genes; ++i)
        {
            std::string filename=_directory+"/"+"tree_"+std::to_string(i)+".bin";
            (*this->_pool)[i].serialize(filename);
        }

    std::ofstream output;
    std::string filename=_directory+"/"+"info.bin";
    output.open(filename,std::ios::binary | std::ios::out);
    output.write((char*)&this->_number_of_genes,sizeof(uint32_t));

    size_t number_of_populations=this->_popset.size();
    output.write((char*)&number_of_populations,sizeof(size_t));

    for(auto& p : this->_popset)
        {
            uint32_t id=p.second.id();
            output.write((char*)&id,sizeof(uint32_t));
            p.second.serialize(_directory);
        }
}
void popset::create(const json &_params)
{
    this->_popset[_params["population"]["name"]]=pop(this->_popset.size(),_params["population"]["size"],this->_number_of_genes,this->_pool);
}
void popset::increment(const json &_params)
{
    this->_popset[_params["source"]["population"]["name"]].increment(_params["source"]["population"]["percentage"]);
}
void popset::decrement(const json &_params)
{
    this->_popset[_params["source"]["population"]["name"]].decrement(_params["source"]["population"]["percentage"]);
}
void popset::split(const json &_params)
{
    std::vector<pop> populations=this->_popset[_params["source"]["population"]["name"]].split(_params["destination"].size());

    for(size_t i=0; i<populations.size(); ++i)
        this->_popset[_params["destination"][i]["population"]["name"]]=populations[i];
    populations.clear();

    this->_popset.erase(this->_popset.find(_params["source"]["population"]["name"]));
}
void popset::stats(const std::string &_directory)
{
    json fstats=json::array();

    std::map<std::string,std::vector<individual>> samples;
    std::vector<individual> summary;

    for(auto& p : this->_popset)
        {
            samples[p.first]=p.second.sample();

            if(this->_popset.size()>1) summary.insert(summary.end(),samples[p.first].begin(),samples[p.first].end());
        }
    if(this->_popset.size()>1) samples["summary"]=summary;

    for(auto& sample : samples)
        {
            for(uint32_t position=0U; position<this->_number_of_genes; ++position)
                {
                    std::map<uint64_t,std::tuple<std::vector<uint32_t>,int>> alleles;

                    for(auto& i : sample.second)
                        {
                            allele_t allele=i.get(position);
                            if(alleles.count(allele->id())==0)
                                alleles[allele->id()]=std::tuple<std::vector<uint32_t>,int>(allele->mutations(),1);
                            else
                                std::get<1>(alleles[allele->id()])++;
                        }
                    fstats.push_back({"population",{{"name",sample.first},{"statistics",statistics::stats(alleles)}}});
                }
        }
    std::ofstream output(_directory+"/stats.json");
    output << std::setw(4) << fstats << std::endl;
    output.close();
}
void popset::mutate(void)
{
    for(uint32_t position=0U; position<this->_number_of_genes; ++position)
        (*this->_pool)[position].mutate();
}
