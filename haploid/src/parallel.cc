#include <glob.hh>
#include <popset.hh>
#include <thread>
#include <sys/wait.h>
#include <unistd.h>

void run(const uint32_t &POPULATION_SIZE,const uint32_t &NUMBER_OF_GENERATIONS,const json &PROFILE)
{

    std::chrono::steady_clock::time_point start,end;
    start=std::chrono::steady_clock::now();

	 popset ps(PROFILE);
    uint32_t p1=ps.create(POPULATION_SIZE);

    for(uint32_t step=0U; step<NUMBER_OF_GENERATIONS; ++step)
        {
            ps.drift();
            if(step!=(NUMBER_OF_GENERATIONS-1U)) ps.flush();
        }


    end=std::chrono::steady_clock::now();
    std::cout << (std::chrono::duration_cast<std::chrono::milliseconds>(end-start)).count() << std::endl;
}

int main(int argc,char** argv)
{
    char c;
    extern char *optarg;
    uint32_t POPULATION_SIZE=0U;
    uint32_t NUMBER_OF_GENERATIONS=0U;
    json     PROFILE;
    uint32_t NUMBER_OF_THREADS=0U;
    uint32_t NUMBER_OF_PROCESSES=0U;

    while((c=getopt(argc,argv,"p:g:s:t:r:"))!=-1)
        {
            switch (c)
                {
                case 'p':
                    POPULATION_SIZE=std::stoul(optarg);
                    break;
                case 'g':
                    NUMBER_OF_GENERATIONS=std::stoul(optarg);
                    break;
                case 't':
                    NUMBER_OF_THREADS=std::stoul(optarg);
                    break;
                case 'r':
                    NUMBER_OF_PROCESSES=std::stoul(optarg);
                    break;
                case 's':
                {
                    std::ifstream finput(optarg);
                    finput >> PROFILE;
                    break;
                }
                default:
                    exit(EXIT_FAILURE);
                }
        }
    if(POPULATION_SIZE==0U)
        {
            std::cerr << "Mandatory parameter -p (population size) needed" << std::endl;
            exit(EXIT_FAILURE);
        }
    if(NUMBER_OF_GENERATIONS==0U)
        {
            std::cerr << "Mandatory parameter -g (number of generations) needed" << std::endl;
            exit(EXIT_FAILURE);
        }

    for(uint32_t i=0U; i<NUMBER_OF_PROCESSES; ++i)
        {
            pid_t pid=fork();
            if(pid==0)
                {
                    std::thread t[NUMBER_OF_THREADS];
                    for(uint32_t j=0; j<NUMBER_OF_THREADS; ++j)
                        {
                            t[j]=std::thread(run,POPULATION_SIZE,NUMBER_OF_GENERATIONS,PROFILE);
                            cpu_set_t cpuset;
                            CPU_ZERO(&cpuset);
                            CPU_SET(i*NUMBER_OF_THREADS+j,&cpuset);
                            int rc=pthread_setaffinity_np(t[j].native_handle(),sizeof(cpu_set_t),&cpuset);
                        }
                    for(uint32_t j=0; j<NUMBER_OF_THREADS; ++j)
                        t[j].join();
                    break;
                }
        }
    if(getpid()!=0)
        {
            pid_t pid;
            int status=0;
            while((pid=wait(&status))>0);
        }

    return(0);
}
