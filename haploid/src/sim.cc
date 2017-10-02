#include <glob.hh>
#include <geneset.hh>

int main(int argc,char** argv) {
    char c;
    extern char *optarg;
    uint32_t POPULATION_SIZE=0U;
    uint32_t NUMBER_OF_GENERATIONS=0U;
    uint32_t LOCUS_LENGTH=0U;
    double   MUTATION_RATE=0.0;

    while((c=getopt(argc,argv,"p:g:l:r:"))!=-1) {
        switch (c) {
        case 'p':
            POPULATION_SIZE=std::stoul(optarg);
            break;
        case 'g':
            NUMBER_OF_GENERATIONS=std::stoul(optarg);
            break;
        case 'l':
            LOCUS_LENGTH=std::stoul(optarg);
            break;
        case 'r':
            MUTATION_RATE=std::stod(optarg);
            break;
        default:
            exit(EXIT_FAILURE);
        }
    }
    if(POPULATION_SIZE==0U) {
        std::cerr << "Mandatory parameter -p (population size) needed" << std::endl;
        exit(EXIT_FAILURE);
    }
    if(NUMBER_OF_GENERATIONS==0U) {
        std::cerr << "Mandatory parameter -g (number of generations) needed" << std::endl;
        exit(EXIT_FAILURE);
    }
    if(LOCUS_LENGTH==0U) {
        std::cerr << "Mandatory parameter -l (locus length) needed" << std::endl;
        exit(EXIT_FAILURE);
    }
    if(MUTATION_RATE==0.0) {
        std::cerr << "Mandatory parameter -r (mutation rate) needed" << std::endl;
        exit(EXIT_FAILURE);
    }

    geneset* gs=new geneset(POPULATION_SIZE,LOCUS_LENGTH,MUTATION_RATE);

    std::chrono::steady_clock::time_point start,end;
    start=std::chrono::steady_clock::now();
    for(uint32_t step=0U; step<NUMBER_OF_GENERATIONS; step++) gs->drift();

	 gs->save("test.json");
	 //gs->print();
    end=std::chrono::steady_clock::now();
    std::cout << (std::chrono::duration_cast<std::chrono::milliseconds>(end-start)).count() << std::endl;

    delete gs;
    return(0);
}
