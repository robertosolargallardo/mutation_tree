#include <glob.hh>
#include <popset.hh>

int main(int argc,char** argv) {
    char c;
    extern char *optarg;
    uint32_t POPULATION_SIZE=0U;
    uint32_t NUMBER_OF_GENERATIONS=0U;
	 json     PROFILE;

    while((c=getopt(argc,argv,"p:g:s:"))!=-1) {
        switch (c) {
        case 'p':
            POPULATION_SIZE=std::stoul(optarg);
            break;
        case 'g':
            NUMBER_OF_GENERATIONS=std::stoul(optarg);
            break;
        case 's': {
				std::ifstream finput(optarg);
				finput >> PROFILE;
            break;
		  }
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
	 popset ps(PROFILE);	 	
	 uint32_t p1=ps.create(POPULATION_SIZE);
	 uint32_t p2=ps.create(POPULATION_SIZE);

    for(uint32_t step=0U; step<NUMBER_OF_GENERATIONS; ++step) ps.drift();

	 /*pop p(POPULATION_SIZE,PROFILE);
    for(uint32_t step=0U; step<NUMBER_OF_GENERATIONS; step++){
		 std::cout << step << std::endl;
		 p.drift();
	 }*/
	
    /*geneset* gs=new geneset(POPULATION_SIZE,LOCUS_LENGTH,MUTATION_RATE);

    std::chrono::steady_clock::time_point start,end;
    start=std::chrono::steady_clock::now();
    for(uint32_t step=0U; step<NUMBER_OF_GENERATIONS; step++) gs->drift();

	 gs->save("test.json");
	 //gs->print();
    end=std::chrono::steady_clock::now();
    std::cout << (std::chrono::duration_cast<std::chrono::milliseconds>(end-start)).count() << std::endl;

    delete gs;*/
    return(0);
}
