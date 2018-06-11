#include <glob.hh>
#include <popset.hh>
#include <evlist.hh>
void recursive(json &input){

	for(auto& element : input){
		if(element.is_object()){
			if(element.count("type") && element["type"]=="random"){
				std::cout << element <<std::endl;
				element=1;
			}
			recursive(element);
		}
		else if(element.is_array()){
			//std::cout << "array" << std::endl;
			recursive(element);
		}
		else 
			;//std::cout << element << std::endl;
	}
}
int main(int argc,char** argv) {
    char c;
    extern char *optarg;
    json     PROFILE;
    json     SCENARIO;

    while((c=getopt(argc,argv,"p:s:"))!=-1) {
        switch (c) {
        case 'p': {
            std::ifstream finput(optarg);
            finput >> PROFILE;
            break;
        }
        case 's': {
            std::ifstream finput(optarg);
            finput >> SCENARIO;
            break;
        }
        default:
            exit(EXIT_FAILURE);
        }
    }

    /*todo esto tiene q ir en una clase simulador*/
    /*popset ps(PROFILE);
    evlist evl(SCENARIO);
    uint32_t lvt=0U;

    while(!evl.empty()) {
        std::shared_ptr<event> e=evl.top();

        for(; lvt<e->timestamp(); ++lvt) {
            ps.drift();
            if(!last(e)) ps.flush();
        }

        switch(e->type()) {
        case CREATE: {
            ps.create(e->params());
            break;
        }
        case INCREMENT: {
            ps.increment(e->params());
            break;
        }
        case DECREMENT: {
            ps.decrement(e->params());
            break;
        }
        case SPLIT: {
            ps.split(e->params());
            break;
        }
        case ENDSIM: {
            break;
        }
        default: {
            std::cerr << "unknown event " << e->type() << std::endl;
            exit(EXIT_FAILURE);
        }

        }
        evl.pop();
    }
    ps.save("trees");*/
	 recursive(PROFILE);
    std::cout << PROFILE.dump() << std::endl;
    return(0);
}

