#include <glob.hh>
#include <popset.hh>
#include <evlist.hh>
#include <sys/stat.h>

class Simulator
{
private:
    std::shared_ptr<popset> _popset;
    std::shared_ptr<evlist> _evlist;

public:
    Simulator(void);
    Simulator(const json&);
    Simulator(const Simulator&);
    Simulator& operator=(const Simulator&);
    ~Simulator(void);

    void run(const std::string&);
};
Simulator::Simulator(void)
{
    this->_popset=nullptr;
    this->_evlist=nullptr;
}
Simulator::Simulator(const json &_fsettings)
{
    this->_popset=std::make_shared<popset>(_fsettings["individual"]);
    this->_evlist=std::make_shared<evlist>(_fsettings["scenario"]);
}
Simulator::Simulator(const Simulator &_simulator)
{
    this->_popset=std::make_shared<popset>(*_simulator._popset);
    this->_evlist=std::make_shared<evlist>(*_simulator._evlist);
}
Simulator& Simulator::operator=(const Simulator &_simulator)
{
    this->_popset=std::make_shared<popset>(*_simulator._popset);
    this->_evlist=std::make_shared<evlist>(*_simulator._evlist);
    return(*this);
}
Simulator::~Simulator(void)
{

}
void Simulator::run(const std::string &_directory)
{
    uint32_t lvt=0U;

    while(!this->_evlist->empty())
        {
            std::shared_ptr<event> e=this->_evlist->top();

            for(; lvt<e->timestamp(); ++lvt)
                {
                    this->_popset->drift();
                    if(!last(e)) this->_popset->flush();
                }

            switch(e->type())
                {
                case CREATE:
                {
                    this->_popset->create(e->params());
                    break;
                }
                case INCREMENT:
                {
                    this->_popset->increment(e->params());
                    break;
                }
                case DECREMENT:
                {
                    this->_popset->decrement(e->params());
                    break;
                }
                case SPLIT:
                {
                    this->_popset->split(e->params());
                    break;
                }
                case ENDSIM:
                {
                    this->_popset->mutate();
                    break;
                }
                default:
                {
                    std::cerr << "unknown event " << e->type() << std::endl;
                    exit(EXIT_FAILURE);
                }

                }
            this->_evlist->pop();
        }
    this->_popset->stats(_directory);
    this->_popset->save(_directory);
}
void parse(json &_settings)
{
    static thread_local std::mt19937 rng(time(0));

    for(auto& i : _settings)
        {
            if(i.is_object())
                {
                    if(i.count("type") && i["type"]=="random")
                        {
                            switch(util::hash(i["distribution"]["type"]))
                                {
                                case UNIFORM:
                                {
                                    std::regex integer("[0-9]+");
                                    if(std::regex_match(i["distribution"]["params"]["a"].dump(),integer) || std::regex_match(i["distribution"]["params"]["b"].dump(),integer))
                                        {
                                            std::uniform_int_distribution<uint32_t> uniform(i["distribution"]["params"]["a"],i["distribution"]["params"]["b"]);
                                            i=uniform(rng);
                                        }
                                    else
                                        {
                                            std::uniform_real_distribution<double> uniform(i["distribution"]["params"]["a"],i["distribution"]["params"]["b"]);
                                            i=uniform(rng);
                                        }
                                    break;
                                };
                                default:
                                {
                                    std::cerr << "Error::unknown distribution " << i["distribution"]["type"] << std::endl;
                                    exit(EXIT_FAILURE);
                                }
                                }
                        }
                    else if(i.count("type") && i["type"]=="fixed")
                        {
                            i=i["value"];
                        }

                    parse(i);
                }
            else if(i.is_array())
                parse(i);
        }
}
void schedule(json &_fsettings)
{
    static thread_local std::mt19937 rng(time(0));

    std::string directory="results/"+std::to_string((std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())).count());
    const int err=mkdir(directory.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    std::string settings=_fsettings.dump();
    parse(_fsettings);

    std::ofstream output(directory+"/input.json");
    output << std::setw(4) << _fsettings << std::endl;

    std::uniform_int_distribution<uint32_t> uniform(0,_fsettings["scenarios"].size()-1);

    json scenario=_fsettings["scenarios"][uniform(rng)];
    _fsettings.erase("scenarios");
    _fsettings["scenario"]=scenario;

    Simulator sim(_fsettings);
    sim.run(directory);
    _fsettings=json::parse(settings);
}
int main(int argc,char** argv)
{
    char c;
    extern char *optarg;
    json     SETTINGS;
    uint32_t n=0U;

    while((c=getopt(argc,argv,"s:n:"))!=-1)
        {
            switch (c)
                {
                case 's':
                {
                    std::ifstream f_settings(optarg);
                    f_settings >> SETTINGS;
                    break;
                }
                case 'n':
                {
                    n=std::stol(optarg);
                    break;
                }
                default:
                    exit(EXIT_FAILURE);
                }
        }

    for(uint32_t i=0U; i<n; ++i)
        {
            schedule(SETTINGS);
        }

    return(0);
}

