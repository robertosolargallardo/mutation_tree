#ifndef _HAPLOID_STATISTICS_H_
#define _HAPLOID_STATISTICS_H_
#include <glob.hh>

namespace haploid{
class statistics
{
public:
    typedef std::map<uint64_t,std::tuple<std::vector<uint32_t>,int>> istat_t;
    static std::map<std::string,double> stats(const istat_t &_alleles)
    {
        std::map<std::string,double> stats;
        double N=std::accumulate(_alleles.begin(),_alleles.end(),0.0,[](double value,const istat_t::value_type& a)
        {
            return(value+double(std::get<1>(a.second)));
        });

        stats["number-of-haplotypes"]=statistics::haplotypes(_alleles,N);

        double segregating_sites=statistics::segregating_sites(_alleles);
        std::vector<std::pair<int,int>> differences=statistics::pairwise_differences(_alleles);
        double mean=statistics::mean(differences,N);

        stats["number-of-segregating-sites"]=segregating_sites;
        stats["mean-number-of-pairwise-differences"]=mean;
        stats["variance-number-of-pairwise-differences"]=statistics::variance(differences,N,mean);
        stats["tajima-d-statistics"]=statistics::tajima_d(N,segregating_sites,mean);

        return(stats);
    }
    static double variance(const std::vector<std::pair<int,int>> &_differences,const double &N,const double &mean)
    {
        double sum=0.0;

        for(auto& d : _differences)
            {
                double diff=(d.first-mean);
                sum+=(diff*diff)*d.second;
            }

        return(sum/(N*N));
    }
    static double mean(const std::vector<std::pair<int,int>> &_differences,const double &N)
    {
        double sum=0.0;

        for(auto& d : _differences)
            sum+=d.first*d.second;

        return(sum/(N*N));
    }
    static double haplotypes(const istat_t &_alleles,const double &N)
    {
        double sum=0.0;

        for(auto& a : _alleles)
            {
                double f=double(std::get<1>(a.second))/N;
                sum+=f*f;
            }

        return((N/(N-1.0))*(1.0-sum));
    }
    static double segregating_sites(const istat_t &_alleles)
    {
        std::vector<uint32_t> mutations;

        for(auto& a : _alleles)
            mutations.insert(mutations.end(),std::get<0>(a.second).begin(),std::get<0>(a.second).end());
        std::sort(mutations.begin(),mutations.end());
        std::vector<uint32_t>::iterator it=std::unique(mutations.begin(),mutations.end());

        return(double(std::distance(mutations.begin(),it)));
    }
    static std::vector<std::pair<int,int>> pairwise_differences(const istat_t &_alleles)
    {
        std::vector<std::pair<int,int>> differences;

        for(auto& i : _alleles)
            {
                for(auto& j : _alleles)
                    {
                        std::vector<uint32_t> diff;
                        std::set_difference((std::get<0>(j.second)).begin(),(std::get<0>(j.second)).end(),(std::get<0>(i.second)).begin(),(std::get<0>(i.second)).end(),std::inserter(diff,diff.begin()));
                        differences.push_back(std::make_pair<int,int>(std::get<1>(j.second)*std::get<1>(i.second),diff.size()));
                    }
            }

        return(differences);
    }
    static double tajima_d(const double &n,const double &ss,const double &mpd)
    {
        if( n <= 1 || ss == 0 || mpd <= 0)
            {
                return 0.0;
            }

        double a1 = 0.0;
        double a2 = 0.0;

        for(unsigned int k = 1; k < (unsigned int)n; ++k)
            {
                a1 += 1.0 / (double)k;
                a2 += 1.0 / (double)(k*k);
            }

        double b1 = (n + 1.0) / ( 3.0 * (n - 1.0) );
        double b2 = (2.0 * (n*n + n + 3.0)) / ( (9.0 * n) * (n - 1.0) );
        double c1 = b1 - (1.0 / a1);
        double c2 = b2 + ((n + 2.0) / (a1 * n)) + a2 / (a1 * a1);
        double e1 = c1 / a1;
        double e2 = c2 / (a1*a1 + a2);
        double res = ( (mpd - (ss / a1)) / sqrt(e1*ss + e2*ss*(ss - 1.0)) );

        return res;
    }
};
};
#endif
